// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_CREATE_ICONS_H_
#define WGT_STEP_STEP_WGT_CREATE_ICONS_H_

#include <manifest_parser/utils/logging.h>

#include <common/step/step_create_icons.h>

#include <string>

namespace wgt {
namespace filesystem {

/**
 * \brief This step return path to widget icon
 */
class StepWgtCreateIcons
    : public common_installer::filesystem::StepCreateIcons {
 public:
  using StepCreateIcons::StepCreateIcons;

  /**
   * \brief Return path to widget icon
   *
   * \return path to widget icon
   */
  boost::filesystem::path GetIconRoot() const override;

  /**
   * \brief Return type of application for which icons should be created
   *
   * \return application type
   */
  std::string GetAppTypeForIcons() const override;

  SCOPE_LOG_TAG(WgtCreateIcons)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_STEP_WGT_CREATE_ICONS_H_
