#pragma once

#include "macros.hpp"

namespace brt {

[[noreturn]] void fatal(const char *file, int line,
    const char *funcname, const char *fmt, ...);

BRT_ALWAYS_INLINE inline void checkAssert(bool cond, const char *str,
                                           const char *file, int line);
[[noreturn]] void failAssert(const char *str, const char *file, int line);

void debuggerBreakPoint();

}

#if __cplusplus >= 202002L
#define FATAL(fmt, ...) ::brt::fatal(__FILE__, __LINE__,\
    BRT_COMPILER_FUNCTION_NAME, fmt __VA_OPT__(,) __VA_ARGS__ )
#else
#define FATAL(fmt, ...) ::brt::fatal(__FILE__, __LINE__,\
    BRT_COMPILER_FUNCTION_NAME, fmt ##__VA_ARGS__ )
#endif


#define chk(x) ::brt::checkAssert(x, #x, __FILE__, __LINE__)


#include "err.inl"
