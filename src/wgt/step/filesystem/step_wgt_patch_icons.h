// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_FILESYSTEM_STEP_WGT_PATCH_ICONS_H_
#define WGT_STEP_FILESYSTEM_STEP_WGT_PATCH_ICONS_H_

#include <manifest_parser/utils/logging.h>

#include <common/step/step.h>

#include <string>

namespace wgt {
namespace filesystem {

/**
 * @brief The StepWgtPatchIcons class
 *        Copy widget icons to standard location of icons - shared/res/
 *        where StepCreateIcons excepts icons to be.
 */
class StepWgtPatchIcons : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status undo() override { return Status::OK; }
  Status clean() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  STEP_NAME(WgtPatchIcons)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_FILESYSTEM_STEP_WGT_PATCH_ICONS_H_
