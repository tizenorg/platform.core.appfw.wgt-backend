// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_SECURITY_STEP_DIRECT_MANIFEST_CHECK_SIGNATURE_H_
#define WGT_STEP_SECURITY_STEP_DIRECT_MANIFEST_CHECK_SIGNATURE_H_

#include <common/step/security/step_check_signature.h>
#include <manifest_parser/utils/logging.h>

namespace wgt {
namespace security {

class StepDirectManifestCheckSignature
    : public common_installer::security::StepCheckSignature {
 public:
  using StepCheckSignature::StepCheckSignature;

 private:
  boost::filesystem::path GetSignatureRoot() const override;

  SCOPE_LOG_TAG(StepDirectManifestCheckSignature)
};

}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_SECURITY_STEP_DIRECT_MANIFEST_CHECK_SIGNATURE_H_
