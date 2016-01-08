// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "hybrid/step/step_parse.h"

namespace {

const char kResWgtPath[] = "res/wgt";

}

namespace hybrid {
namespace parse {

bool StepParse::LocateConfigFile() {
  return Check(context_->unpacked_dir_path.get() / kResWgtPath);
}

}  // namespace parse
}  // namespace hybrid

