// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/filesystem/step_wgt_prepare_package_directory.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/system/system_error.hpp>

#include <common/paths.h>
#include <common/utils/file_util.h>

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

const char kResWgtDirectory[] = "res/wgt";

}  // namespace

namespace wgt {
namespace filesystem {

ci::Step::Status StepWgtPreparePackageDirectory::CreateSymlinkToMountPoint() {
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
  } else {
    bf::create_directories(res_wgt_link.parent_path(), error);
    if (error) {
      LOG(ERROR) << "Failed to create " << kResWgtDirectory << " directory";
      return Status::APP_DIR_ERROR;
    }
  }
  bf::create_symlink(mount_point, res_wgt_link, error);
  if (error) {
    LOG(ERROR) << "Failed to create symlink to widget image";
    return Status::APP_DIR_ERROR;
  }
  return Status::OK;
}

ci::Step::Status StepWgtPreparePackageDirectory::process() {
  Status status = CreateSymlinkToMountPoint();
  if (status != Status::OK)
    return status;
  LOG(DEBUG) << "Symlink to mount point created";

  return Status::OK;
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
