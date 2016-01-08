// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_create_icons.h"

#include <pkgmgr-info.h>

namespace {
const char kResWgt[] = "res/wgt";

namespace bf = boost::filesystem;
}  // namespace

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtCreateIcons::process() {
  std::vector<bf::path> paths;
  // TODO (l.wysocki): As a temporary solution this will copy icon into two
  // destinations respectively {HOME}/.applications/icons,
  // and {APP_ROOT}/shared/res, when some project will stop using old
  // location ({HOME}/.applications/icons) then it can be removed from here.
  paths.push_back(getIconPath(context_->uid.get()));
  paths.push_back(StepCreateIcons::GetIconRoot());
  return CopyIcons(paths);
}

boost::filesystem::path StepWgtCreateIcons::GetIconRoot() const {
  return context_->pkg_path.get() / kResWgt;
}

}  // namespace filesystem
}  // namespace wgt

