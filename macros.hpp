/*
 * Copyright 2021-2022 Brennan Shacklett and contributors
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#pragma once

#define BRT_STRINGIFY_HELPER(m) #m
#define BRT_STRINGIFY(m) BRT_STRINGIFY_HELPER(m)
#define BRT_MACRO_CONCAT_HELPER(x, y) x##y
#define BRT_MACRO_CONCAT(x, y) BRT_MACRO_CONCAT_HELPER(x, y)

#define BRT_LOC_APPEND(m) m ": " __FILE__ " @ " BRT_STRINGIFY(__LINE__)

#if defined(BRT_CXX_CLANG) or defined(BRT_CXX_GCC) or defined(BRT_IS_GPU)
#define BRT_COMPILER_FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(BRT_CXX_MSVC)
#define BRT_COMPILER_FUNCTION_NAME __FUNCSIG__
#endif

#ifdef BRT_ARCH_X64
#define BRT_CACHE_LINE (64)
#elif defined(BRT_ARCH_ARM) && defined(BRT_OS_MACOS)
#define BRT_CACHE_LINE (128)
#else
#define BRT_CACHE_LINE (64)
#endif

#if defined(BRT_CXX_MSVC)

#define BRT_NO_INLINE __declspec(noinline)
#if defined(BRT_CXX_CLANG_CL)
#define BRT_ALWAYS_INLINE __attribute__((always_inline))
#else
#define BRT_ALWAYS_INLINE [[msvc::forceinline]]
#endif

#elif defined(BRT_CXX_CLANG) || defined(BRT_CXX_GCC) || defined(BRT_IS_GPU)

#define BRT_ALWAYS_INLINE __attribute__((always_inline))
#define BRT_NO_INLINE __attribute__((noinline))

#endif

#if defined(BRT_OS_WINDOWS)
#define BRT_IMPORT __declspec(dllimport)
#define BRT_EXPORT __declspec(dllexport)
#else
#define BRT_IMPORT __attribute__ ((visibility ("default")))
#define BRT_EXPORT __attribute__ ((visibility ("default")))
#endif

#if defined(BRT_CXX_MSVC)
#define BRT_UNREACHABLE() __assume(0)
#else
#define BRT_UNREACHABLE() __builtin_unreachable()
#endif

#if defined(BRT_CXX_CLANG) || defined(BRT_CXX_CLANG_CL)
#define BRT_LFBOUND [[clang::lifetimebound]]
#elif defined(BRT_CXX_MSVC)
#define BRT_LFBOUND [[msvc::lifetimebound]]
#else
#define BRT_LFBOUND
#endif

#define BRT_UNIMPLEMENTED() \
    static_assert(false, "Unimplemented")

#if defined(BRT_IS_GPU) || defined(BRT_CXX_CLANG)
#define BRT_UNROLL _Pragma("unroll")
#else
#define BRT_UNROLL
#endif
