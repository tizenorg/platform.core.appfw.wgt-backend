SET(SRCS
  step/step_encrypt_resources.cc
  step/step_merge_tpk_config.cc
  step/step_parse.cc
  step/step_stash_tpk_config.cc
  hybrid_installer.cc
)
ADD_LIBRARY(${TARGET_LIBNAME_HYBRID} STATIC ${SRCS})

TARGET_INCLUDE_DIRECTORIES(${TARGET_LIBNAME_HYBRID} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")

# Target - deps
APPLY_PKG_CONFIG(${TARGET_LIBNAME_HYBRID} PUBLIC
  TPK_INSTALLER_DEPS
  Boost
)

# Target - in-package deps
TARGET_LINK_LIBRARIES(${TARGET_LIBNAME_HYBRID} PRIVATE ${TARGET_LIBNAME_WGT})
