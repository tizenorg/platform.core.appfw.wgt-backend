// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/wgt_app_query_interface.h"

#include <unistd.h>
#include <sys/types.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <common/pkgmgr_registration.h>
#include <common/recovery_file.h>
#include <common/request.h>
#include <common/utils/file_util.h>

#include <manifest_parser/manifest_parser.h>
#include <manifest_parser/utils/logging.h>
#include <wgt_manifest_handlers/application_manifest_constants.h>
#include <wgt_manifest_handlers/tizen_application_handler.h>
#include <wgt_manifest_handlers/widget_handler.h>

#include <memory>
#include <string>
#include <vector>


namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

const char kHybridConfigLocation[] = "res/wgt/config.xml";
const char kTizenManifestLocation[] = "tizen-manifest.xml";

std::string GetInstallationRequestInfo(int argc, char** argv) {
  std::string path;
  for (int i = 0; i < argc; ++i) {
    if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "-r") ||
        !strcmp(argv[i], "-d") || !strcmp(argv[i], "-b")) {
      if (i + 1 < argc) {
        path = argv[i + 1];
        break;
      }
    }
  }
  return path;
}

std::string GetPkgIdFromPath(const std::string& path) {
  if (!bf::exists(path))
    return {};
  bf::path tmp_path = common_installer::GenerateTmpDir("/tmp");
  bs::error_code code;
  bf::create_directories(tmp_path, code);
  if (code)
    return {};
  if (!common_installer::ExtractToTmpDir(path.c_str(), tmp_path,
      "config.xml")) {
    bf::remove_all(tmp_path, code);
    return {};
  }
  bf::path config_path = tmp_path / "config.xml";
  std::vector<parser::ManifestHandler*> handlers = {
    new wgt::parse::WidgetHandler(),
    new wgt::parse::TizenApplicationHandler()
  };
  std::unique_ptr<parser::ManifestHandlerRegistry> registry(
      new parser::ManifestHandlerRegistry(handlers));
  std::unique_ptr<parser::ManifestParser> parser(
      new parser::ManifestParser(std::move(registry)));
  if (!parser->ParseManifest(config_path)) {
    bf::remove_all(tmp_path, code);
    return {};
  }
  auto info = std::static_pointer_cast<const wgt::parse::TizenApplicationInfo>(
      parser->GetManifestData(
          wgt::application_widget_keys::kTizenApplicationKey));
  if (!info) {
    bf::remove_all(tmp_path, code);
    return {};
  }
  std::string pkg_id = info->package();

  bf::remove_all(tmp_path, code);
  return pkg_id;
}

std::string ReadPkgidFromRecovery(const std::string& recovery_path) {
  std::unique_ptr<ci::recovery::RecoveryFile> recovery_file =
      ci::recovery::RecoveryFile::OpenRecoveryFileForPath(recovery_path);
  recovery_file->Detach();
  return recovery_file->pkgid();
}

}  // namespace

namespace wgt {

bool WgtAppQueryInterface::IsAppInstalledByArgv(int argc, char** argv) {
  std::string path = GetInstallationRequestInfo(argc, argv);
  if (path.empty())
    return false;
  std::string pkg_id = GetPkgIdFromPath(path);
  if (pkg_id.empty())
    return false;
  return ci::IsPackageInstalled(pkg_id, ci::GetRequestMode());
}

bool WgtAppQueryInterface::IsHybridApplication(int argc, char** argv) {
  std::string arg = GetInstallationRequestInfo(argc, argv);
  if (arg.find("apps_rw/recovery-") != std::string::npos)
    arg = ReadPkgidFromRecovery(arg);
  if (ci::IsPackageInstalled(arg, ci::GetRequestMode())) {
    bf::path package_directory(ci::GetRootAppPath(false));
    if (bf::exists(package_directory / arg / kTizenManifestLocation) &&
        bf::exists(package_directory / arg / kHybridConfigLocation))
      return true;
  } else {
    bool tizen_manifest_found = false;
    bool config_xml_found = false;
    if (!ci::CheckPathInZipArchive(arg.c_str(), kTizenManifestLocation,
                                   &tizen_manifest_found))
      return false;
    if (!ci::CheckPathInZipArchive(arg.c_str(), kHybridConfigLocation,
                                   &config_xml_found))
      return false;
    if (tizen_manifest_found && config_xml_found)
      return true;
  }
  return false;
}

}  // namespace wgt
