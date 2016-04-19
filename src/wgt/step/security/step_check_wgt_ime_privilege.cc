// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <wgt/step/security/step_check_wgt_ime_privilege.h>

#include <manifest_parser/utils/version_number.h>
#include <manifest_parser/utils/logging.h>

#include <common/utils/glist_range.h>

namespace {
const char kImeCategoryName[] = "http://tizen.org/category/ime";
const char kImePrivilegeName[] = "http://tizen.org/privilege/ime";
}

namespace wgt {
namespace security {

common_installer::Step::Status StepCheckWgtImePrivilege::process() {
  utils::VersionNumber apiVersion{context_->manifest_data.get()->api_version};

  const auto version23 = apiVersion < utils::VersionNumber{"2.4"};

  for (const auto app :
      GListRange<application_x *>(context_->manifest_data.get()->application)) {
    for (const auto category : GListRange<char *>(app->category)) {
      if (strcmp(category, kImeCategoryName) == 0) {
        const auto result = (version23) ? (Check23Api()) : (Check24Api());
        if (result != Status::OK) {
          LOG(ERROR) << "Insufficient privileges for IME application.";
          return result;
        }

        break;
      }
    }
  }

  return Status::OK;
}

common_installer::Step::Status StepCheckWgtImePrivilege::Check23Api() const {
  const auto &ime = context_->manifest_plugins_data.get().ime_info.get();
  if (ime.getUuid().empty()) {
    LOG(ERROR) << "Missing IME tag.";
    return Status::CONFIG_ERROR;
  }

  return CheckImePrivilege();
}

common_installer::Step::Status StepCheckWgtImePrivilege::Check24Api() const {
  return CheckImePrivilege();
}

common_installer::Step::Status
StepCheckWgtImePrivilege::CheckImePrivilege() const {
  for (const auto privilege :
      GListRange<char *>(context_->manifest_data.get()->privileges)) {
    if (strcmp(privilege, kImePrivilegeName) == 0)
      return Status::OK;
  }

  LOG(ERROR) << "Missing IME privilege.";
  return Status::PRIVILEGE_ERROR;
}
}  // namespace security
}  // namespace wgt
