// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_RDS_PARSE_H_
#define WGT_STEP_STEP_RDS_PARSE_H_

#include <common/step/step.h>

#include <boost/filesystem.hpp>
#include <string>
#include <vector>

namespace wgt {
namespace rds {

/**
 * \brief This step parse .rds_delta file
 *
 * This is to prepare RDS installation process
 */
class StepRDSParse : public common_installer::Step {
 public:
  using Step::Step;

  /**
   * \brief Parse .rds_delta file
   *
   * \return Status::ERROR when wgt backend data are missing,
   *         Status::OK otherwise
   */
  Status process() override;

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status clean() override { return Status::OK; }

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status undo() override { return Status::OK; }

  /**
   * \brief Validate if .rds_delta file exist
   *
   * \return Status::Error if file not exist,
   *         Status::OK otherwise
   */
  Status precheck() override;

 private:
  boost::filesystem::path rds_file_path_;
};
}  // namespace rds
}  // namespace wgt
#endif  // WGT_STEP_STEP_RDS_PARSE_H_
