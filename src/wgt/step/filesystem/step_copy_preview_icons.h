// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_FILESYSTEM_STEP_COPY_PREVIEW_ICONS_H_
#define WGT_STEP_FILESYSTEM_STEP_COPY_PREVIEW_ICONS_H_

#include <manifest_parser/utils/logging.h>

#include <common/app_installer.h>
#include <common/installer_context.h>
#include <common/step/step.h>

namespace wgt {
namespace filesystem {

/**
 * \brief Step that create copy of preview icons in shared/res/ directory
 */
class StepCopyPreviewIcons : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  STEP_NAME(CopyPreviewIcons)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_FILESYSTEM_STEP_COPY_PREVIEW_ICONS_H_
