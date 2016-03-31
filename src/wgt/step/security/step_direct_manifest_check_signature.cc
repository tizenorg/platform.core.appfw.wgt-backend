// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/security/step_direct_manifest_check_signature.h"

namespace bf = boost::filesystem;

namespace {

const char kResWgt[] = "res/wgt";

}  // namespace

namespace wgt {
namespace security {

bf::path StepDirectManifestCheckSignature::GetSignatureRoot() const {
  return StepCheckSignature::GetSignatureRoot() / kResWgt;
}

}  // namespace security
}  // namespace wgt
