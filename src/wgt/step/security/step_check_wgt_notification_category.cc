// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <wgt/step/security/step_check_wgt_notification_category.h>

#include <common/utils/glist_range.h>

#include <manifest_parser/utils/logging.h>

namespace {
  const auto kNotificationCategoryName
      = "http://tizen.org/category/always_on_top";
}

namespace wgt {
namespace security {

common_installer::Step::Status StepCheckWgtNotificationCategory::process() {
  const auto notificationAllowed = context_->privilege_level.get() >=
    common_installer::PrivilegeLevel::PARTNER;

  // no point in checking if priv is set
  if (notificationAllowed)
    return Status::OK;

  for (const auto app :
      GListRange<application_x *>(context_->manifest_data.get()->application)) {
    for (const auto category : GListRange<char *>(app->category)) {
      if (strcmp(category, kNotificationCategoryName) == 0) {
          LOG(ERROR) << "Insufficient privileges for notification category.";
          return Status::PRIVILEGE_ERROR;
      }
    }
  }

  LOG(INFO) << "No notification category present.";
  return Status::OK;
}
}
}
