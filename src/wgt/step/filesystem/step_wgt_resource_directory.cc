// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/filesystem/step_wgt_resource_directory.h"

#include <boost/filesystem/path.hpp>

#include <common/utils/file_util.h>

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtResourceDirectory::process() {
  bf::path temp_path = context_->unpacked_dir_path.get();
  temp_path += ".temp";
  bf::path resource_path = context_->unpacked_dir_path.get() / "res/wgt";

  if (!common_installer::MoveDir(context_->unpacked_dir_path.get(),
                                        temp_path)) {
    LOG(ERROR) << "Failed to move: " << context_->unpacked_dir_path.get()
               << " to: " << temp_path;
    return Status::APP_DIR_ERROR;
  }
  bs::error_code error;
  bf::create_directories(resource_path.parent_path(), error);
  if (error) {
    LOG(ERROR) << "Failed to create proper directory structure in widget";
    return Status::APP_DIR_ERROR;
  }
  if (!common_installer::MoveDir(temp_path, resource_path)) {
    LOG(ERROR) << "Failed to move: " << temp_path << " to: " << resource_path;
    return Status::APP_DIR_ERROR;
  }

  LOG(INFO) << "Widget content moved to res/wgt subdirectory";
  return Status::OK;
}

}  // namespace filesystem
}  // namespace wgt
