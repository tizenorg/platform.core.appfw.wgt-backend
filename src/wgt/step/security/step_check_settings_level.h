// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_SECURITY_STEP_CHECK_SETTINGS_LEVEL_H_
#define WGT_STEP_SECURITY_STEP_CHECK_SETTINGS_LEVEL_H_

#include <manifest_parser/utils/logging.h>

#include <common/app_installer.h>
#include <common/installer_context.h>
#include <common/step/step.h>

namespace wgt {
namespace security {

/**
 * \brief Step that check privileges level for settings
 */
class StepCheckSettingsLevel : public common_installer::Step {
 public:
  using Step::Step;

  /**
   * \brief Check privileges level for settings
   *
   * \return Status::ERROR when invalid privileges detected,
   *         Status::OK otherwise
   */
  Status process() override;

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status clean() override { return Status::OK; }

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status undo() override { return Status::OK; }

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(CheckSettingsLevel)
};

}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_SECURITY_STEP_CHECK_SETTINGS_LEVEL_H_
