if (BRT_USE_BUNDLED_TOOLCHAIN)
  add_subdirectory(toolchain)
endif()

set(BRT_BUNDLE_TMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bundle-tmp")
file(MAKE_DIRECTORY "${BRT_BUNDLE_TMP_DIR}")
include(setup-gtest.cmake)

add_subdirectory(gtest EXCLUDE_FROM_ALL)
target_link_libraries(gtest_main PUBLIC
  brt-cxx-flags brt-cxx-noexceptrtti brt-libcxx)
target_link_libraries(gtest PUBLIC
  brt-cxx-flags brt-cxx-noexceptrtti brt-libcxx)
