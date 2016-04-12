// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/wgt_installer.h"

#include <common/pkgmgr_interface.h>

#include <common/step/backup/step_backup_icons.h>
#include <common/step/backup/step_backup_manifest.h>
#include <common/step/backup/step_copy_backup.h>
#include <common/step/configuration/step_block_cross_update.h>
#include <common/step/configuration/step_configure.h>
#include <common/step/configuration/step_fail.h>
#include <common/step/configuration/step_parse_manifest.h>
#include <common/step/filesystem/step_clear_data.h>
#include <common/step/filesystem/step_copy.h>
#include <common/step/filesystem/step_copy_storage_directories.h>
#include <common/step/filesystem/step_copy_tep.h>
#include <common/step/filesystem/step_create_icons.h>
#include <common/step/filesystem/step_create_per_user_storage_directories.h>
#include <common/step/filesystem/step_create_storage_directories.h>
#include <common/step/filesystem/step_delta_patch.h>
#include <common/step/filesystem/step_recover_files.h>
#include <common/step/filesystem/step_recover_icons.h>
#include <common/step/filesystem/step_recover_manifest.h>
#include <common/step/filesystem/step_recover_storage_directories.h>
#include <common/step/filesystem/step_remove_files.h>
#include <common/step/filesystem/step_remove_icons.h>
#include <common/step/filesystem/step_remove_per_user_storage_directories.h>
#include <common/step/filesystem/step_remove_temporary_directory.h>
#include <common/step/filesystem/step_remove_zip_image.h>
#include <common/step/filesystem/step_unzip.h>
#include <common/step/mount/step_mount_install.h>
#include <common/step/mount/step_mount_unpacked.h>
#include <common/step/mount/step_mount_update.h>
#include <common/step/pkgmgr/step_check_blacklist.h>
#include <common/step/pkgmgr/step_check_removable.h>
#include <common/step/pkgmgr/step_kill_apps.h>
#include <common/step/pkgmgr/step_recover_application.h>
#include <common/step/pkgmgr/step_register_app.h>
#include <common/step/pkgmgr/step_remove_manifest.h>
#include <common/step/pkgmgr/step_run_parser_plugins.h>
#include <common/step/pkgmgr/step_unregister_app.h>
#include <common/step/pkgmgr/step_update_app.h>
#include <common/step/pkgmgr/step_update_tep.h>
#include <common/step/recovery/step_open_recovery_file.h>
#include <common/step/security/step_check_old_certificate.h>
#include <common/step/security/step_check_signature.h>
#include <common/step/security/step_privilege_compatibility.h>
#include <common/step/security/step_recover_security.h>
#include <common/step/security/step_register_security.h>
#include <common/step/security/step_revoke_security.h>
#include <common/step/security/step_rollback_deinstallation_security.h>
#include <common/step/security/step_rollback_installation_security.h>
#include <common/step/security/step_update_security.h>

#include <wgt_manifest_handlers/widget_config_parser.h>
#include <common/step/rds/step_rds_modify.h>
#include <common/step/rds/step_rds_parse.h>

#include "wgt/step/configuration/step_parse.h"
#include "wgt/step/configuration/step_parse_recovery.h"
#include "wgt/step/encryption/step_encrypt_resources.h"
#include "wgt/step/encryption/step_remove_encryption_data.h"
#include "wgt/step/filesystem/step_create_symbolic_link.h"
#include "wgt/step/filesystem/step_wgt_patch_icons.h"
#include "wgt/step/filesystem/step_wgt_patch_storage_directories.h"
#include "wgt/step/filesystem/step_wgt_prepare_package_directory.h"
#include "wgt/step/filesystem/step_wgt_resource_directory.h"
#include "wgt/step/filesystem/step_wgt_update_package_directory.h"
#include "wgt/step/pkgmgr/step_generate_xml.h"
#include "wgt/step/rds/step_wgt_rds_modify.h"
#include "wgt/step/security/step_add_default_privileges.h"
#include "wgt/step/security/step_check_settings_level.h"
#include "wgt/step/security/step_check_wgt_background_category.h"

namespace ci = common_installer;

namespace wgt {

WgtInstaller::WgtInstaller(ci::PkgMgrPtr pkgrmgr)
    : AppInstaller("wgt", pkgrmgr) {
  context_->backend_data.set(new WgtBackendData());

  /* treat the request */
  switch (pkgmgr_->GetRequestType()) {
    case ci::RequestType::Install : {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<wgt::configuration::StepParse>(true);
      AddStep<ci::pkgmgr::StepCheckBlacklist>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<wgt::encryption::StepEncryptResources>();
      AddStep<wgt::filesystem::StepWgtResourceDirectory>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::filesystem::StepCopy>();
      AddStep<ci::filesystem::StepCopyTep>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCreateStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRegisterApplication>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Install);
      AddStep<ci::filesystem::StepCreatePerUserStorageDirectories>();
      AddStep<ci::security::StepRegisterSecurity>();
      break;
    }
    case ci::RequestType::Update: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<wgt::configuration::StepParse>(true);
      AddStep<ci::pkgmgr::StepCheckBlacklist>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<ci::security::StepCheckOldCertificate>();
      AddStep<wgt::filesystem::StepWgtResourceDirectory>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::BACKUP);
      AddStep<ci::configuration::StepBlockCrossUpdate>();
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::backup::StepCopyBackup>();
      AddStep<ci::filesystem::StepCopyTep>();
      AddStep<ci::pkgmgr::StepUpdateTep>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<ci::filesystem::StepCopyStorageDirectories>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Upgrade);
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      break;
    }
    case ci::RequestType::Uninstall: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::pkgmgr::StepCheckRemovable>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Uninstall);
      AddStep<ci::filesystem::StepRemovePerUserStorageDirectories>();
      AddStep<ci::pkgmgr::StepUnregisterApplication>();
      AddStep<ci::security::StepRollbackDeinstallationSecurity>();
      AddStep<ci::filesystem::StepRemoveFiles>();
      AddStep<ci::filesystem::StepRemoveZipImage>();
      AddStep<ci::filesystem::StepRemoveIcons>();
      AddStep<wgt::encryption::StepRemoveEncryptionData>();
      AddStep<ci::security::StepRevokeSecurity>();
      AddStep<ci::pkgmgr::StepRemoveManifest>();
      break;
    }
    case ci::RequestType::Reinstall: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<wgt::configuration::StepParse>(false);
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::BACKUP);
      AddStep<ci::configuration::StepBlockCrossUpdate>();
      AddStep<ci::pkgmgr::StepCheckBlacklist>();
      AddStep<ci::rds::StepRDSParse>();
      AddStep<wgt::rds::StepWgtRDSModify>();
      AddStep<ci::security::StepUpdateSecurity>();
      break;
    }
    case ci::RequestType::Delta: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      // TODO(t.iwanek): manifest is parsed twice...
      AddStep<wgt::configuration::StepParse>(false);
      // start file may not have changed
      AddStep<ci::filesystem::StepDeltaPatch>("res/wgt/");
      AddStep<wgt::configuration::StepParse>(true);
      AddStep<ci::pkgmgr::StepCheckBlacklist>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<ci::security::StepCheckOldCertificate>();
      AddStep<wgt::filesystem::StepWgtResourceDirectory>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::BACKUP);
      AddStep<ci::configuration::StepBlockCrossUpdate>();
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::backup::StepCopyBackup>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<ci::filesystem::StepCopyStorageDirectories>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Upgrade);
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      break;
    }
    case ci::RequestType::Recovery: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::recovery::StepOpenRecoveryFile>();
      AddStep<wgt::configuration::StepParseRecovery>();
      AddStep<ci::pkgmgr::StepRecoverApplication>();
      AddStep<ci::filesystem::StepRemoveTemporaryDirectory>();
      AddStep<ci::filesystem::StepRecoverIcons>();
      AddStep<ci::filesystem::StepRecoverManifest>();
      AddStep<ci::filesystem::StepRecoverStorageDirectories>();
      AddStep<ci::filesystem::StepRecoverFiles>();
      AddStep<ci::security::StepRecoverSecurity>();
      break;
    }
    case ci::RequestType::Clear: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepClearData>();
      break;
    }
    case ci::RequestType::MountInstall: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::mount::StepMountUnpacked>();
      AddStep<wgt::configuration::StepParse>(true);
      AddStep<ci::pkgmgr::StepCheckBlacklist>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<wgt::encryption::StepEncryptResources>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::mount::StepMountInstall>();
      AddStep<wgt::filesystem::StepWgtPreparePackageDirectory>();
      AddStep<ci::filesystem::StepCopyTep>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCreateStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRegisterApplication>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Install);
      AddStep<ci::filesystem::StepCreatePerUserStorageDirectories>();
      AddStep<ci::security::StepRegisterSecurity>();
      break;
    }
    case ci::RequestType::MountUpdate: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::mount::StepMountUnpacked>();
      AddStep<wgt::configuration::StepParse>(true);
      AddStep<ci::pkgmgr::StepCheckBlacklist>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<ci::security::StepCheckOldCertificate>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::BACKUP);
      AddStep<ci::configuration::StepBlockCrossUpdate>();
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::mount::StepMountUpdate>();
      AddStep<wgt::filesystem::StepWgtUpdatePackageDirectory>();
      AddStep<ci::filesystem::StepCopyTep>();
      AddStep<ci::pkgmgr::StepUpdateTep>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Upgrade);
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      break;
    }
    default: {
      AddStep<ci::configuration::StepFail>();
    }
  }
}

}  // namespace wgt
