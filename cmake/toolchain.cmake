include(FetchContent)

if (WIN32)
  set(BRT_USE_BUNDLED_TOOLCHAIN_DEFAULT OFF)
else()
  set(BRT_USE_BUNDLED_TOOLCHAIN_DEFAULT ON)
endif()

option(BRT_USE_BUNDLED_TOOLCHAIN "Use prebuilt toolchain" ${BRT_USE_BUNDLED_TOOLCHAIN_DEFAULT})
unset(BRT_USE_BUNDLED_TOOLCHAIN_DEFAULT)
if (NOT BRT_USE_BUNDLED_TOOLCHAIN)
    return()
endif()

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_OBJC_STANDARD 17)
set(CMAKE_OBJC_EXTENSIONS OFF)

# Default hidden visibility for all symbols
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)


function(brt_setup_toolchain)
    cmake_path(GET CMAKE_CURRENT_FUNCTION_LIST_DIR PARENT_PATH BRT_DIR)

    set(DEPS_DIR "${BRT_DIR}/deps")

    FetchContent_Populate(madrona-toolchain-src
        GIT_REPOSITORY https://github.com/shacklettbp/madrona-toolchain.git
        GIT_TAG 8c0b55b52c74f2a2f237c97be332bd5d579a39c1
        GIT_PROGRESS ON
        SOURCE_DIR "${DEPS_DIR}/toolchain"
        BINARY_DIR "${DEPS_DIR}/toolchain-src-cmake"
        SUBBUILD_DIR "${DEPS_DIR}/toolchain-src-cmake"
    )

    include("${DEPS_DIR}/toolchain/cmake/set_toolchain.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" PARENT_SCOPE)

    unset(MADRONA_USE_TOOLCHAIN CACHE)
endfunction()

if (NOT BRT_USE_BUNDLED_TOOLCHAIN)
  brt_setup_toolchain()
  unset(brt_setup_toolchain)
endif()
