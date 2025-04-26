set(BRT_CXX_CLANG FALSE)
set(BRT_CXX_GCC FALSE)
set(BRT_CXX_MSVC FALSE)

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(BRT_CXX_CLANG TRUE)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(BRT_CXX_GCC TRUE)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(BRT_CXX_MSVC TRUE)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (CMAKE_CXX_COMPILER_BRT_CXX_VARIANT STREQUAL "MSVC")
    set(BRT_CXX_MSVC TRUE)
    set(BRT_CXX_CLANG_CL TRUE)
  else ()
    set(BRT_CXX_CLANG TRUE)
  endif ()
endif ()

if (BRT_CXX_MSVC)
  string(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
  string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
else()
  string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
  string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb3")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3")
endif()

add_library(brt-sys-defns INTERFACE)

if (BRT_CXX_CLANG)
  list(APPEND BRT_SYS_DEFNS "BRT_CXX_CLANG=1")
endif()

if (BRT_CXX_GCC)
  list(APPEND BRT_SYS_DEFNS "BRT_CXX_GCC=1")
endif()

if (BRT_CXX_MSVC)
  list(APPEND BRT_SYS_DEFNS "BRT_CXX_MSVC=1")
endif()

if (BRT_CXX_CLANG_CL)
  list(APPEND BRT_SYS_DEFNS "BRT_CXX_CLANG_CL=1")
endif()

if (BRT_OS_LINUX)
  list(APPEND BRT_SYS_DEFNS "BRT_OS_LINUX=1")
endif()

if (BRT_OS_MACOS)
  list(APPEND BRT_SYS_DEFNS "BRT_OS_MACOS=1")
endif()

if (BRT_OS_WINDOWS)
  list(APPEND BRT_SYS_DEFNS "BRT_OS_WINDOWS=1")
endif()

if (BRT_ARCH_X64)
  list(APPEND BRT_SYS_DEFNS "BRT_OS_X64=1")
endif()

if (BRT_ARCH_ARM)
  list(APPEND BRT_SYS_DEFNS "BRT_ARCH_ARM=1")
endif()

target_compile_definitions(brt-sys-defns INTERFACE ${BRT_SYS_DEFNS})

add_library(brt-cxx-flags INTERFACE)
target_link_libraries(brt-cxx-flags INTERFACE
    brt-sys-defns
)

if (BRT_CXX_CLANG OR BRT_CXX_GCC)
  target_compile_options(brt-cxx-flags INTERFACE
    -pedantic -Wall -Wextra
  )

  if (BRT_ARCH_X64 AND BRT_OS_LINUX)
    target_compile_options(brt-cxx-flags INTERFACE
      -march=x86-64-v3
    )
  elseif (BRT_ARCH_ARM AND BRT_OS_MACOS)
    target_compile_options(brt-cxx-flags INTERFACE
      -mcpu=apple-m1
    )
  endif()
elseif (BRT_CXX_MSVC)
  # FIXME: some of these options (/permissive-, /Zc:__cplusplus,
  # /Zc:preprocessor) should just be applied globally to the toolchain
  target_compile_options(brt-cxx-flags INTERFACE
    /Zc:__cplusplus
    /permissive-
    /W4
    /wd4324 # Struct padded for alignas ... yeah that's the point
    /wd4701 # Potentially uninitialized variable. MSVC analysis really sucks on this
    /wd4244 /wd4267 # Should reenable these
  )

  if (NOT BRT_CXX_CLANG_CL)
    target_compile_options(brt-cxx-flags INTERFACE
      /Zc:preprocessor
    )
  endif()
endif()

if (BRT_CXX_GCC)
  target_compile_options(brt-cxx-flags INTERFACE
    -fdiagnostics-color=always  
  )
elseif (BRT_CXX_CLANG)
  target_compile_options(brt-cxx-flags INTERFACE
    -fcolor-diagnostics
  )
endif ()

if (BRT_CXX_CLANG)
  target_compile_options(brt-cxx-flags INTERFACE
    -Wshadow
  )
endif()

add_library(brt-cxx-noexceptrtti INTERFACE)
if (BRT_CXX_GCC OR BRT_CXX_CLANG)
  target_compile_options(brt-cxx-noexceptrtti INTERFACE
    -fno-exceptions -fno-rtti
  )
elseif (BRT_CXX_MSVC)
  target_compile_options(brt-cxx-noexceptrtti INTERFACE
    /GR-
  )
else()
  message(FATAL_ERROR "Unsupported compiler frontend")
endif()
