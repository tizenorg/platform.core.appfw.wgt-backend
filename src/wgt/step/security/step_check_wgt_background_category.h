// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_SECURITY_STEP_CHECK_WGT_BACKGROUND_CATEGORY_H_
#define WGT_STEP_SECURITY_STEP_CHECK_WGT_BACKGROUND_CATEGORY_H_

#include <common/step/security/step_check_background_category.h>
#include <manifest_parser/utils/version_number.h>

namespace wgt {
namespace security {

/**
 * \brief This step check background category value and modify it depending on
 *        required version, cert level, background support, and value itself
 */
class StepCheckWgtBackgroundCategory :
    public common_installer::security::StepCheckBackgroundCategory {
 public:
  explicit StepCheckWgtBackgroundCategory(
      common_installer::InstallerContext* context);

 protected:
  bool GetBackgroundSupport() override;
};

}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_SECURITY_STEP_CHECK_WGT_BACKGROUND_CATEGORY_H_
