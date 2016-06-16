// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef SRC_WGT_STEP_SECURITY_STEP_CHECK_EXTENSION_PRIVILEGES_H_
#define SRC_WGT_STEP_SECURITY_STEP_CHECK_EXTENSION_PRIVILEGES_H_

#include <common/app_installer.h>
#include <common/installer_context.h>
#include <common/step/step.h>
#include <manifest_parser/utils/logging.h>
#include <manifest_parser/values.h>

#include <vector>
#include <string>
#include <set>

namespace wgt {
namespace security {

/**
 * \brief Step that add user extension privileges during installation
 */
class StepCheckExtensionPrivileges : public common_installer::Step {
 public:
  using Step::Step;

  /**
   * \brief Add extension privileges
   *
   * \return Status::OK
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
   * \brief Check requirements for this step
   *
   * \return Status::ERROR when rmanifest data are missing,
   *         Status::OK otherwise
   */
  Status precheck() override;

  STEP_NAME(CheckExtensionPrivileges)
 private:
  std::string GetExtensionPath();
  bool CheckPriviligeLevel(std::set<std::string> priv_set);
};

}  // namespace security
}  // namespace wgt

#endif  // SRC_WGT_STEP_SECURITY_STEP_CHECK_EXTENSION_PRIVILEGES_H_
