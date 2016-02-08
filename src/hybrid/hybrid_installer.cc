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
#include <common/step/step_clear_data.h>
#include <common/step/step_create_icons.h>
#include <common/step/step_create_storage_directories.h>
#include <common/step/step_delta_patch.h>
#include <common/step/step_fail.h>
#include <common/step/step_kill_apps.h>
#include <common/step/step_parse_manifest.h>
#include <common/step/step_privilege_compatibility.h>
#include <common/step/step_register_app.h>
#include <common/step/step_register_security.h>
#include <common/step/step_remove_files.h>
#include <common/step/step_remove_icons.h>
#include <common/step/step_revoke_security.h>
#include <common/step/step_open_recovery_file.h>
#include <common/step/step_recover_application.h>
#include <common/step/step_recover_files.h>
#include <common/step/step_recover_icons.h>
#include <common/step/step_recover_manifest.h>
#include <common/step/step_recover_security.h>
#include <common/step/step_recover_storage_directories.h>
#include <common/step/step_remove_temporary_directory.h>
#include <common/step/step_rollback_deinstallation_security.h>
#include <common/step/step_rollback_installation_security.h>
#include <common/step/step_run_parser_plugins.h>
#include <common/step/step_unregister_app.h>
#include <common/step/step_unzip.h>
#include <common/step/step_update_app.h>
#include <common/step/step_update_security.h>
#include <common/step/step_remove_manifest.h>

#include <tpk/step/step_create_symbolic_link.h>
#include <tpk/step/step_tpk_patch_icons.h>

#include "hybrid/hybrid_backend_data.h"
#include "hybrid/step/step_encrypt_resources.h"
#include "hybrid/step/step_merge_tpk_config.h"
#include "hybrid/step/step_stash_tpk_config.h"
#include "hybrid/step/step_parse.h"
#include "wgt/step/step_check_settings_level.h"
#include "wgt/step/step_check_wgt_background_category.h"
#include "wgt/step/step_create_symbolic_link.h"
#include "wgt/step/step_generate_xml.h"
#include "wgt/step/step_parse_recovery.h"
#include "wgt/step/step_remove_encryption_data.h"
#include "wgt/step/step_wgt_patch_icons.h"
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
      AddStep<ci::parse::StepParseManifest>(
          ci::parse::StepParseManifest::ManifestLocation::PACKAGE,
          ci::parse::StepParseManifest::StoreLocation::NORMAL);
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
      AddStep<tpk::filesystem::StepTpkPatchIcons>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCreateStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Install);
      AddStep<ci::pkgmgr::StepRegisterApplication>();
      AddStep<ci::security::StepRegisterSecurity>();
      break;
    case ci::RequestType::Update:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<ci::parse::StepParseManifest>(
          ci::parse::StepParseManifest::ManifestLocation::PACKAGE,
          ci::parse::StepParseManifest::StoreLocation::NORMAL);
      AddStep<hybrid::parse::StepStashTpkConfig>();
      AddStep<hybrid::parse::StepParse>(true);
      AddStep<hybrid::parse::StepMergeTpkConfig>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<hybrid::encrypt::StepEncryptResources>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::parse::StepParseManifest>(
          ci::parse::StepParseManifest::ManifestLocation::INSTALLED,
          ci::parse::StepParseManifest::StoreLocation::BACKUP);
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::backup::StepCopyBackup>();
      AddStep<tpk::filesystem::StepTpkPatchIcons>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCopyStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Upgrade);
      break;
    case ci::RequestType::Uninstall:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      // TODO(t.iwanek): this parses both configuration files
      // tpk and wgt, removing pkgmgr-parser should change this code
      // that it will still support parsing both files
      AddStep<ci::parse::StepParseManifest>(
          ci::parse::StepParseManifest::ManifestLocation::INSTALLED,
          ci::parse::StepParseManifest::StoreLocation::NORMAL);
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Uninstall);
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::pkgmgr::StepUnregisterApplication>();
      AddStep<ci::security::StepRollbackDeinstallationSecurity>();
      AddStep<ci::filesystem::StepRemoveFiles>();
      AddStep<ci::filesystem::StepRemoveIcons>();
      AddStep<wgt::encrypt::StepRemoveEncryptionData>();
      AddStep<ci::security::StepRevokeSecurity>();
      AddStep<ci::pkgmgr::StepRemoveManifest>();
      break;
    case ci::RequestType::Reinstall:
      // RDS is not supported for hybrid apps
      AddStep<ci::configuration::StepFail>();
      break;
    case ci::RequestType::Delta:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<ci::parse::StepParseManifest>(
          ci::parse::StepParseManifest::ManifestLocation::PACKAGE,
          ci::parse::StepParseManifest::StoreLocation::NORMAL);
      AddStep<hybrid::parse::StepStashTpkConfig>();
      // TODO(t.iwanek): manifest is parsed twice...
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
      AddStep<ci::parse::StepParseManifest>(
          ci::parse::StepParseManifest::ManifestLocation::INSTALLED,
          ci::parse::StepParseManifest::StoreLocation::BACKUP);
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::backup::StepCopyBackup>();
      AddStep<tpk::filesystem::StepTpkPatchIcons>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCopyStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Upgrade);
      break;
    case ci::RequestType::Recovery:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::recovery::StepOpenRecoveryFile>();
      AddStep<ci::parse::StepParseManifest>(
          ci::parse::StepParseManifest::ManifestLocation::RECOVERY,
          ci::parse::StepParseManifest::StoreLocation::NORMAL);
      AddStep<hybrid::parse::StepStashTpkConfig>();
      AddStep<wgt::parse::StepParseRecovery>();
      AddStep<hybrid::parse::StepMergeTpkConfig>();
      AddStep<ci::pkgmgr::StepRecoverApplication>();
      AddStep<ci::filesystem::StepRemoveTemporaryDirectory>();
      AddStep<ci::filesystem::StepRecoverIcons>();
      AddStep<ci::filesystem::StepRecoverManifest>();
      AddStep<ci::filesystem::StepRecoverStorageDirectories>();
      AddStep<ci::filesystem::StepRecoverFiles>();
      AddStep<ci::security::StepRecoverSecurity>();
      break;
    case ci::RequestType::Clear:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepClearData>();
      break;
    default:
      AddStep<ci::configuration::StepFail>();
      break;
  }
}

}  // namespace hybrid

