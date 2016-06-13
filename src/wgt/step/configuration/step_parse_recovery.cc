// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/configuration/step_parse_recovery.h"

#include <common/paths.h>
#include <common/installer_context.h>

namespace {

const char kResWgtPath[] = "res/wgt";

}

namespace wgt {
namespace configuration {

StepParseRecovery::StepParseRecovery(
    common_installer::InstallerContext* context)
    : StepParse(context, StepParse::ConfigLocation::RECOVERY, false) {
}

common_installer::Step::Status StepParseRecovery::process() {
  (void) StepParse::process();
  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());
  return Status::OK;
}

common_installer::Step::Status StepParseRecovery::precheck() {
  if (context_->root_application_path.get().empty()) {
    LOG(ERROR) << "Root path of packages in not set";
    return Status::INVALID_VALUE;
  }
  if (context_->pkgid.get().empty()) {
    LOG(WARNING) << "Pkgid is not set. Parsing skipped";
    return Status::OK;
  }
  return Status::OK;
}

}  // namespace configuration
}  // namespace wgt
