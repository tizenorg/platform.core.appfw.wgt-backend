// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef HYBRID_STEP_CONFIGURATION_STEP_PARSE_H_
#define HYBRID_STEP_CONFIGURATION_STEP_PARSE_H_

#include <manifest_parser/utils/logging.h>

#include "wgt/step/configuration/step_parse.h"

namespace hybrid {
namespace configuration {

/**
 * \brief This step parse config.xml configuration file of widget
 *
 * This subclass overrides location of file and it is used when hybrid package
 * is installed.
 */
class StepParse : public wgt::configuration::StepParse {
 public:
  using wgt::configuration::StepParse::StepParse;

  bool LocateConfigFile() override;

  SCOPE_LOG_TAG(Parse)
};

}  // namespace configuration
}  // namespace hybrid

#endif  // HYBRID_STEP_CONFIGURATION_STEP_PARSE_H_
