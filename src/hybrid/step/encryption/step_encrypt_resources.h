// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef HYBRID_STEP_ENCRYPTION_STEP_ENCRYPT_RESOURCES_H_
#define HYBRID_STEP_ENCRYPTION_STEP_ENCRYPT_RESOURCES_H_

#include <manifest_parser/utils/logging.h>

#include "wgt/step/encryption/step_encrypt_resources.h"

namespace hybrid {
namespace encryption {

/**
 * \brief Step that encrypt application resources files if flagged to do so
 *
 * This is variant for hybrid package
 */
class StepEncryptResources : public wgt::encryption::StepEncryptResources {
 public:
  using wgt::encryption::StepEncryptResources::StepEncryptResources;

 private:
  void SetEncryptionRoot() override;

  STEP_NAME(EncryptResources)
};

}  // namespace encryption
}  // namespace hybrid

#endif  // HYBRID_STEP_ENCRYPTION_STEP_ENCRYPT_RESOURCES_H_
