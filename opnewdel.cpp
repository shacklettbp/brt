#include "macros.hpp"
#include "err.hpp"

#include <cstdlib>
#include <cstdio>
#include <new>

namespace brt {
namespace {

inline void * osAlloc(size_t num_bytes, size_t alignment)
{
  void *ptr;
  if (alignment <= alignof(void *)) {
    ptr = std::malloc(num_bytes);
  } else {
#if defined(_LIBCPP_VERSION)
    ptr = std::aligned_alloc(alignment, num_bytes);
#elif defined(BRT_CXX_MSVC)
    ptr = _aligned_malloc(num_bytes, alignment);
#else
    STATIC_UNIMPLEMENTED();
#endif
  }

  if (ptr == nullptr) [[unlikely]] {
    FATAL("OOM: %lu\n", num_bytes);
  }

  return ptr;
}

inline void osDealloc(void *ptr, size_t alignment)
{
#if defined(_LIBCPP_VERSION)
  (void)alignment;
  std::free(ptr);
#elif defined(BRT_CXX_MSVC)
  if (alignment <= alignof(void *)) {
    std::free(ptr);
  } else {
    _aligned_free(ptr);
  }
#else
  STATIC_UNIMPLEMENTED();
#endif
}

}
}

// brt-libcxx is compiled without operator new and delete,
// because libc++'s static hermetic mode marks operator new and delete
// as hidden symbols. Unfortunately, this breaks ASAN's (and our own) ability
// to export operator new and operator delete outside of the shared library
// executable. Therefore we disable operator new and delete in libcxx and
// libcxxabi and must provide them here.

// Unaligned versions

#ifdef BRT_WINDOWS
#define BRT_NEWDEL_VIS
#else
#define BRT_NEWDEL_VIS BRT_EXPORT
#endif

BRT_NEWDEL_VIS void * operator new(size_t num_bytes)
{
  return ::brt::osAlloc(num_bytes, alignof(void *));
}

BRT_NEWDEL_VIS void operator delete(void *ptr) noexcept
{
 ::brt::osDealloc(ptr, alignof(void *));
}

BRT_NEWDEL_VIS void * operator new(
    size_t num_bytes, const std::nothrow_t &) noexcept
{
 return ::brt::osAlloc(num_bytes, alignof(void *));
}

BRT_NEWDEL_VIS void operator delete(
    void *ptr, const std::nothrow_t &) noexcept
{
  ::brt::osDealloc(ptr, alignof(void *));
}

BRT_NEWDEL_VIS void * operator new[](size_t num_bytes)
{
  return ::brt::osAlloc(num_bytes, alignof(void *));
}

BRT_NEWDEL_VIS void operator delete[](void *ptr) noexcept
{
  ::brt::osDealloc(ptr, alignof(void *));
}

BRT_NEWDEL_VIS void * operator new[](
    size_t num_bytes, const std::nothrow_t &) noexcept
{
  return ::brt::osAlloc(num_bytes, alignof(void *));
}

BRT_NEWDEL_VIS void operator delete[](
    void *ptr, const std::nothrow_t &) noexcept
{
  ::brt::osDealloc(ptr, alignof(void *));
}

// Aligned versions

BRT_NEWDEL_VIS void * operator new(size_t num_bytes, std::align_val_t al)
{
  return ::brt::osAlloc(num_bytes, (size_t)al);
}

BRT_NEWDEL_VIS void operator delete(void *ptr, std::align_val_t al) noexcept
{
  ::brt::osDealloc(ptr, (size_t)al);
}

BRT_NEWDEL_VIS void * operator new(
    size_t num_bytes, std::align_val_t al, const std::nothrow_t &) noexcept
{
  return ::brt::osAlloc(num_bytes, (size_t)al);
}

BRT_NEWDEL_VIS void operator delete(
    void *ptr, std::align_val_t al, const std::nothrow_t &) noexcept
{
  ::brt::osDealloc(ptr, (size_t)al);
}

BRT_NEWDEL_VIS void * operator new[](size_t num_bytes, std::align_val_t al)
{
  return ::brt::osAlloc(num_bytes, (size_t)al);
}

BRT_NEWDEL_VIS void operator delete[](void *ptr, std::align_val_t al) noexcept
{
  ::brt::osDealloc(ptr, (size_t)al);
}

BRT_NEWDEL_VIS void * operator new[](
    size_t num_bytes, std::align_val_t al, const std::nothrow_t &) noexcept
{
  return ::brt::osAlloc(num_bytes, (size_t)al);
}

BRT_NEWDEL_VIS void operator delete[](
    void *ptr, std::align_val_t al, const std::nothrow_t &) noexcept
{
  ::brt::osDealloc(ptr, (size_t)al);
}
