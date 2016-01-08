// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_backup_icons.h"

#include <pkgmgr-info.h>

namespace {
const char kSharedRes[] = "shared/res";

namespace bf = boost::filesystem;
}  // namespace

namespace wgt {
namespace backup {

common_installer::Step::Status StepWgtBackupIcons::process() {
  std::vector<bf::path> paths;
  paths.push_back(getIconPath(context_->uid.get()));
  paths.push_back(context_->root_application_path.get()
      / context_->pkgid.get() / kSharedRes);

  return MoveIcons(paths);
}

}  // namespace backup
}  // namespace wgt
