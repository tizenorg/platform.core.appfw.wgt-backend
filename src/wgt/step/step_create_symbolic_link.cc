/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_create_symbolic_link.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <common/utils/file_util.h>
#include <common/utils/glist_range.h>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>


namespace {

const char kWrtServiceBinaryPath[] = "/usr/bin/wrt-service";

}  // namespace

namespace wgt {
namespace filesystem {

namespace bf = boost::filesystem;

common_installer::Step::Status StepCreateSymbolicLink::process() {
  assert(context_->manifest_data.get());
  boost::system::error_code error;
  for (application_x* app :
       GListRange<application_x*>(context_->manifest_data.get()->application)) {
    // filter out non-wgt apps as this step is run for hybrid backend too
    if (strcmp("webapp", app->type) != 0)
      continue;
    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
    bf::path exec_path = context_->pkg_path.get() / bf::path("bin");
    common_installer::CreateDir(exec_path);

    exec_path /= bf::path(app->appid);

    if (strcmp(app->component_type, "uiapp") == 0) {
      bf::create_symlink(bf::path(WRT_LAUNCHER), exec_path, error);
    } else if (strcmp(app->component_type, "watchapp") == 0) {
      bf::create_symlink(bf::path(WRT_LAUNCHER), exec_path, error);
    } else {
      bf::create_symlink(kWrtServiceBinaryPath, exec_path, error);
    }
    if (error) {
      LOG(ERROR) << "Failed to set symbolic link "
        << boost::system::system_error(error).what();
      return Step::Status::ERROR;
    }
  }
  LOG(DEBUG) << "Symlinks created successfully";

  return Status::OK;
}

common_installer::Step::Status StepCreateSymbolicLink::undo() {
  for (application_x* app :
       GListRange<application_x*>(context_->manifest_data.get()->application)) {
    bf::path exec_path = context_->pkg_path.get() / "bin" / app->appid;
    if (bf::exists(exec_path))
      bf::remove_all(exec_path);
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace wgt
