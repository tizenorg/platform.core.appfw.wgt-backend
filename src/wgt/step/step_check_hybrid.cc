// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_check_hybrid.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "wgt/wgt_backend_data.h"

namespace bf = boost::filesystem;

namespace {

const char kConfigLocationForHybrid[] = "res/wgt/config.xml";
const char kManifestLocationForHybrid[] = "tizen-manifest.xml";

}  // namespace

namespace wgt {
namespace hybrid {

common_installer::Step::Status StepCheckHybrid::process() {
  if (bf::exists(context_->pkg_path.get())) {
    if (bf::exists(kManifestLocationForHybrid)) {
      auto* data = static_cast<WgtBackendData*>(context_->backend_data.get());
      LOG(INFO) << "Package is a hybrid application";
      data->is_hybrid = true;
    }
  } else {
    if (bf::exists(
        context_->unpacked_dir_path.get() / kConfigLocationForHybrid)) {
      auto* data = static_cast<WgtBackendData*>(context_->backend_data.get());
      LOG(INFO) << "Package is a hybrid application";
      data->is_hybrid = true;
    }
  }
  return Status::OK;
}

}  // namespace hybrid
}  // namespace wgt

