// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_CHECK_HYBRID_H_
#define WGT_STEP_STEP_CHECK_HYBRID_H_

#include <common/step/step.h>
#include <manifest_parser/utils/logging.h>

namespace wgt {
namespace hybrid {

/**
 * \brief Step sets information if web application is hybrid
 */
class StepCheckHybrid : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(CheckHybrid)
};

}  // namespace hybrid
}  // namespace wgt

#endif  // WGT_STEP_STEP_CHECK_HYBRID_H_
