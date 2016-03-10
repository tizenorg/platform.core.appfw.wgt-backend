// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/filesystem/step_wgt_patch_icons.h"

#include <pkgmgr-info.h>

#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {
const char kResWgt[] = "res/wgt";
}  // namespace

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtPatchIcons::process() {
  bf::path common_icon_location = context_->pkg_path.get() / "shared" / "res";
  bs::error_code error;
  bf::create_directories(common_icon_location, error);
  for (application_x* app :
      GListRange<application_x*>(context_->manifest_data.get()->application)) {
    if (strcmp(app->type, "webapp") != 0)
      continue;
    if (app->icon) {
      icon_x* icon = reinterpret_cast<icon_x*>(app->icon->data);
      bf::path icon_text(icon->text);
      bf::path icon_path = common_icon_location / app->appid;
      if (icon_text.has_extension())
        icon_path += icon_text.extension();
      else
        icon_path += ".png";

      bf::copy_file(icon->text, icon_path,
                    bf::copy_option::overwrite_if_exists, error);
      if (error) {
        LOG(ERROR) << "Failed to move icon from " << icon->text << " to "
                   << icon_path;
        return Status::ICON_ERROR;
      }
      if (icon->text)
        free(const_cast<char*>(icon->text));
      icon->text = strdup(icon_path.c_str());
    }
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace wgt

