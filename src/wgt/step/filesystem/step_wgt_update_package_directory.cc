// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/filesystem/step_wgt_update_package_directory.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <common/paths.h>
#include <common/utils/file_util.h>

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

const char* kBackupEntries[] = {
  "bin",
  "shared/res"
};

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
StepWgtUpdatePackageDirectory::CreateBackupOfDirectories() {
  bf::path backup_path =
      ci::GetBackupPathForPackagePath(context_->pkg_path.get());
  for (auto& entry : kBackupEntries) {
    bf::path directory = context_->pkg_path.get() / entry;
    if (!bf::exists(directory))
      continue;
    LOG(DEBUG) << "Backup directory entry: " << entry;
    bf::path directory_backup = backup_path / entry;
    if (!MoveCreateDir(directory, directory_backup)) {
      LOG(ERROR) << "Failed to create backup directory "
                 << directory_backup;
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

ci::Step::Status
StepWgtUpdatePackageDirectory::RecoverBackupOfDirectories() {
  bf::path backup_path =
      ci::GetBackupPathForPackagePath(context_->pkg_path.get());

  // skip if there is no backup of directories
  if (!bf::exists(backup_path))
    return Status::OK;

  for (auto& entry : kBackupEntries) {
    bf::path directory = context_->pkg_path.get() / entry;
    bf::path directory_backup = backup_path / entry;
    if (!bf::exists(directory_backup))
        continue;
    LOG(DEBUG) << "Recover directory entry: " << entry;
    if (!ci::MoveDir(directory_backup, directory)) {
      LOG(ERROR) << "Failed to restore directory backup "
                 << directory_backup;
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

ci::Step::Status StepWgtUpdatePackageDirectory::process() {
  auto status = CreateBackupOfDirectories();
  if (status != Status::OK)
    return status;
  LOG(DEBUG) << "Directory backups created";

  status = StepWgtPreparePackageDirectory::process();
  if (status != Status::OK)
    return status;

  return Status::OK;
}

ci::Step::Status StepWgtUpdatePackageDirectory::clean() {
  bf::path backup_path =
      ci::GetBackupPathForPackagePath(context_->pkg_path.get());
  if (bf::exists(backup_path)) {
    bs::error_code error;
    bf::remove_all(backup_path, error);
    if (error) {
      LOG(ERROR) << "Failed to remove backup directories";
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

ci::Step::Status StepWgtUpdatePackageDirectory::undo() {
  Status status = RecoverBackupOfDirectories();
  if (status != Status::OK)
    return status;
  bf::path backup_path =
      ci::GetBackupPathForPackagePath(context_->pkg_path.get());
  if (bf::exists(backup_path)) {
    bs::error_code error;
    bf::remove_all(backup_path, error);
    if (error) {
      LOG(ERROR) << "Failed to remove backup directories";
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace wgt
