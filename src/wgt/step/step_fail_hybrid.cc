// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_fail_hybrid.h"

#include "wgt/wgt_backend_data.h"

namespace wgt {
namespace configuration {

common_installer::Step::Status StepFailHybrid::process() {
  if (static_cast<WgtBackendData*>(context_->backend_data.get())->is_hybrid) {
    return StepFail::process();
  } else {
    return Status::OK;
  }
}

}  // namespace configuration
}  // namespace wgt
