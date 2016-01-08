// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef HYBRID_HYBRID_INSTALLER_H_
#define HYBRID_HYBRID_INSTALLER_H_

#include "common/app_installer.h"
#include "common/pkgmgr_interface.h"

namespace hybrid {

/**
 * @brief The TpkInstaller class
 *        Handles request of installation of native part of hybrid package.
 *
 * Pkgmgr request is parsed within and sequence of steps is built to be run.
 *
 * This backend is called by wgt-backend if it encounters hybrid package to
 * install native part of that package.
 */
class HybridInstaller : public common_installer::AppInstaller {
 public:
  explicit HybridInstaller(common_installer::PkgMgrPtr pkgmgr);

  SCOPE_LOG_TAG(HybridInstaller)
};

}  // namespace hybrid

#endif  // HYBRID_HYBRID_INSTALLER_H_
