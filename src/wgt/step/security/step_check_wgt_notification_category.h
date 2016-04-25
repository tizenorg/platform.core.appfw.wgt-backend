// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_SECURITY_STEP_CHECK_WGT_NOTIFICATION_CATEGORY_H_
#define WGT_STEP_SECURITY_STEP_CHECK_WGT_NOTIFICATION_CATEGORY_H_

#include <common/step/step.h>

namespace wgt {
namespace security {

/**
 * \brief Checks if the app can show notifications, if it requires that feature.
 */
class StepCheckWgtNotificationCategory :
  public common_installer::Step {
 public:
  using common_installer::Step::Step;
  ~StepCheckWgtNotificationCategory() = default;

  Status process() override;

  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }
};
}
}

#endif
