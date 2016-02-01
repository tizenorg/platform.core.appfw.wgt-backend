// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_REMOVE_MANIFEST_H_
#define WGT_STEP_STEP_WGT_REMOVE_MANIFEST_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace wgt {
namespace pkgmgr {

/**
 * \brief Step responsbile for removing manifest file during uninstallation
 */
class StepRemoveManifest : public common_installer::Step {
 public:
  using common_installer::Step::Step;

  /**
   * \brief main logic of remove manifest
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status process() override;

  /**
   * \brief empty method
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status clean() override { return Status::OK; }

  /**
   * \brief empty method
   *
   * \return Status:OK if success, Status::ERROR othewise
   */
  Status undo() override { return Status::OK; }

  /**
   * \brief empty method
   *
   * \return Status::OK
   */
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(RemoveManifest)
};

}  // namespace pkgmgr
}  // namespace wgt

#endif  // WGT_STEP_STEP_WGT_REMOVE_MANIFEST_H_
