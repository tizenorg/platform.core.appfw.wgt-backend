// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_create_icons.h"

#include <pkgmgr-info.h>

#include "common/utils/glist_range.h"

namespace bf = boost::filesystem;
namespace ci = common_installer;

namespace {
const char kResWgt[] = "res/wgt";
}  // namespace

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtCreateIcons::process() {
  // TODO (l.wysocki): As a temporary solution this will copy icons into two
  // destinations respectively {HOME}/.applications/icons,
  // and {APP_ROOT}/shared/res, when some project will stop using old
  // location ({HOME}/.applications/icons) then it can be removed from here.
  std::vector<bf::path> paths { getIconPath(context_->uid.get()),
                                StepCreateIcons::GetIconRoot() };

  // explicit step for wgt apps to add absolute path to icon in order to
  // store it in db
  ci::Step::Status result = CopyIcons(paths);
  for (application_x* app :
      GListRange<application_x*>(context_->manifest_data.get()->application)) {
    if (app->icon) {
      icon_x* icon = reinterpret_cast<icon_x*>(app->icon->data);
      bf::path icon_path = StepCreateIcons::GetIconRoot() / icon->text;
      if (icon->text)
        free((void *)icon->text);
      icon->text = strdup(icon_path.c_str());
    }
  }
  return result;
}

boost::filesystem::path StepWgtCreateIcons::GetIconRoot() const {
  return context_->pkg_path.get() / kResWgt;
}

std::string StepWgtCreateIcons::GetAppTypeForIcons() const {
  return "webapp";
}

}  // namespace filesystem
}  // namespace wgt

