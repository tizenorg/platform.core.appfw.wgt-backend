// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_BACKUP_ICONS_H_
#define WGT_STEP_STEP_WGT_BACKUP_ICONS_H_

#include <common/step/step_backup_icons.h>
#include <manifest_parser/utils/logging.h>

namespace wgt {
namespace backup {

/**
 *\brief Step responsible for creating backup for icons during update and
 *       uninstallation.
 *       Used by WGT backend
 */
class StepWgtBackupIcons : public common_installer::backup::StepBackupIcons {
 public:
  using StepBackupIcons::StepBackupIcons;

  /**
   * \brief main logic of backuping icons
   *
   * \return Status::OK, if successful backup, Status::ERROR otherwise
   */
  Status process() override;

  SCOPE_LOG_TAG(WgtBackupIcons)
};

}  // namespace backup
}  // namespace wgt

#endif  // WGT_STEP_STEP_WGT_BACKUP_ICONS_H_
