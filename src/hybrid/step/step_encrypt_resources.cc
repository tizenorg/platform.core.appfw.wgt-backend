// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "hybrid/step/step_encrypt_resources.h"

namespace hybrid {
namespace encrypt {

void StepEncryptResources::SetEncryptionRoot() {
  input_ = context_->unpacked_dir_path.get() / "res/wgt";
}

}  // namespace encrypt
}  // namespace hybrid

