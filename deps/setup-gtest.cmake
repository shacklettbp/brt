set(GTEST_BUNDLED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gtest")
set(GTEST_BUILD_TIMESTAMP_FILE "${BRT_BUNDLE_TMP_DIR}/gtest-build-stamp")
set(GTEST_BUILD_CONFIG_HASH_FILE "${BRT_BUNDLE_TMP_DIR}/gtest-build-config-hash")

function(fetch_gtest)
  FetchContent_Populate(gtest-bundled
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG e4ece4881d1fefc1e67d21c7493835815cd13085
    GIT_PROGRESS ON
    GIT_SUBMODULES ""
    GIT_SUBMODULES_RECURSE OFF
    SOURCE_DIR "${GTEST_BUNDLED_DIR}"
  )

  file(SHA512 "${CMAKE_CURRENT_LIST_FILE}" GTEST_CONFIG_FILE_HASH)

  file(TOUCH "${GTEST_BUILD_TIMESTAMP_FILE}")
  file(WRITE "${GTEST_BUILD_CONFIG_HASH_FILE}" "${GTEST_CONFIG_FILE_HASH}")
endfunction()

function(check_fetch_gtest)
  file(SHA512 "${CMAKE_CURRENT_LIST_FILE}" GTEST_CONFIG_FILE_HASH)

  if (EXISTS "${GTEST_BUILD_TIMESTAMP_FILE}")
    file(READ "${GTEST_BUILD_TIMESTAMP_FILE}" CUR_BUILD_TIMESTAMP)
  else()
    set(CUR_BUILD_TIMESTAMP "")
  endif()

  if (EXISTS "${GTEST_BUILD_CONFIG_HASH_FILE}")
    file(READ "${GTEST_BUILD_CONFIG_HASH_FILE}" CUR_BUILD_CONFIG_HASH)
  else()
    set(CUR_BUILD_CONFIG_HASH "")
  endif()

  set(NEED_FETCH_GTEST FALSE)
  if (NOT "${CUR_BUILD_CONFIG_HASH}" MATCHES "${GTEST_BUILD_CONFIG_FILE_HASH}")
    set(NEED_FETCH_GTEST TRUE)
  endif()

  if (NOT EXISTS "${GTEST_BUILD_TIMESTAMP_FILE}")
    set(NEED_FETCH_GTEST TRUE)
  else()
    if ("${CMAKE_CURRENT_LIST_FILE}" IS_NEWER_THAN "${GTEST_BUILD_TIMESTAMP_FILE}")
      set(NEED_FETCH_GTEST TRUE)
    endif()
  endif()

  if (NEED_FETCH_GTEST)
    fetch_gtest()
  endif()
endfunction()

check_fetch_gtest()
unset(fetch_gtest)
unset(check_fetch_gtest)
