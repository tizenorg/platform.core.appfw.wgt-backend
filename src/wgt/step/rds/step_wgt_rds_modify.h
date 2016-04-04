// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_RDS_STEP_WGT_RDS_MODIFY_H_
#define WGT_STEP_RDS_STEP_WGT_RDS_MODIFY_H_

#include <boost/filesystem.hpp>
#include <common/step/step.h>
#include <common/step/rds/step_rds_modify.h>

#include <string>
#include <utility>
#include <vector>

#include "common/installer_context.h"

namespace wgt {
namespace rds {

/**
 * \brief Step that apply RDS modification during reinstallation process
 */
class StepWgtRDSModify : public common_installer::rds::StepRDSModify {
 public:
  using StepRDSModify::StepRDSModify;

  /**
   * \brief return app path
   *
   * \return std::string
   */
  std::string GetAppPath() override;
};

}  // rds
}  // wgt

#endif  // WGT_STEP_RDS_STEP_WGT_RDS_MODIFY_H_
