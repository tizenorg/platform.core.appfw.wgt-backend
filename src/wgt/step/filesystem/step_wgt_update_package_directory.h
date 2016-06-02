// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_FILESYSTEM_STEP_WGT_UPDATE_PACKAGE_DIRECTORY_H_
#define WGT_STEP_FILESYSTEM_STEP_WGT_UPDATE_PACKAGE_DIRECTORY_H_

#include <manifest_parser/utils/logging.h>

#include "wgt/step/filesystem/step_wgt_prepare_package_directory.h"

namespace wgt {
namespace filesystem {

class StepWgtUpdatePackageDirectory : public StepWgtPreparePackageDirectory {
 public:
  using StepWgtPreparePackageDirectory::StepWgtPreparePackageDirectory;

  Status process() override;
  Status undo() override;
  Status clean() override;

 private:
  Status CreateBackupOfDirectories();
  Status RecoverBackupOfDirectories();

  STEP_NAME(WgtUpdatePackageDirectory)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_FILESYSTEM_STEP_WGT_UPDATE_PACKAGE_DIRECTORY_H_
