# Target - sources
SET(SRCS
  step/configuration/step_parse.cc
  step/configuration/step_parse_recovery.cc
  step/encryption/step_encrypt_resources.cc
  step/encryption/step_remove_encryption_data.cc
  step/filesystem/step_create_symbolic_link.cc
  step/filesystem/step_wgt_patch_icons.cc
  step/filesystem/step_wgt_patch_storage_directories.cc
  step/filesystem/step_wgt_prepare_package_directory.cc
  step/filesystem/step_wgt_resource_directory.cc
  step/filesystem/step_wgt_update_package_directory.cc
  step/pkgmgr/step_generate_xml.cc
  step/rds/step_wgt_rds_modify.cc
  step/security/step_add_default_privileges.cc
  step/security/step_check_settings_level.cc
  step/security/step_check_wgt_background_category.cc
  wgt_app_query_interface.cc
  wgt_installer.cc
)

IF(WRT_LAUNCHER)
    ADD_DEFINITIONS("-DWRT_LAUNCHER=\"${WRT_LAUNCHER}\"")
    MESSAGE( "WRT LAUNCHER binary path is  set to ${WRT_LAUNCHER}")
ELSE(WRT_LAUNCHER)
    MESSAGE(FATAL_ERROR, "WRT LAUNCHER binary path is not set")
ENDIF(WRT_LAUNCHER)

# Target - definition
ADD_LIBRARY(${TARGET_LIBNAME_WGT} STATIC ${SRCS})
# Target - includes
TARGET_INCLUDE_DIRECTORIES(${TARGET_LIBNAME_WGT} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")
# Target - deps
APPLY_PKG_CONFIG(${TARGET_LIBNAME_WGT} PUBLIC
  APP_INSTALLERS_DEPS
  WGT_MANIFEST_HANDLERS_DEPS
  MANIFEST_PARSER_DEPS
  PKGMGR_INSTALLER_DEPS
  ENCRYPTION_DEPS
  Boost
)
