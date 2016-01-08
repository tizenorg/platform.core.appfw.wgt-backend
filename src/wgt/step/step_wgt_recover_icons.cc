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
  std::vector<bf::path> paths;
  paths.push_back(bf::path(getIconPath(context_->uid.get())));
  paths.push_back(context_->root_application_path.get()
      / context_->pkgid.get() / kSharedRes);
  return paths;
}

}  // namespace filesystem
}  // namespace wgt

