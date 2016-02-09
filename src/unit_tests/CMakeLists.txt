SET(DESTINATION_DIR wgt-backend-ut)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../)

# Executables
ADD_EXECUTABLE(${TARGET_SMOKE_TEST}
  smoke_test.cc
)

INSTALL(DIRECTORY test_samples/ DESTINATION ${SHAREDIR}/${DESTINATION_DIR}/test_samples)

APPLY_PKG_CONFIG(smoke_test PUBLIC
  Boost
  GTEST
)

# FindGTest module do not sets all needed libraries in GTEST_LIBRARIES and
# GTest main libraries is still missing, so additional linking of
# GTEST_MAIN_LIBRARIES is needed.
target_link_libraries(smoke_test PRIVATE ${TARGET_LIBNAME_WGT} ${TARGET_LIBNAME_HYBRID} ${GTEST_MAIN_LIBRARIES})

INSTALL(TARGETS smoke_test DESTINATION ${BINDIR}/${DESTINATION_DIR})
