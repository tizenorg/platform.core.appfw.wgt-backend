// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_FILESYSTEM_STEP_WGT_PREPARE_PACKAGE_DIRECTORY_H_
#define WGT_STEP_FILESYSTEM_STEP_WGT_PREPARE_PACKAGE_DIRECTORY_H_

#include <manifest_parser/utils/logging.h>

#include <boost/filesystem/path.hpp>
#include <pkgmgrinfo_basic.h>

#include <string>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace wgt {
namespace filesystem {

/**
 * \brief Responsible for adjusting package directory after mounting zip package
 *        by StepMountInstall or StepMountUpdate
 *
 * This step will preserve old application symlinks in bin/ directory.
 */
class StepWgtPreparePackageDirectory : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override;

 private:
  Status CreateSymglinkToMountPoint();
  Status CreateBackupOfBinaryDirectory();
  Status RecoverBackupOfBinaryDirectory();

  SCOPE_LOG_TAG(WgtPreparePackageDirectory)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_FILESYSTEM_STEP_WGT_PREPARE_PACKAGE_DIRECTORY_H_
