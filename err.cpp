#include "err.hpp"

#ifndef BRT_IS_GPU
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#endif

#if defined(BRT_OS_LINUX) || defined(BRT_OS_MACOS)
#include "signal.h"
#endif

namespace brt {

void failAssert(const char *str, const char *file, int line)
{
  FATAL("Assert '%s' failed at %s line %d", str, file, line);
}

void fatal(const char *file, int line, const char *funcname,
           const char *fmt, ...)
{
#ifdef BRT_IS_GPU
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  printf("\n");

  __assertfail("Fatal error", file, line, funcname, 1);
  __builtin_unreachable();

#else
  // Use a fixed size buffer for the error message. This sets an upper
  // bound on total memory size, and wastes 4kb on memory, but is very
  // robust to things going horribly wrong elsewhere.
  static std::array<char, 4096> buffer;

  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer.data(), buffer.size(), fmt, args);

  fprintf(stderr, "Fatal error at %s:%d in %s\n", file, line,
          funcname);
  fprintf(stderr, "%s\n", buffer.data());

  fflush(stderr);
  abort();
#endif
}

void debuggerBreakPoint()
{
#if defined(BRT_OS_LINUX) || defined(BRT_OS_MACOS)
  signal(SIGTRAP, SIG_IGN);
  raise(SIGTRAP);
  signal(SIGTRAP, SIG_DFL);
#elif defined(BRT_OS_WINDOWS)
  if (IsDebuggerPresent()) {
    DebugBreak();
  }
#endif
}

}
