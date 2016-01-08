// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_FAIL_HYBRID_H_
#define WGT_STEP_STEP_FAIL_HYBRID_H_

#include <common/installer_context.h>
#include <common/step/step_fail.h>
#include <manifest_parser/utils/logging.h>

namespace wgt {
namespace configuration {

/**
 * \brief step used to fail installation in modes that don't support hybrid apps
 */
class StepFailHybrid : public common_installer::configuration::StepFail {
 public:
  using StepFail::StepFail;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(FailHybrid)
};

}  // namespace configuration
}  // namespace wgt

#endif  // WGT_STEP_STEP_FAIL_HYBRID_H_
