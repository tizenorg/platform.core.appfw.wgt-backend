// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "hybrid/hybrid_installer.h"

#include <common/step/step_check_signature.h>
#include <common/step/step_backup_icons.h>
#include <common/step/step_backup_manifest.h>
#include <common/step/step_configure.h>
#include <common/step/step_copy.h>
#include <common/step/step_copy_backup.h>
#include <common/step/step_copy_storage_directories.h>
#include <common/step/step_create_icons.h>
#include <common/step/step_create_storage_directories.h>
#include <common/step/step_delta_patch.h>
#include <common/step/step_fail.h>
#include <common/step/step_kill_apps.h>
#include <common/step/step_old_manifest.h>
#include <common/step/step_parse.h>
#include <common/step/step_privilege_compatibility.h>
#include <common/step/step_register_app.h>
#include <common/step/step_register_security.h>
#include <common/step/step_remove_files.h>
#include <common/step/step_remove_icons.h>
#include <common/step/step_revoke_security.h>
#include <common/step/step_rollback_deinstallation_security.h>
#include <common/step/step_rollback_installation_security.h>
#include <common/step/step_unregister_app.h>
#include <common/step/step_unzip.h>
#include <common/step/step_update_app.h>
#include <common/step/step_update_security.h>

#include <tpk/step/step_create_symbolic_link.h>
#include <tpk/step/step_parse.h>

#include "hybrid/hybrid_backend_data.h"
#include "hybrid/step/step_encrypt_resources.h"
#include "hybrid/step/step_merge_tpk_config.h"
#include "hybrid/step/step_stash_tpk_config.h"
#include "hybrid/step/step_parse.h"
#include "wgt/step/step_check_settings_level.h"
#include "wgt/step/step_check_wgt_background_category.h"
#include "wgt/step/step_create_symbolic_link.h"
#include "wgt/step/step_generate_xml.h"
#include "wgt/step/step_remove_encryption_data.h"
#include "wgt/step/step_wgt_create_icons.h"
#include "wgt/step/step_wgt_patch_storage_directories.h"

namespace ci = common_installer;

namespace hybrid {

HybridInstaller::HybridInstaller(common_installer::PkgMgrPtr pkgmgr)
    : AppInstaller("wgt", pkgmgr) {
  context_->backend_data.set(new HybridBackendData());

  switch (pkgmgr_->GetRequestType()) {
    case ci::RequestType::Install:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<tpk::parse::StepParse>();
      AddStep<hybrid::parse::StepStashTpkConfig>();
      AddStep<hybrid::parse::StepParse>(true);
      AddStep<hybrid::parse::StepMergeTpkConfig>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<hybrid::encrypt::StepEncryptResources>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::filesystem::StepCopy>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepWgtCreateIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCreateStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRegisterApplication>();
      AddStep<ci::security::StepRegisterSecurity>();
      break;
    case ci::RequestType::Update:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<tpk::parse::StepParse>();
      AddStep<hybrid::parse::StepStashTpkConfig>();
      AddStep<hybrid::parse::StepParse>(true);
      AddStep<hybrid::parse::StepMergeTpkConfig>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<hybrid::encrypt::StepEncryptResources>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::backup::StepOldManifest>();
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::backup::StepCopyBackup>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepWgtCreateIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCopyStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      break;
    case ci::RequestType::Uninstall:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      // TODO(t.iwanek): this parses both configuration files
      // tpk and wgt, removing pkgmgr-parser should change this code
      // that it will still support parsing both files
      AddStep<ci::parse::StepParse>();
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::pkgmgr::StepUnregisterApplication>();
      AddStep<ci::security::StepRollbackDeinstallationSecurity>();
      AddStep<ci::filesystem::StepRemoveFiles>();
      AddStep<ci::filesystem::StepRemoveIcons>();
      AddStep<wgt::encrypt::StepRemoveEncryptionData>();
      AddStep<ci::security::StepRevokeSecurity>();
      break;
    case ci::RequestType::Reinstall:
      // RDS is not supported for hybrid apps
      AddStep<ci::configuration::StepFail>();
      break;
    case ci::RequestType::Delta:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      // TODO(t.iwanek): manifest is parsed twice...
      AddStep<tpk::parse::StepParse>();
      AddStep<hybrid::parse::StepStashTpkConfig>();
      AddStep<hybrid::parse::StepParse>(false);
      AddStep<hybrid::parse::StepMergeTpkConfig>();
      AddStep<ci::filesystem::StepDeltaPatch>();
      AddStep<wgt::parse::StepParse>(true);
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<hybrid::encrypt::StepEncryptResources>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::backup::StepOldManifest>();
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::backup::StepCopyBackup>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepWgtCreateIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCopyStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      break;
    case ci::RequestType::Recovery:
      // TODO(t.iwanek): implement recovery for hybrid apps if possible
      AddStep<ci::configuration::StepFail>();
      break;
    default:
      AddStep<ci::configuration::StepFail>();
      break;
  }
}

}  // namespace hybrid

