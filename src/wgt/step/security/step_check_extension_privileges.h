// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_SECURITY_STEP_CHECK_EXTENSION_PRIVILEGES_H_
#define WGT_STEP_SECURITY_STEP_CHECK_EXTENSION_PRIVILEGES_H_

#include <vector>

#include <common/app_installer.h>
#include <common/installer_context.h>
#include <common/step/step.h>
#include <manifest_parser/utils/logging.h>
#include <manifest_parser/values.h>

namespace wgt {
namespace security {

/**
 * \brief Step that add default privileges during installation
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

//  SCOPE_LOG_TAG(CheckExtensionPrivileges)

private:
  std::string GetExtensionPath();
//  std::vector<std::string> ParseExtensionConfig(std::string configXml);
  bool CheckPriviligeLevel(std::set<std::string> priv_set);
};

}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_SECURITY_STEP_CHECK_EXTENSION_PRIVILEGES_H_
