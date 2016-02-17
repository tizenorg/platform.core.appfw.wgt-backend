// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <common/pkgmgr_interface.h>
#include <cerrno>

#include "wgt/wgt_app_query_interface.h"
#include "wgt/wgt_installer.h"

namespace ci = common_installer;

namespace {

class StepCrash : public ci::Step {
 public:
  using Step::Step;

  ci::Step::Status process() override {
    raise(SIGSEGV);
    return Status::OK;
  }
  ci::Step::Status clean() override { return ci::Step::Status::OK; }
  ci::Step::Status undo() override { return ci::Step::Status::OK; }
  ci::Step::Status precheck() override { return ci::Step::Status::OK; }
};

}  // namespace

// this main of test binay in done purely for recovery smoke test.
int main(int argc, char** argv) {
  ci::PkgmgrInstaller pkgmgr_installer;
  wgt::WgtAppQueryInterface query_interface;
  auto pkgmgr = ci::PkgMgrInterface::Create(argc, argv, &pkgmgr_installer,
                                            &query_interface);
  if (!pkgmgr) {
    LOG(ERROR) << "Options of pkgmgr installer cannot be parsed";
    return EINVAL;
  }

  wgt::WgtInstaller installer(pkgmgr);
  installer.AddStep<StepCrash>();
  return (installer.Run() == ci::AppInstaller::Result::OK) ? 0 : 1;
}

