// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/filesystem/step_wgt_prepare_package_directory.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/system/system_error.hpp>

#include <common/backup_paths.h>
#include <common/utils/file_util.h>

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

const char kBinDirectory[] = "bin";
const char kResWgtDirectory[] = "res/wgt";

bool MoveCreateDir(const bf::path& source, const bf::path& destination) {
  if (!bf::exists(destination.parent_path())) {
    bs::error_code error;
    bf::create_directories(destination.parent_path(), error);
    if (error) {
      LOG(ERROR) << "Cannot create directory: " << destination.parent_path();
      return false;
    }
  }
  if (!ci::MoveDir(source, destination)) {
    LOG(ERROR) << "Failed to move directory " << destination;
    return false;
  }
  return true;
}

}  // namespace

namespace wgt {
namespace filesystem {

ci::Step::Status
StepWgtPreparePackageDirectory::CreateBackupOfBinaryDirectory() {
  bf::path bin_directory = context_->pkg_path.get() / kBinDirectory;
  bf::path backup_path =
      ci::GetBackupPathForPackagePath(context_->pkg_path.get());
  bf::path bin_directory_backup = backup_path / kBinDirectory;
  if (bf::exists(bin_directory)) {
    if (!MoveCreateDir(bin_directory, bin_directory_backup)) {
      LOG(ERROR) << "Failed to create ";
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

ci::Step::Status
StepWgtPreparePackageDirectory::RecoverBackupOfBinaryDirectory() {
  bf::path bin_directory = context_->pkg_path.get() / kBinDirectory;
  bf::path backup_path =
      ci::GetBackupPathForPackagePath(context_->pkg_path.get());
  bf::path bin_directory_backup = backup_path / kBinDirectory;
  if (bf::exists(bin_directory_backup)) {
    if (!ci::MoveDir(bin_directory_backup, bin_directory)) {
      LOG(ERROR) << "Failed to restore bin/ directory backup";
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

ci::Step::Status StepWgtPreparePackageDirectory::CreateSymglinkToMountPoint() {
  bs::error_code error;
  bf::path mount_point = ci::GetMountLocation(context_->pkg_path.get());
  bf::path res_wgt_link = context_->pkg_path.get() / kResWgtDirectory;
  if (bf::exists(res_wgt_link)) {
    if (!bf::is_symlink(res_wgt_link)) {
      LOG(ERROR) << res_wgt_link << " is not symlink. Cannot proceed";
      return Status::APP_DIR_ERROR;
    }
    bf::remove(res_wgt_link, error);
    if (error) {
      LOG(ERROR) << "Failed to remote old symlink to wgt resource directory";
      return Status::APP_DIR_ERROR;
    }
    bf::create_symlink(mount_point, res_wgt_link, error);
    if (error) {
      LOG(ERROR) << "Failed to create symlink to widget image";
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

ci::Step::Status StepWgtPreparePackageDirectory::process() {
  auto status = CreateBackupOfBinaryDirectory();
  if (status != Status::OK)
    return status;

  status = CreateSymglinkToMountPoint();
  if (status != Status::OK)
    return status;

  return Status::OK;
}

ci::Step::Status StepWgtPreparePackageDirectory::clean() {
  bf::path backup_path =
      ci::GetBackupPathForPackagePath(context_->pkg_path.get());
  if (bf::exists(backup_path)) {
    bs::error_code error;
    bf::remove(backup_path, error);
    if (error) {
      LOG(ERROR) << "Failed to remove backup directories";
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

ci::Step::Status StepWgtPreparePackageDirectory::undo() {
  return RecoverBackupOfBinaryDirectory();
}

ci::Step::Status StepWgtPreparePackageDirectory::precheck() {
  if (context_->pkg_path.get().empty()) {
    LOG(ERROR) << "Package installation path is not set";
    return Status::INVALID_VALUE;
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace wgt
