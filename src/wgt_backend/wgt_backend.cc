/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <common/pkgmgr_interface.h>
#include <cerrno>

#include "hybrid/hybrid_installer.h"
#include "wgt/wgt_app_query_interface.h"
#include "wgt/wgt_installer.h"

namespace ci = common_installer;

int main(int argc, char** argv) {
  wgt::WgtAppQueryInterface query_interface;
  auto pkgmgr = ci::PkgMgrInterface::Create(argc, argv, &query_interface);
  if (!pkgmgr) {
    LOG(ERROR) << "Options of pkgmgr installer cannot be parsed";
    return EINVAL;
  }

  // This is workaround for hybrid apps as they requires much different flow
  // but installer does not branch at all in current design
  if (query_interface.IsHybridApplication(argc, argv)) {
    LOG(INFO) << "Hybrid package detected";
    hybrid::HybridInstaller installer(pkgmgr);
    return (installer.Run() == ci::AppInstaller::Result::OK) ? 0 : 1;
  } else {
    wgt::WgtInstaller installer(pkgmgr);
    return (installer.Run() == ci::AppInstaller::Result::OK) ? 0 : 1;
  }
}
