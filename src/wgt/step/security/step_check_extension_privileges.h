// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_SECURITY_STEP_CHECK_EXTENSION_PRIVILEGES_H_
#define WGT_STEP_SECURITY_STEP_CHECK_EXTENSION_PRIVILEGES_H_

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
   * \Adds extra package privileges defined by extension manifest.
   *
   * \return Status::OK
   */
  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  /**
   * \brief Check requirements for this step
   *
   * \return Status::ERROR when rmanifest data are missing,
   *         Status::OK otherwise
   */
  Status precheck() override;
 private:
  std::string GetExtensionPath();
  bool CheckPrivilegeLevel(std::set<std::string> priv_set);
  STEP_NAME(CheckExtensionPrivileges)
};

}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_SECURITY_STEP_CHECK_EXTENSION_PRIVILEGES_H_
