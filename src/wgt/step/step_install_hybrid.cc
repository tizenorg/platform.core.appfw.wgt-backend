// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_install_hybrid.h"

#include <common/utils/subprocess.h>
#include "wgt/wgt_backend_data.h"

namespace ci = common_installer;

namespace {

const char kHybridBackendLocation[] = "/usr/bin/hybrid-backend";

}  // namespace

namespace wgt {
namespace hybrid {

common_installer::Step::Status StepInstallHybrid::process() {
  if (static_cast<WgtBackendData*>(context_->backend_data.get())->is_hybrid) {
    LOG(INFO) << "Performing installation for native part of hybrid package...";
    ci::Subprocess hybrid_backend(kHybridBackendLocation);
    if (!hybrid_backend.Run("-i", context_->pkgid.get())) {
      LOG(ERROR) << "Cannot execute: " << kHybridBackendLocation;
      return Status::ERROR;
    }
    int status = hybrid_backend.Wait();
    if (status != 0) {
      LOG(ERROR) << "Installation of native part failed, exit code: " << status;
      return Status::ERROR;
    }
    LOG(INFO) << "Native part of hybrid package installed";
  }
  return Status::OK;
}

}  // namespace hybrid
}  // namespace wgt
