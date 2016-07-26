// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "hybrid/hybrid_installer.h"

#include <common/step/backup/step_backup_icons.h>
#include <common/step/backup/step_backup_manifest.h>
#include <common/step/backup/step_copy_backup.h>
#include <common/step/configuration/step_block_cross_update.h>
#include <common/step/configuration/step_check_tizen_version.h>
#include <common/step/configuration/step_configure.h>
#include <common/step/configuration/step_fail.h>
#include <common/step/configuration/step_parse_manifest.h>
#include <common/step/filesystem/step_acquire_external_storage.h>
#include <common/step/filesystem/step_clear_data.h>
#include <common/step/filesystem/step_copy.h>
#include <common/step/filesystem/step_copy_storage_directories.h>
#include <common/step/filesystem/step_copy_tep.h>
#include <common/step/filesystem/step_create_icons.h>
#include <common/step/filesystem/step_create_per_user_storage_directories.h>
#include <common/step/filesystem/step_create_globalapp_symlinks.h>
#include <common/step/filesystem/step_create_storage_directories.h>
#include <common/step/filesystem/step_delta_patch.h>
#include <common/step/filesystem/step_recover_files.h>
#include <common/step/filesystem/step_recover_icons.h>
#include <common/step/filesystem/step_recover_manifest.h>
#include <common/step/filesystem/step_recover_external_storage.h>
#include <common/step/filesystem/step_recover_storage_directories.h>
#include <common/step/filesystem/step_remove_files.h>
#include <common/step/filesystem/step_remove_icons.h>
#include <common/step/filesystem/step_remove_per_user_storage_directories.h>
#include <common/step/filesystem/step_remove_globalapp_symlinks.h>
#include <common/step/filesystem/step_remove_temporary_directory.h>
#include <common/step/filesystem/step_remove_zip_image.h>
#include <common/step/filesystem/step_remove_tep.h>
#include <common/step/filesystem/step_unzip.h>
#include <common/step/filesystem/step_update_tep.h>
#include <common/step/mount/step_mount_install.h>
#include <common/step/mount/step_mount_unpacked.h>
#include <common/step/mount/step_mount_update.h>
#include <common/step/pkgmgr/step_check_removable.h>
#include <common/step/pkgmgr/step_check_restriction.h>
#include <common/step/pkgmgr/step_kill_apps.h>
#include <common/step/pkgmgr/step_recover_application.h>
#include <common/step/pkgmgr/step_register_app.h>
#include <common/step/pkgmgr/step_remove_manifest.h>
#include <common/step/pkgmgr/step_run_parser_plugins.h>
#include <common/step/pkgmgr/step_unregister_app.h>
#include <common/step/pkgmgr/step_update_app.h>
#include <common/step/pkgmgr/step_update_pkg_disable_info.h>
#include <common/step/recovery/step_open_recovery_file.h>
#include <common/step/security/step_check_signature.h>
#include <common/step/security/step_privilege_compatibility.h>
#include <common/step/security/step_recover_security.h>
#include <common/step/security/step_register_security.h>
#include <common/step/security/step_revoke_security.h>
#include <common/step/security/step_rollback_deinstallation_security.h>
#include <common/step/security/step_rollback_installation_security.h>
#include <common/step/security/step_update_security.h>

#include <tpk/step/filesystem/step_create_symbolic_link.h>
#include <tpk/step/filesystem/step_tpk_patch_icons.h>
#include <tpk/step/filesystem/step_tpk_prepare_package_directory.h>
#include <tpk/step/filesystem/step_tpk_update_package_directory.h>

#include "hybrid/hybrid_backend_data.h"
#include "hybrid/step/configuration/step_merge_tpk_config.h"
#include "hybrid/step/configuration/step_stash_tpk_config.h"
#include "hybrid/step/encryption/step_encrypt_resources.h"
#include "wgt/step/configuration/step_parse.h"
#include "wgt/step/encryption/step_remove_encryption_data.h"
#include "wgt/step/filesystem/step_copy_preview_icons.h"
#include "wgt/step/filesystem/step_create_symbolic_link.h"
#include "wgt/step/filesystem/step_wgt_patch_icons.h"
#include "wgt/step/filesystem/step_wgt_patch_storage_directories.h"
#include "wgt/step/pkgmgr/step_generate_xml.h"
#include "wgt/step/security/step_check_settings_level.h"
#include "wgt/step/security/step_check_wgt_background_category.h"
#include "wgt/step/security/step_check_wgt_notification_category.h"
#include "wgt/step/security/step_check_wgt_ime_privilege.h"

namespace ci = common_installer;

namespace hybrid {

HybridInstaller::HybridInstaller(common_installer::PkgMgrPtr pkgmgr)
    : AppInstaller("wgt", pkgmgr) {
  context_->backend_data.set(new HybridBackendData());

  switch (pkgmgr_->GetRequestType()) {
    case ci::RequestType::Install:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::PACKAGE,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<hybrid::configuration::StepStashTpkConfig>();
      AddStep<wgt::configuration::StepParse>(
          wgt::configuration::StepParse::ConfigLocation::RESOURCE_WGT, true);
      AddStep<hybrid::configuration::StepMergeTpkConfig>();
      AddStep<ci::pkgmgr::StepCheckRestriction>();
      AddStep<ci::configuration::StepCheckTizenVersion>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<wgt::security::StepCheckWgtNotificationCategory>();
      AddStep<wgt::security::StepCheckWgtImePrivilege>();
      AddStep<hybrid::encryption::StepEncryptResources>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::filesystem::StepAcquireExternalStorage>();
      AddStep<ci::filesystem::StepCopy>();
      AddStep<ci::filesystem::StepCopyTep>();
      AddStep<tpk::filesystem::StepTpkPatchIcons>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepCopyPreviewIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCreateStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Install);
      AddStep<ci::pkgmgr::StepRegisterApplication>();
      AddStep<ci::security::StepRegisterSecurity>();
      AddStep<ci::filesystem::StepCreatePerUserStorageDirectories>();
      AddStep<ci::filesystem::StepCreateGlobalAppSymlinks>();
      break;
    case ci::RequestType::Update:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::PACKAGE,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<hybrid::configuration::StepStashTpkConfig>();
      AddStep<wgt::configuration::StepParse>(
          wgt::configuration::StepParse::ConfigLocation::RESOURCE_WGT, true);
      AddStep<hybrid::configuration::StepMergeTpkConfig>();
      AddStep<ci::configuration::StepCheckTizenVersion>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<wgt::security::StepCheckWgtNotificationCategory>();
      AddStep<wgt::security::StepCheckWgtImePrivilege>();
      AddStep<hybrid::encryption::StepEncryptResources>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::BACKUP);
      AddStep<ci::configuration::StepBlockCrossUpdate>();
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::filesystem::StepAcquireExternalStorage>();
      AddStep<ci::backup::StepCopyBackup>();
      AddStep<ci::filesystem::StepUpdateTep>();
      AddStep<tpk::filesystem::StepTpkPatchIcons>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepCopyPreviewIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCopyStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Upgrade);
      AddStep<ci::filesystem::StepRemoveGlobalAppSymlinks>();
      AddStep<ci::filesystem::StepCreateGlobalAppSymlinks>();
      break;
    case ci::RequestType::Uninstall:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::pkgmgr::StepCheckRestriction>();
      AddStep<ci::pkgmgr::StepCheckRemovable>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Uninstall);
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::filesystem::StepAcquireExternalStorage>();
      AddStep<ci::filesystem::StepRemoveGlobalAppSymlinks>();
      AddStep<ci::filesystem::StepRemovePerUserStorageDirectories>();
      AddStep<ci::pkgmgr::StepUnregisterApplication>();
      AddStep<ci::security::StepRollbackDeinstallationSecurity>();
      AddStep<ci::filesystem::StepRemoveTep>();
      AddStep<ci::filesystem::StepRemoveFiles>();
      AddStep<ci::filesystem::StepRemoveZipImage>();
      AddStep<ci::filesystem::StepRemoveIcons>();
      AddStep<wgt::encryption::StepRemoveEncryptionData>();
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
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::PACKAGE,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<hybrid::configuration::StepStashTpkConfig>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::BACKUP);
      AddStep<ci::filesystem::StepDeltaPatch>();
      AddStep<wgt::configuration::StepParse>(
          wgt::configuration::StepParse::ConfigLocation::RESOURCE_WGT, true);
      AddStep<hybrid::configuration::StepMergeTpkConfig>();
      AddStep<ci::configuration::StepCheckTizenVersion>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<wgt::security::StepCheckWgtNotificationCategory>();
      AddStep<wgt::security::StepCheckWgtImePrivilege>();
      AddStep<hybrid::encryption::StepEncryptResources>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::configuration::StepBlockCrossUpdate>();
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::filesystem::StepAcquireExternalStorage>();
      AddStep<ci::backup::StepCopyBackup>();
      AddStep<ci::filesystem::StepUpdateTep>();
      AddStep<tpk::filesystem::StepTpkPatchIcons>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepCopyPreviewIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCopyStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Upgrade);
      AddStep<ci::filesystem::StepRemoveGlobalAppSymlinks>();
      AddStep<ci::filesystem::StepCreateGlobalAppSymlinks>();
      break;
    case ci::RequestType::Recovery:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::recovery::StepOpenRecoveryFile>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::RECOVERY,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<ci::pkgmgr::StepRecoverApplication>();
      AddStep<ci::filesystem::StepRemoveTemporaryDirectory>();
      AddStep<ci::filesystem::StepRecoverIcons>();
      AddStep<ci::filesystem::StepRecoverManifest>();
      AddStep<ci::filesystem::StepRecoverExternalStorage>();
      AddStep<ci::filesystem::StepRecoverStorageDirectories>();
      AddStep<ci::filesystem::StepRecoverFiles>();
      AddStep<ci::security::StepRecoverSecurity>();
      break;
    case ci::RequestType::Clear:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<ci::filesystem::StepClearData>();
      break;
    case ci::RequestType::MountInstall:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::mount::StepMountUnpacked>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::PACKAGE,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<hybrid::configuration::StepStashTpkConfig>();
      AddStep<wgt::configuration::StepParse>(
          wgt::configuration::StepParse::ConfigLocation::RESOURCE_WGT, true);
      AddStep<hybrid::configuration::StepMergeTpkConfig>();
      AddStep<ci::pkgmgr::StepCheckRestriction>();
      AddStep<ci::configuration::StepCheckTizenVersion>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<wgt::security::StepCheckWgtNotificationCategory>();
      AddStep<wgt::security::StepCheckWgtImePrivilege>();
      AddStep<hybrid::encryption::StepEncryptResources>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::mount::StepMountInstall>();
      AddStep<tpk::filesystem::StepTpkPreparePackageDirectory>();
      AddStep<ci::filesystem::StepCopyTep>();
      AddStep<tpk::filesystem::StepTpkPatchIcons>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepCopyPreviewIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<ci::filesystem::StepCreateStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Install);
      AddStep<ci::pkgmgr::StepRegisterApplication>();
      AddStep<ci::security::StepRegisterSecurity>();
      AddStep<ci::filesystem::StepCreatePerUserStorageDirectories>();
      AddStep<ci::filesystem::StepCreateGlobalAppSymlinks>();
      break;
    case ci::RequestType::MountUpdate:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::mount::StepMountUnpacked>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::PACKAGE,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<hybrid::configuration::StepStashTpkConfig>();
      AddStep<wgt::configuration::StepParse>(
          wgt::configuration::StepParse::ConfigLocation::RESOURCE_WGT, true);
      AddStep<hybrid::configuration::StepMergeTpkConfig>();
      AddStep<ci::configuration::StepCheckTizenVersion>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<wgt::security::StepCheckWgtNotificationCategory>();
      AddStep<wgt::security::StepCheckWgtImePrivilege>();
      AddStep<hybrid::encryption::StepEncryptResources>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::BACKUP);
      AddStep<ci::configuration::StepBlockCrossUpdate>();
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::mount::StepMountUpdate>();
      AddStep<tpk::filesystem::StepTpkUpdatePackageDirectory>();
      AddStep<ci::filesystem::StepUpdateTep>();
      AddStep<tpk::filesystem::StepTpkPatchIcons>();
      AddStep<wgt::filesystem::StepWgtPatchIcons>();
      AddStep<ci::filesystem::StepCreateIcons>();
      AddStep<wgt::filesystem::StepCopyPreviewIcons>();
      AddStep<wgt::filesystem::StepWgtPatchStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<tpk::filesystem::StepCreateSymbolicLink>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Upgrade);
      AddStep<ci::filesystem::StepRemoveGlobalAppSymlinks>();
      AddStep<ci::filesystem::StepCreateGlobalAppSymlinks>();
      break;
    case ci::RequestType::ManifestDirectInstall:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<hybrid::configuration::StepStashTpkConfig>();
      AddStep<wgt::configuration::StepParse>(
          wgt::configuration::StepParse::ConfigLocation::INSTALLED, true);
      AddStep<hybrid::configuration::StepMergeTpkConfig>();
      AddStep<ci::configuration::StepCheckTizenVersion>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Install);
      AddStep<ci::pkgmgr::StepRegisterApplication>();
      AddStep<ci::security::StepRegisterSecurity>();
      AddStep<ci::filesystem::StepCreatePerUserStorageDirectories>();
      AddStep<ci::filesystem::StepCreateGlobalAppSymlinks>();
      break;
    case ci::RequestType::ManifestDirectUpdate:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<hybrid::configuration::StepStashTpkConfig>();
      AddStep<wgt::configuration::StepParse>(
          wgt::configuration::StepParse::ConfigLocation::INSTALLED, true);
      AddStep<hybrid::configuration::StepMergeTpkConfig>();
      AddStep<ci::configuration::StepCheckTizenVersion>();
      AddStep<ci::security::StepCheckSignature>();
      AddStep<ci::security::StepPrivilegeCompatibility>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::security::StepCheckWgtBackgroundCategory>();
      AddStep<ci::security::StepRollbackInstallationSecurity>();
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::BACKUP);
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<wgt::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
          ci::Plugin::ActionType::Upgrade);
      AddStep<ci::filesystem::StepRemoveGlobalAppSymlinks>();
      AddStep<ci::filesystem::StepCreateGlobalAppSymlinks>();
      break;
    case ci::RequestType::EnablePkg:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::pkgmgr::StepUpdatePkgDisableInfo>(
        ci::pkgmgr::StepUpdatePkgDisableInfo::ActionType::Enable);
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
        ci::Plugin::ActionType::Uninstall);
      break;
    case ci::RequestType::DisablePkg:
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::configuration::StepParseManifest>(
          ci::configuration::StepParseManifest::ManifestLocation::INSTALLED,
          ci::configuration::StepParseManifest::StoreLocation::NORMAL);
      AddStep<ci::pkgmgr::StepKillApps>();
      AddStep<ci::pkgmgr::StepUpdatePkgDisableInfo>(
        ci::pkgmgr::StepUpdatePkgDisableInfo::ActionType::Disable);
      AddStep<ci::pkgmgr::StepRunParserPlugin>(
        ci::Plugin::ActionType::Uninstall);
      break;
    default:
      AddStep<ci::configuration::StepFail>();
      break;
  }
}

}  // namespace hybrid

