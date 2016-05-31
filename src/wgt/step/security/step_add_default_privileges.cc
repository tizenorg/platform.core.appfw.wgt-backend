// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/security/step_add_default_privileges.h"
#include <common/privileges.h>

#include <pkgmgrinfo_basic.h>

#include <cstdlib>
#include <cstring>
#include <memory>

namespace wgt {
namespace security {

common_installer::Step::Status StepAddDefaultPrivileges::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "Manifest data is not set";
    return Status::ERROR;
  }
  return Status::OK;
}

common_installer::Step::Status StepAddDefaultPrivileges::process() {
  manifest_x* m = context_->manifest_data.get();
  m->privileges = g_list_append(m->privileges,
          strdup(common::privileges::kPrivForWebApp));
  return Status::OK;
}

}  // namespace security
}  // namespace wgt
