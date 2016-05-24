// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/filesystem/step_copy_preview_icons.h"

#include <boost/filesystem/path.hpp>
#include <common/utils/file_util.h>

#include <string>

#include "wgt/wgt_backend_data.h"

namespace bf = boost::filesystem;
namespace ci = common_installer;

namespace {

const char kResWgt[] = "res/wgt";
const char kSharedRes[] = "shared/res";

}  // namespace

namespace wgt {
namespace filesystem {

ci::Step::Status StepCopyPreviewIcons::process() {
  WgtBackendData* backend_data =
      static_cast<WgtBackendData*>(context_->backend_data.get());
  for (auto& appwidget : backend_data->appwidgets.get().app_widgets()) {
    if (!appwidget.icon_src.empty()) {
      bf::path icon_path =
          context_->pkg_path.get() / kResWgt / appwidget.icon_src;
      for (auto& size : appwidget.content_size) {
        std::string type = wgt::parse::AppWidgetSizeTypeToString(size.type);
        std::string icon_name = appwidget.id + "." + type + "." + "preview" +
            bf::path(appwidget.icon_src).extension().string();
        bf::path preview_icon =
            context_->pkg_path.get() / kSharedRes / icon_name;
        if (!ci::CopyFile(icon_path, preview_icon)) {
          LOG(ERROR) << "Cannot create preview icon: " << preview_icon;
          return Status::ICON_ERROR;
        }
      }
    }
  }
  LOG(DEBUG) << "Preview icons created";
  return Status::OK;
}

}  // namespace filesystem
}  // namespace wgt
