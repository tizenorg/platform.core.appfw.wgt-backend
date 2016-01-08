// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_RECOVER_ICONS_H_
#define WGT_STEP_STEP_WGT_RECOVER_ICONS_H_

#include <common/step/step_recover_icons.h>

namespace wgt {
namespace filesystem {

/**
 * @brief The StepRecoverIcons class
 *        Fixes state of platform icon files in recovery mode.
 *
 * For recovery of new installation, all icons files are removed.
 * For recovery of update installation, all icons of applications of package are
 * restored to its previous locations.
 */
class StepWgtRecoverIcons :
    public common_installer::filesystem::StepRecoverIcons {
 protected:
  virtual std::vector<boost::filesystem::path> GetIconsPaths();

  SCOPE_LOG_TAG(WgtRecoverIcons)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_STEP_WGT_RECOVER_ICONS_H_
