// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "hybrid/step/step_stash_tpk_config.h"

#include "hybrid/hybrid_backend_data.h"

namespace hybrid {
namespace parse {

common_installer::Step::Status StepStashTpkConfig::process() {
  HybridBackendData* data =
      static_cast<HybridBackendData*>(context_->backend_data.get());
  data->tpk_manifest_data.set(context_->manifest_data.get());
  context_->manifest_data.set(nullptr);

  return Status::OK;
}

}  // namespace parse
}  // namespace hybrid
