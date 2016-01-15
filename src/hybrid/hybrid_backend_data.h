// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef HYBRID_HYBRID_BACKEND_DATA_H_
#define HYBRID_HYBRID_BACKEND_DATA_H_

#include <common/utils/property.h>
#include <pkgmgrinfo_basic.h>
#include <pkgmgr_parser.h>
#include <wgt_manifest_handlers/setting_handler.h>

#include <string>
#include <vector>

#include "wgt/wgt_backend_data.h"

namespace hybrid {

class HybridBackendData : public wgt::WgtBackendData {
 public:
  HybridBackendData() { }
  ~HybridBackendData() override {
    if (tpk_manifest_data.get())
      pkgmgr_parser_free_manifest_xml(tpk_manifest_data.get());
  }

  Property<manifest_x*> tpk_manifest_data;
};

}  // namespace hybrid

#endif  // HYBRID_HYBRID_BACKEND_DATA_H_

