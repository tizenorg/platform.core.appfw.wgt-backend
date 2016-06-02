// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_SECURITY_STEP_CHECK_WGT_IME_PRIVILEGE_H_
#define WGT_STEP_SECURITY_STEP_CHECK_WGT_IME_PRIVILEGE_H_

#include <common/step/step.h>

namespace wgt {
namespace security {

/**
 * \brief Checks if the app has IME privileges.
 */
class StepCheckWgtImePrivilege :
  public common_installer::Step {
 public:
  using common_installer::Step::Step;
  ~StepCheckWgtImePrivilege() override = default;

  Status process() override;

  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

 private:
  Status Check23Api() const;
  Status Check24Api() const;

  Status CheckImePrivilege() const;

  STEP_NAME(CheckWgtImePrivilege)
};
}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_SECURITY_STEP_CHECK_WGT_IME_PRIVILEGE_H_
