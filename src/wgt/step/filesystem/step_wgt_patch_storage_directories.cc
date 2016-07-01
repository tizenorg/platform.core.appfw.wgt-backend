// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/filesystem/step_wgt_patch_storage_directories.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <common/utils/file_util.h>

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kSharedLocation[] = "shared";
const char kSharedResLocation[] = "shared/res";
const char kResWgtSubPath[] = "res/wgt";
const char kTemporaryData[] = "tmp";

}  // namespace

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtPatchStorageDirectories::process() {
  if (!CreatePrivateTmpDir())
    return Status::APP_DIR_ERROR;

  char version = context_->manifest_data.get()->api_version[0];
  if ((version-'0') >= 3) {
    LOG(DEBUG) << "Copying widget's shared/ directory";
    if (!ShareDirFor3x())
      return Status::APP_DIR_ERROR;
  }

  return Status::OK;
}

bool StepWgtPatchStorageDirectories::ShareDirFor3x() {
  bf::path shared_dir = context_->pkg_path.get() / kSharedLocation;
  if (!bf::exists(shared_dir)) {
    bs::error_code error;
    bf::create_directory(shared_dir);
    if (error) {
      LOG(ERROR) << "Failed to create directory: " << shared_dir;
      return false;
    }
  }
  bf::path src = context_->pkg_path.get() / kResWgtSubPath / kSharedResLocation;
  if (!bf::exists(src))
    return true;
  if (!bf::is_directory(src)) {
    LOG(WARNING) << "Widget's shared/res/ is not directory";
    return true;
  }
  bf::path dst = context_->pkg_path.get() / kSharedResLocation;
  if (!common_installer::MoveDir(src, dst,
                                 common_installer::FS_MERGE_DIRECTORIES)) {
    LOG(ERROR) << "Failed to move shared data from res/wgt to shared";
    return false;
  }

  bs::error_code error_code;
  bf::create_symlink(dst, src, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create symbolic link for shared dir"
               << boost::system::system_error(error_code).what();
    return false;
  }
  return true;
}

bool StepWgtPatchStorageDirectories::CreatePrivateTmpDir() {
  bs::error_code error_code;
  bf::path tmp_path = context_->pkg_path.get() / kTemporaryData;
  bf::create_directory(tmp_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create private temporary directory for package";
    return false;
  }
  return true;
}

}  // namespace filesystem
}  // namespace wgt
