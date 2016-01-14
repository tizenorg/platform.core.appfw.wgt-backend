// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_PATCH_STORAGE_DIRECTORIES_H_
#define WGT_STEP_STEP_WGT_PATCH_STORAGE_DIRECTORIES_H_

#include <common/step/step.h>
#include <manifest_parser/utils/logging.h>

namespace wgt {
namespace filesystem {

/**
 * \brief Installation (WGT).
 *        Responsible for copying widgets shared/ directory into package's
 *        shared/ directory for tizen widget with api version > 3.0
 */
class StepWgtPatchStorageDirectories :
    public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

 private:
  bool ShareDirFor3x();
  bool CreatePrivateTmpDir();

  SCOPE_LOG_TAG(PatchWgtStorageDirectories)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_STEP_WGT_PATCH_STORAGE_DIRECTORIES_H_
