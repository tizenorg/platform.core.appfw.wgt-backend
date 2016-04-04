// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/rds/step_wgt_rds_modify.h"

#include <boost/filesystem.hpp>
#include <pkgmgrinfo_basic.h>

#include <cstdlib>
#include <cstring>
#include <memory>

namespace wgt {
namespace rds {

std::string StepWgtRDSModify::GetAppPath() {
  boost::filesystem::path p = context_->pkg_path.get() / "res" / "wgt";
  return p.string();
}

}  // namespace rds
}  // namespace wgt
