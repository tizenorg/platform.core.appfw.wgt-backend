// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_recover_icons.h"

#include <pkgmgr-info.h>

namespace {
const char kSharedRes[] = "shared/res";

namespace bf = boost::filesystem;
}  // namespace

namespace wgt {
namespace filesystem {

std::vector<boost::filesystem::path> StepWgtRecoverIcons::GetIconsPaths() {
  // TODO (l.wysocki): As a temporary solution this will move icons into two
  // destinations respectively {HOME}/.applications/icons,
  // and {APP_ROOT}/shared/res, when some project will stop using old
  // location ({HOME}/.applications/icons) then it can be removed from here.
  std::vector<bf::path> paths { bf::path(getIconPath(context_->uid.get())),
                                context_->root_application_path.get()
                                / context_->pkgid.get() / kSharedRes };
  return paths;
}

}  // namespace filesystem
}  // namespace wgt

