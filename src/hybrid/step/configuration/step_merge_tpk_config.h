// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef HYBRID_STEP_CONFIGURATION_STEP_MERGE_TPK_CONFIG_H_
#define HYBRID_STEP_CONFIGURATION_STEP_MERGE_TPK_CONFIG_H_

#include <common/step/step.h>
#include <manifest_parser/utils/logging.h>

namespace hybrid {
namespace configuration {

/**
 * \brief MergeTpkConfig
 *        Merges information from tpk manifest into information from widget
 *        configuration for hybrid package.
 */
class StepMergeTpkConfig : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  STEP_NAME(MergeTpkConfig)
};

}  // namespace configuration
}  // namespace hybrid

#endif  // HYBRID_STEP_CONFIGURATION_STEP_MERGE_TPK_CONFIG_H_
