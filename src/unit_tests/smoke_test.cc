// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/system/error_code.hpp>
#include <common/backup_paths.h>
#include <common/pkgmgr_interface.h>
#include <common/pkgmgr_registration.h>
#include <common/request.h>
#include <common/step/step_fail.h>
#include <gtest/gtest.h>
#include <gtest/gtest-death-test.h>
#include <pkgmgr-info.h>
#include <signal.h>
#include <unistd.h>
#include <tzplatform_config.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "hybrid/hybrid_installer.h"
#include "wgt/wgt_app_query_interface.h"
#include "wgt/wgt_installer.h"

#define SIZEOFARRAY(ARR)                                                       \
  sizeof(ARR) / sizeof(ARR[0])                                                 \

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

const bf::path kSmokePackagesDirectory =
    "/usr/share/wgt-backend-ut/test_samples/smoke/";

const char kApplicationDir[] = ".applications";
const char kApplicationDirBackup[] = ".applications.bck";
const char KUserAppsDir[] = "apps_rw";
const char KUserAppsDirBackup[] = "apps_rw.bck";

enum class RequestResult {
  NORMAL,
  FAIL,
  CRASH
};

class TestPkgmgrInstaller : public ci::PkgmgrInstallerInterface {
 public:
  bool CreatePkgMgrInstaller(pkgmgr_installer** installer,
                             ci::InstallationMode* mode) {
    *installer = pkgmgr_installer_new();
    if (!*installer)
      return false;
    *mode = ci::InstallationMode::ONLINE;
    return true;
  }

  bool ShouldCreateSignal() const {
    return false;
  }
};

enum class PackageType {
  WGT,
  HYBRID
};

class StepCrash : public ci::Step {
 public:
  using Step::Step;

  ci::Step::Status process() override {
    raise(SIGSEGV);
    return Status::OK;
  }
  ci::Step::Status clean() override { return ci::Step::Status::OK; }
  ci::Step::Status undo() override { return ci::Step::Status::OK; }
  ci::Step::Status precheck() override { return ci::Step::Status::OK; }
};

void RemoveAllRecoveryFiles() {
  bf::path root_path = ci::GetRootAppPath(false);
  if (!bf::exists(root_path))
    return;
  for (auto& dir_entry : boost::make_iterator_range(
         bf::directory_iterator(root_path), bf::directory_iterator())) {
    if (bf::is_regular_file(dir_entry)) {
      if (dir_entry.path().string().find("/recovery") != std::string::npos) {
        bs::error_code error;
        bf::remove(dir_entry.path(), error);
      }
    }
  }
}

bf::path FindRecoveryFile() {
  bf::path root_path = ci::GetRootAppPath(false);
  for (auto& dir_entry : boost::make_iterator_range(
         bf::directory_iterator(root_path), bf::directory_iterator())) {
    if (bf::is_regular_file(dir_entry)) {
      if (dir_entry.path().string().find("/recovery") != std::string::npos) {
        return dir_entry.path();
      }
    }
  }
  return {};
}

bool ValidateFileContentInPackage(const std::string& pkgid,
                                  const std::string& relative,
                                  const std::string& expected) {
  bf::path root_path = ci::GetRootAppPath(false);
  bf::path file_path = root_path / pkgid / relative;
  if (!bf::exists(file_path)) {
    LOG(ERROR) << file_path << " doesn't exist";
    return false;
  }
  FILE* handle = fopen(file_path.c_str(), "r");
  if (!handle) {
    LOG(ERROR) << file_path << " cannot  be open";
    return false;
  }
  std::string content;
  std::array<char, 200> buffer;
  while (fgets(buffer.data(), buffer.size(), handle)) {
    content += buffer.data();
  }
  fclose(handle);
  return content == expected;
}

void ValidatePackageFS(const std::string& pkgid,
                       const std::vector<std::string>& appids) {
  bf::path root_path = ci::GetRootAppPath(false);
  bf::path package_path = root_path / pkgid;
  bf::path data_path = package_path / "data";
  bf::path shared_path = package_path / "shared";
  bf::path cache_path = package_path / "cache";
  ASSERT_TRUE(bf::exists(root_path));
  ASSERT_TRUE(bf::exists(package_path));
  ASSERT_TRUE(bf::exists(data_path));
  ASSERT_TRUE(bf::exists(shared_path));
  ASSERT_TRUE(bf::exists(cache_path));

  bf::path manifest_path =
      bf::path(getUserManifestPath(getuid())) / (pkgid + ".xml");
  ASSERT_TRUE(bf::exists(manifest_path));

  for (auto& appid : appids) {
    bf::path binary_path = package_path / "bin" / appid;
    ASSERT_TRUE(bf::exists(binary_path));
    bf::path icon_path = bf::path(getIconPath(getuid())) / (appid + ".png");
    ASSERT_TRUE(bf::exists(icon_path));
    bf::path icon_backup = ci::GetBackupPathForIconFile(icon_path);
    ASSERT_FALSE(bf::exists(icon_backup));
  }

  bf::path widget_root_path = package_path / "res" / "wgt";
  bf::path config_path = widget_root_path / "config.xml";
  ASSERT_TRUE(bf::exists(widget_root_path));
  ASSERT_TRUE(bf::exists(config_path));

  bf::path private_tmp_path = package_path / "tmp";
  ASSERT_TRUE(bf::exists(private_tmp_path));

  // backups should not exist
  bf::path package_backup = ci::GetBackupPathForPackagePath(package_path);
  bf::path manifest_backup = ci::GetBackupPathForManifestFile(manifest_path);
  ASSERT_FALSE(bf::exists(package_backup));
  ASSERT_FALSE(bf::exists(manifest_backup));
}

void PackageCheckCleanup(const std::string& pkgid,
                         const std::vector<std::string>& appids) {
  bf::path root_path = ci::GetRootAppPath(false);
  bf::path package_path = root_path / pkgid;
  ASSERT_FALSE(bf::exists(package_path));

  bf::path manifest_path =
      bf::path(getUserManifestPath(getuid())) / (pkgid + ".xml");
  ASSERT_FALSE(bf::exists(manifest_path));

  for (auto& appid : appids) {
    bf::path icon_path = bf::path(getIconPath(getuid())) / (appid + ".png");
    ASSERT_FALSE(bf::exists(icon_path));
    bf::path icon_backup = ci::GetBackupPathForIconFile(icon_path);
    ASSERT_FALSE(bf::exists(icon_backup));
  }

  // backups should not exist
  bf::path package_backup = ci::GetBackupPathForPackagePath(package_path);
  bf::path manifest_backup = ci::GetBackupPathForManifestFile(manifest_path);
  ASSERT_FALSE(bf::exists(package_backup));
  ASSERT_FALSE(bf::exists(manifest_backup));
}

void ValidatePackage(const std::string& pkgid,
                     const std::vector<std::string>& appids) {
  ASSERT_TRUE(ci::IsPackageInstalled(pkgid, ci::GetRequestMode()));
  ValidatePackageFS(pkgid, appids);
}

void CheckPackageNonExistance(const std::string& pkgid,
                              const std::vector<std::string>& appids) {
  ASSERT_FALSE(ci::IsPackageInstalled(pkgid, ci::GetRequestMode()));
  PackageCheckCleanup(pkgid, appids);
}

std::unique_ptr<ci::AppQueryInterface> CreateQueryInterface() {
  std::unique_ptr<ci::AppQueryInterface> query_interface(
      new wgt::WgtAppQueryInterface());
  return query_interface;
}

std::unique_ptr<ci::AppInstaller> CreateInstaller(ci::PkgMgrPtr pkgmgr,
                                                  PackageType type) {
  switch (type) {
    case PackageType::WGT:
      return std::unique_ptr<ci::AppInstaller>(new wgt::WgtInstaller(pkgmgr));
    case PackageType::HYBRID:
      return std::unique_ptr<ci::AppInstaller>(
          new hybrid::HybridInstaller(pkgmgr));
    default:
      LOG(ERROR) << "Unknown installer type";
      return nullptr;
  }
}

ci::AppInstaller::Result RunInstallerWithPkgrmgr(ci::PkgMgrPtr pkgmgr,
                                                 PackageType type,
                                                 RequestResult mode) {
  std::unique_ptr<ci::AppInstaller> installer = CreateInstaller(pkgmgr, type);
  switch (mode) {
  case RequestResult::FAIL:
    installer->AddStep<ci::configuration::StepFail>();
    break;
  case RequestResult::CRASH:
    installer->AddStep<StepCrash>();
  default:
    break;
  }
  return installer->Run();
}

ci::AppInstaller::Result Install(const bf::path& path,
                                 PackageType type,
                                 RequestResult mode = RequestResult::NORMAL) {
  const char* argv[] = {"", "-i", path.c_str()};
  TestPkgmgrInstaller pkgmgr_installer;
  std::unique_ptr<ci::AppQueryInterface> query_interface =
      CreateQueryInterface();
  auto pkgmgr =
      ci::PkgMgrInterface::Create(SIZEOFARRAY(argv), const_cast<char**>(argv),
                                  &pkgmgr_installer, query_interface.get());
  if (!pkgmgr) {
    LOG(ERROR) << "Failed to initialize pkgmgr interface";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return RunInstallerWithPkgrmgr(pkgmgr, type, mode);
}

ci::AppInstaller::Result Update(const bf::path& path_old,
                                const bf::path& path_new,
                                PackageType type,
                                RequestResult mode = RequestResult::NORMAL) {
  if (Install(path_old, type) != ci::AppInstaller::Result::OK) {
    LOG(ERROR) << "Failed to install application. Cannot update";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return Install(path_new, type, mode);
}

ci::AppInstaller::Result Uninstall(const std::string& pkgid,
                                   PackageType type,
                                   RequestResult mode = RequestResult::NORMAL) {
  const char* argv[] = {"", "-d", pkgid.c_str()};
  TestPkgmgrInstaller pkgmgr_installer;
  std::unique_ptr<ci::AppQueryInterface> query_interface =
      CreateQueryInterface();
  auto pkgmgr =
      ci::PkgMgrInterface::Create(SIZEOFARRAY(argv), const_cast<char**>(argv),
                                  &pkgmgr_installer, query_interface.get());
  if (!pkgmgr) {
    LOG(ERROR) << "Failed to initialize pkgmgr interface";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return RunInstallerWithPkgrmgr(pkgmgr, type, mode);
}

ci::AppInstaller::Result Reinstall(const bf::path& path,
                                   const bf::path& delta_dir,
                                   PackageType type,
                                   RequestResult mode = RequestResult::NORMAL) {
  if (Install(path, type) != ci::AppInstaller::Result::OK) {
    LOG(ERROR) << "Failed to install application. Cannot perform RDS";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  const char* argv[] = {"", "-r", delta_dir.c_str()};
  TestPkgmgrInstaller pkgmgr_installer;
  std::unique_ptr<ci::AppQueryInterface> query_interface =
      CreateQueryInterface();
  auto pkgmgr =
      ci::PkgMgrInterface::Create(SIZEOFARRAY(argv), const_cast<char**>(argv),
                                  &pkgmgr_installer, query_interface.get());
  if (!pkgmgr) {
    LOG(ERROR) << "Failed to initialize pkgmgr interface";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return RunInstallerWithPkgrmgr(pkgmgr, type, mode);
}

ci::AppInstaller::Result DeltaInstall(const bf::path& path,
    const bf::path& delta_package, PackageType type) {
  if (Install(path, type) != ci::AppInstaller::Result::OK) {
    LOG(ERROR) << "Failed to install application. Cannot perform RDS";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return Install(delta_package, type);
}

ci::AppInstaller::Result Recover(const bf::path& recovery_file,
                                 PackageType type,
                                 RequestResult mode = RequestResult::NORMAL) {
  const char* argv[] = {"", "-b", recovery_file.c_str()};
  TestPkgmgrInstaller pkgmgr_installer;
  std::unique_ptr<ci::AppQueryInterface> query_interface =
      CreateQueryInterface();
  auto pkgmgr =
      ci::PkgMgrInterface::Create(SIZEOFARRAY(argv), const_cast<char**>(argv),
                                  &pkgmgr_installer, query_interface.get());
  if (!pkgmgr) {
    LOG(ERROR) << "Failed to initialize pkgmgr interface";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return RunInstallerWithPkgrmgr(pkgmgr, type, mode);
}

}  // namespace

namespace common_installer {

class SmokeEnvironment : public testing::Environment {
 public:
  explicit SmokeEnvironment(const bf::path& home) : home_(home) {
  }
  void SetUp() override {
    bs::error_code error;
    bf::remove_all(home_ / kApplicationDirBackup, error);
    bf::remove_all(home_ / KUserAppsDirBackup, error);
    if (bf::exists(home_ / KUserAppsDir)) {
      bf::rename(home_ / KUserAppsDir, home_ / KUserAppsDirBackup, error);
      if (error)
        LOG(ERROR) << "Failed to setup test environment. Does some previous"
                   << " test crashed? Directory: "
                   << (home_ / KUserAppsDirBackup) << " should not exist.";
      assert(!error);
    }
    if (bf::exists(home_ / kApplicationDir)) {
      bf::rename(home_ / kApplicationDir, home_ / kApplicationDirBackup, error);
      if (error)
        LOG(ERROR) << "Failed to setup test environment. Does some previous"
                   << " test crashed? Directory: "
                   << (home_ / kApplicationDirBackup) << " should not exist.";
      assert(!error);
    }
  }
  void TearDown() override {
    bs::error_code error;
    bf::remove_all(home_ / kApplicationDir, error);
    bf::remove_all(home_ / KUserAppsDir, error);
    if (bf::exists(home_ / KUserAppsDirBackup))
      bf::rename(home_ / KUserAppsDirBackup, home_ / KUserAppsDir, error);
    if (bf::exists(home_ / kApplicationDirBackup))
      bf::rename(home_ / kApplicationDirBackup, home_ / kApplicationDir, error);
  }

 private:
  bf::path home_;
};

class SmokeTest : public testing::Test {
};

TEST_F(SmokeTest, InstallationMode) {
  bf::path path = kSmokePackagesDirectory / "InstallationMode.wgt";
  std::string pkgid = "smokeapp03";
  std::string appid = "smokeapp03.InstallationMode";
  ASSERT_EQ(Install(path, PackageType::WGT), ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, {appid});
}

TEST_F(SmokeTest, UpdateMode) {
  bf::path path_old = kSmokePackagesDirectory / "UpdateMode.wgt";
  bf::path path_new = kSmokePackagesDirectory / "UpdateMode_2.wgt";
  std::string pkgid = "smokeapp04";
  std::string appid = "smokeapp04.UpdateMode";
  ASSERT_EQ(Update(path_old, path_new, PackageType::WGT),
            ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, {appid});

  ASSERT_TRUE(ValidateFileContentInPackage(pkgid, "res/wgt/VERSION", "2\n"));
}

TEST_F(SmokeTest, DeinstallationMode) {
  bf::path path = kSmokePackagesDirectory / "DeinstallationMode.wgt";
  std::string pkgid = "smokeapp05";
  std::string appid = "smokeapp05.DeinstallationMode";
  ASSERT_EQ(Install(path, PackageType::WGT),
            ci::AppInstaller::Result::OK);
  ASSERT_EQ(Uninstall(pkgid, PackageType::WGT), ci::AppInstaller::Result::OK);
  CheckPackageNonExistance(pkgid, {appid});
}

TEST_F(SmokeTest, RDSMode) {
  bf::path path = kSmokePackagesDirectory / "RDSMode.wgt";
  bf::path delta_directory = kSmokePackagesDirectory / "delta_dir/";
  std::string pkgid = "smokeapp11";
  std::string appid = "smokeapp11.RDSMode";
  ASSERT_EQ(Reinstall(path, delta_directory, PackageType::WGT),
            ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, {appid});

  // Check delta modifications
  bf::path root_path = ci::GetRootAppPath(false);
  ASSERT_FALSE(bf::exists(root_path / pkgid / "res" / "wgt" / "DELETED"));
  ASSERT_TRUE(bf::exists(root_path / pkgid / "res" / "wgt" / "ADDED"));
  ValidateFileContentInPackage(pkgid, "res/wgt/MODIFIED", "2\n");
}

TEST_F(SmokeTest, DeltaMode) {
  bf::path path = kSmokePackagesDirectory / "DeltaMode.wgt";
  bf::path delta_package = kSmokePackagesDirectory / "DeltaMode.delta";
  std::string pkgid = "smokeapp17";
  std::string appid = "smokeapp17.DeltaMode";
  ASSERT_EQ(DeltaInstall(path, delta_package, PackageType::WGT),
            ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, {appid});

  // Check delta modifications
  bf::path root_path = ci::GetRootAppPath(false);
  ASSERT_FALSE(bf::exists(root_path / pkgid / "res" / "wgt" / "DELETED"));
  ASSERT_TRUE(bf::exists(root_path / pkgid / "res" / "wgt" / "ADDED"));
  ASSERT_TRUE(bf::exists(root_path / pkgid / "res" / "wgt" / "css" / "style.css"));  // NOLINT
  ASSERT_TRUE(bf::exists(root_path / pkgid / "res" / "wgt" / "images" / "tizen_32.png"));  // NOLINT
  ASSERT_TRUE(bf::exists(root_path / pkgid / "res" / "wgt" / "js" / "main.js"));
  ValidateFileContentInPackage(pkgid, "res/wgt/MODIFIED", "version 2\n");
}

TEST_F(SmokeTest, RecoveryMode_ForInstallation) {
  bf::path path = kSmokePackagesDirectory / "RecoveryMode_ForInstallation.wgt";
  ASSERT_DEATH(Install(path, PackageType::WGT, RequestResult::CRASH), ".*");

  std::string pkgid = "smokeapp09";
  std::string appid = "smokeapp09.RecoveryModeForInstallation";
  bf::path recovery_file = FindRecoveryFile();
  ASSERT_FALSE(recovery_file.empty());
  ASSERT_EQ(Recover(recovery_file, PackageType::WGT),
      ci::AppInstaller::Result::OK);
  CheckPackageNonExistance(pkgid, {appid});
}

TEST_F(SmokeTest, RecoveryMode_ForUpdate) {
  bf::path path_old = kSmokePackagesDirectory / "RecoveryMode_ForUpdate.wgt";
  bf::path path_new = kSmokePackagesDirectory / "RecoveryMode_ForUpdate_2.wgt";
  RemoveAllRecoveryFiles();
  ASSERT_DEATH(Update(path_old, path_new, PackageType::WGT,
                      RequestResult::CRASH), ".*");

  std::string pkgid = "smokeapp10";
  std::string appid = "smokeapp10.RecoveryModeForUpdate";
  bf::path recovery_file = FindRecoveryFile();
  ASSERT_FALSE(recovery_file.empty());
  ASSERT_EQ(Recover(recovery_file, PackageType::WGT),
            ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, {appid});

  ASSERT_TRUE(ValidateFileContentInPackage(pkgid, "res/wgt/VERSION", "1\n"));
}

TEST_F(SmokeTest, InstallationMode_GoodSignature) {
  bf::path path = kSmokePackagesDirectory / "InstallationMode_GoodSignature.wgt";  // NOLINT
  ASSERT_EQ(Install(path, PackageType::WGT), ci::AppInstaller::Result::OK);
}

TEST_F(SmokeTest, InstallationMode_WrongSignature) {
  bf::path path = kSmokePackagesDirectory / "InstallationMode_WrongSignature.wgt";  // NOLINT
  ASSERT_EQ(Install(path, PackageType::WGT), ci::AppInstaller::Result::ERROR);
}

TEST_F(SmokeTest, InstallationMode_Rollback) {
  bf::path path = kSmokePackagesDirectory / "InstallationMode_Rollback.wgt";
  std::string pkgid = "smokeapp06";
  std::string appid = "smokeapp06.InstallationModeRollback";
  ASSERT_EQ(Install(path, PackageType::WGT, RequestResult::FAIL),
            ci::AppInstaller::Result::ERROR);
  CheckPackageNonExistance(pkgid, {appid});
}

TEST_F(SmokeTest, UpdateMode_Rollback) {
  bf::path path_old = kSmokePackagesDirectory / "UpdateMode_Rollback.wgt";
  bf::path path_new = kSmokePackagesDirectory / "UpdateMode_Rollback_2.wgt";
  std::string pkgid = "smokeapp07";
  std::string appid = "smokeapp07.UpdateModeRollback";
  ASSERT_EQ(Update(path_old, path_new, PackageType::WGT, RequestResult::FAIL),
                   ci::AppInstaller::Result::ERROR);
  ValidatePackage(pkgid, {appid});

  ASSERT_TRUE(ValidateFileContentInPackage(pkgid, "res/wgt/VERSION", "1\n"));
}

TEST_F(SmokeTest, InstallationMode_Hybrid) {
  bf::path path = kSmokePackagesDirectory / "InstallationMode_Hybrid.wgt";
  std::string pkgid = "smokehyb01";
  std::string appid1 = "smokehyb01.Web";
  std::string appid2 = "smokehyb01.Native";
  ASSERT_EQ(Install(path, PackageType::HYBRID), ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, {appid1, appid2});
}

TEST_F(SmokeTest, UpdateMode_Hybrid) {
  bf::path path_old = kSmokePackagesDirectory / "UpdateMode_Hybrid.wgt";
  bf::path path_new = kSmokePackagesDirectory / "UpdateMode_Hybrid_2.wgt";
  std::string pkgid = "smokehyb02";
  std::string appid1 = "smokehyb02.Web";
  std::string appid2 = "smokehyb02.Native";
  ASSERT_EQ(Update(path_old, path_new, PackageType::HYBRID),
            ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, {appid1, appid2});

  ASSERT_TRUE(ValidateFileContentInPackage(pkgid, "res/wgt/VERSION", "2\n"));
  ASSERT_TRUE(ValidateFileContentInPackage(pkgid, "VERSION", "2\n"));
}

TEST_F(SmokeTest, DeinstallationMode_Hybrid) {
  bf::path path = kSmokePackagesDirectory / "DeinstallationMode_Hybrid.wgt";
  std::string pkgid = "smokehyb03";
  std::string appid1 = "smokehyb03.Web";
  std::string appid2 = "smokehyb03.Native";
  ASSERT_EQ(Install(path, PackageType::HYBRID),
            ci::AppInstaller::Result::OK);
  ASSERT_EQ(Uninstall(pkgid, PackageType::WGT), ci::AppInstaller::Result::OK);
  CheckPackageNonExistance(pkgid, {appid1, appid2});
}

TEST_F(SmokeTest, DeltaMode_Hybrid) {
  bf::path path = kSmokePackagesDirectory / "DeltaMode_Hybrid.wgt";
  bf::path delta_package = kSmokePackagesDirectory / "DeltaMode_Hybrid.delta";
  std::string pkgid = "smokehyb04";
  std::string appid1 = "smokehyb04.Web";
  std::string appid2 = "smokehyb04.Native";
  ASSERT_EQ(DeltaInstall(path, delta_package, PackageType::HYBRID),
            ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, {appid1, appid2});

  // Check delta modifications
  bf::path root_path = ci::GetRootAppPath(false);
  ASSERT_FALSE(bf::exists(root_path / pkgid / "res" / "wgt" / "DELETED"));
  ASSERT_TRUE(bf::exists(root_path / pkgid / "res" / "wgt" / "ADDED"));
  ASSERT_FALSE(bf::exists(root_path / pkgid / "lib" / "DELETED"));
  ASSERT_TRUE(bf::exists(root_path / pkgid / "lib" / "ADDED"));
  ASSERT_TRUE(bf::exists(root_path / pkgid / "res" / "wgt" / "css" / "style.css"));  // NOLINT
  ASSERT_TRUE(bf::exists(root_path / pkgid / "res" / "wgt" / "images" / "tizen_32.png"));  // NOLINT
  ASSERT_TRUE(bf::exists(root_path / pkgid / "res" / "wgt" / "js" / "main.js"));
  ValidateFileContentInPackage(pkgid, "res/wgt/MODIFIED", "version 2\n");
  ValidateFileContentInPackage(pkgid, "lib/MODIFIED", "version 2\n");
}

}  // namespace common_installer

int main(int argc,  char** argv) {
  testing::InitGoogleTest(&argc, argv);
  const char* directory = getenv("HOME");
  if (!directory) {
    LOG(ERROR) << "Cannot get $HOME value";
    return 1;
  }
  testing::AddGlobalTestEnvironment(
      new common_installer::SmokeEnvironment(directory));
  return RUN_ALL_TESTS();
}
