list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (NOT PROJECT_NAME)
  cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
  cmake_policy(VERSION 3.26)
  
  list(APPEND LANGS C CXX)
  if (APPLE)
    list(APPEND LANGS OBJC)
  endif()

  include(toolchain)

  project(brt LANGUAGES ${LANGS})

  include(sys-detect)
  include(cxx-setup)
endif()

if (BRT_USE_BUNDLED_TOOLCHAIN)
  add_subdirectory(deps/toolchain)
endif()

add_library(brt-hdrs INTERFACE)
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH PARENT_SRC_DIR)
target_include_directories(brt-hdrs INTERFACE ${PARENT_SRC_DIR})

add_library(brt-common-flags INTERFACE)
target_link_libraries(brt-common-flags INTERFACE
  brt-hdrs
  brt-cxx-flags
  brt-cxx-noexceptrtti
)

add_library(brt-libcxx INTERFACE)
if (BRT_USE_BUNDLED_TOOLCHAIN)
  target_link_libraries(brt-libcxx INTERFACE madrona_libcxx)
endif ()

add_library(brt STATIC
  macros.hpp span.hpp types.hpp
  utils.hpp utils.inl utils.cpp
  rand.hpp rand.inl rand.cpp
  err.hpp err.inl err.cpp
  math.hpp math.inl
  stack_alloc.hpp stack_alloc.inl stack_alloc.cpp
  opnewdel.cpp
)

target_link_libraries(brt
  PUBLIC
    brt-common-flags
    brt-libcxx
)
