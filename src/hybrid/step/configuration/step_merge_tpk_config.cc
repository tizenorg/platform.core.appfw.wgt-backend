// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "hybrid/step/configuration/step_merge_tpk_config.h"

#include <pkgmgrinfo_basic.h>

#include "hybrid/hybrid_backend_data.h"

namespace hybrid {
namespace configuration {

common_installer::Step::Status StepMergeTpkConfig::process() {
  HybridBackendData* data =
      static_cast<HybridBackendData*>(context_->backend_data.get());
  manifest_x* tpk_data = data->tpk_manifest_data.get();
  manifest_x* wgt_data = context_->manifest_data.get();

  wgt_data->application =
      g_list_concat(wgt_data->application, tpk_data->application);
  tpk_data->application = nullptr;

  wgt_data->privileges =
      g_list_concat(wgt_data->privileges, tpk_data->privileges);
  tpk_data->privileges = nullptr;

  return Status::OK;
}

}  // namespace configuration
}  // namespace hybrid
