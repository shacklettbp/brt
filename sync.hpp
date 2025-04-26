/*
 * Copyright 2021-2022 Brennan Shacklett and contributors
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#pragma once

#include "macros.hpp"
#include "types.hpp"

#include <atomic>

#ifndef BRT_IS_GPU
#include <version>
#endif

#if defined(__has_feature)
#if __has_feature(thread_sanitizer)
#define BRT_TSAN_ENABLED (1)
#endif
#endif

#ifdef BRT_TSAN_ENABLED
extern "C" {
extern void __tsan_acquire(void *addr);
extern void __tsan_release(void *addr);
}

#define BRT_TSAN_ACQUIRE(addr) __tsan_acquire(addr)
#define BRT_TSAN_RELEASE(addr) __tsan_release(addr)
#define BRT_TSAN_DISABLE __attribute__((no_sanitize_thread))

#else
#define BRT_TSAN_ACQUIRE(addr)
#define BRT_TSAN_RELEASE(addr)
#define BRT_TSAN_DISABLE
#endif

namespace brt {

namespace sync {
using memory_order = std::memory_order;
inline constexpr memory_order relaxed = std::memory_order_relaxed;
inline constexpr memory_order acquire = std::memory_order_acquire;
inline constexpr memory_order release = std::memory_order_release;
inline constexpr memory_order acq_rel = std::memory_order_acq_rel;
inline constexpr memory_order seq_cst = std::memory_order_seq_cst;
}

template <typename T>
class Atomic {
public:
  constexpr Atomic(T v)
    : impl_(v)
  {
    static_assert(decltype(impl_)::is_always_lock_free);
  }

  template <sync::memory_order order>
    inline T load() const
    {
      return impl_.load(order);
    }

  inline T load_relaxed() const
  {
    return impl_.load(sync::relaxed);
  }

  inline T load_acquire() const
  {
    return impl_.load(sync::acquire);
  }

  template <sync::memory_order order>
  inline void store(T v)
  {
    impl_.store(v, order);
  }

  inline void store_relaxed(T v)
  {
    impl_.store(v, sync::relaxed);
  }

  inline void store_release(T v)
  {
    impl_.store(v, sync::release);
  }

  template <sync::memory_order order>
  inline T exchange(T v)
  {
    return impl_.exchange(v, order);
  }

  template <sync::memory_order success_order,
  sync::memory_order failure_order>
  inline bool compare_exchange_weak(T &expected, T desired)
  {
    return impl_.compare_exchange_weak(expected, desired,
                                       success_order, failure_order);
  }

  template <sync::memory_order success_order,
  sync::memory_order failure_order>
  inline bool compare_exchange_strong(T &expected, T desired)
  {
    return impl_.compare_exchange_strong(expected, desired,
                                         success_order, failure_order);
  }

  template <sync::memory_order order>
  T fetch_add(T v) requires (std::is_integral_v<T>)
  {
    return impl_.fetch_add(v, order);
  }

  inline T fetch_add_relaxed(T v)
  {
    return impl_.fetch_add(v, sync::relaxed);
  }

  inline T fetch_add_acquire(T v)
  {
    return impl_.fetch_add(v, sync::acquire);
  }

  inline T fetch_add_release(T v)
  {
    return impl_.fetch_add(v, sync::release);
  }

  inline T fetch_add_acq_rel(T v)
  {
    return impl_.fetch_add(v, sync::acq_rel);
  }

  template <sync::memory_order order>
  T fetch_sub(T v)
  {
    return impl_.fetch_sub(v, order);
  }

  inline T fetch_sub_relaxed(T v)
  {
    return impl_.fetch_sub(v, sync::relaxed);
  }

  inline T fetch_sub_acquire(T v)
  {
    return impl_.fetch_sub(v, sync::acquire);
  }

  inline T fetch_sub_release(T v)
  {
    return impl_.fetch_sub(v, sync::release);
  }

  inline T fetch_sub_acq_rel(T v)
  {
    return impl_.fetch_sub(v, sync::acq_rel);
  }

  template <sync::memory_order order>
  inline void wait(T v)
  {
    return impl_.wait(v, order);
  }

  inline void notify_one()
  {
    return impl_.notify_one();
  }

  inline void notify_all()
  {
    return impl_.notify_all();
  }

private:
#ifdef BRT_IS_GPU
  cuda::atomic<T, cuda::thread_scope_device> impl_;
#else
  std::atomic<T> impl_;
#endif
};

using AtomicU32 = Atomic<uint32_t>;
using AtomicI32 = Atomic<int32_t>;
using AtomicU64 = Atomic<uint64_t>;
using AtomicI64 = Atomic<int64_t>;
using AtomicFloat = Atomic<float>;

#if defined(__cpp_lib_atomic_ref) or defined(BRT_IS_GPU)
#define BRT_STD_ATOMIC_REF
#endif
template <typename T>
class AtomicRef {
public:
  AtomicRef(T &ref)
#ifndef BRT_STD_ATOMIC_REF
    : addr_(&ref)
#else
    : ref_(ref)
#endif
    {}

  template <sync::memory_order order>
  inline T load() const
  {
#ifndef BRT_STD_ATOMIC_REF
    return __builtin_bit_cast(T,
      __atomic_load_n((ValueT *)addr_, OrderMap<order>::builtin));
#else
    return ref_.load(order);
#endif
  }

  inline T load_relaxed() const
  {
    return load<sync::relaxed>();
  }

  inline T load_acquire() const
  {
    return load<sync::acquire>();
  }

  template <sync::memory_order order>
  inline void store(T v)
  {
#ifndef BRT_STD_ATOMIC_REF
    __atomic_store_n((ValueT *)addr_, __builtin_bit_cast(ValueT, v),
                     OrderMap<order>::builtin);
#else
    ref_.store(v, order);
#endif
  }

  inline void store_relaxed(T v)
  {
    store<sync::relaxed>(v);
  }

  inline void store_release(T v)
  {
    store<sync::release>(v);
  }

  template <sync::memory_order order>
  inline T exchange(T v)
  {
#ifndef BRT_STD_ATOMIC_REF
    return __atomic_exchange_n(
      (ValueT *)addr_,
      __builtin_bit_cast(ValueT, v),
      OrderMap<order>::builtin);
#else
    return ref_.exchange(v, order);
#endif
  }

  template <sync::memory_order success_order,
            sync::memory_order failure_order>
  inline bool compare_exchange_weak(T &expected, T desired)
  {
#ifndef BRT_STD_ATOMIC_REF
    return __atomic_compare_exchange_n(
      (ValueT *)addr_, (ValueT *)&expected,
      __builtin_bit_cast(ValueT, desired), true,
      OrderMap<success_order>::builtin,
      OrderMap<failure_order>::builtin);
#else
    return ref_.compare_exchange_weak(expected, desired,
                                      success_order, failure_order);
#endif
  }

  template <sync::memory_order order>
  inline T fetch_add(T v)
  {
#ifndef BRT_STD_ATOMIC_REF
    return __atomic_fetch_add(addr_, v, OrderMap<order>::builtin);
#else
    return ref_.fetch_add(v, order);
#endif
  }

  inline T fetch_add_relaxed(T v)
  {
    return fetch_add<sync::relaxed>(v);
  }

  inline T fetch_add_acquire(T v)
  {
    return fetch_add<sync::acquire>(v);
  }

  inline T fetch_add_release(T v)
  {
    return fetch_add<sync::release>(v);
  }

  inline T fetch_add_acq_rel(T v)
  {
    return fetch_add<sync::acq_rel>(v);
  }

  template <sync::memory_order order>
  inline T fetch_sub(T v)
  {
#ifndef BRT_STD_ATOMIC_REF
    return __atomic_fetch_sub(addr_, v, OrderMap<order>::builtin);
#else
    return ref_.fetch_sub(v, order);
#endif
  }

  inline T fetch_sub_relaxed(T v)
  {
    return fetch_sub<sync::relaxed>(v);
  }

  inline T fetch_sub_acquire(T v)
  {
    return fetch_sub<sync::acquire>(v);
  }

  inline T fetch_sub_release(T v)
  {
    return fetch_sub<sync::release>(v);
  }

  inline T fetch_sub_acq_rel(T v)
  {
    return fetch_sub<sync::acq_rel>(v);
  }

  template <sync::memory_order order>
  inline T fetch_or(T v)
  {
#ifndef BRT_STD_ATOMIC_REF
    return __atomic_fetch_or(addr_, v, OrderMap<order>::builtin);
#else
    return ref_.fetch_or(v, order);
#endif
  }

private:
  static_assert(sizeof(T) == 4 || sizeof(T) == 8);
  static_assert(std::is_trivially_copyable_v<T>);

#ifndef BRT_STD_ATOMIC_REF
  template <size_t t_size> struct ValueType;
  template <> struct ValueType<8> {
    using type = uint64_t;
  };
  template <> struct ValueType<4> {
    using type = uint32_t;
  };

  template <sync::memory_order order> struct OrderMap;
  template <> struct OrderMap<sync::relaxed> {
    static inline constexpr int builtin = __ATOMIC_RELAXED;
  };

  template <> struct OrderMap<sync::acquire> {
    static inline constexpr int builtin = __ATOMIC_ACQUIRE;
  };

  template <> struct OrderMap<sync::release> {
    static inline constexpr int builtin = __ATOMIC_RELEASE;
  };

  template <> struct OrderMap<sync::acq_rel> {
    static inline constexpr int builtin = __ATOMIC_ACQ_REL;
  };

  template <> struct OrderMap<sync::seq_cst> {
    static inline constexpr int builtin = __ATOMIC_SEQ_CST;
  };

  using ValueT = typename ValueType<sizeof(T)>::type;
  T *addr_;
#else
#ifdef BRT_IS_GPU
  cuda::atomic_ref<T, cuda::thread_scope_device> ref_;
#else
  std::atomic_ref<T> ref_;
#endif
  static_assert(decltype(ref_)::is_always_lock_free);
#endif
};
#undef BRT_STD_ATOMIC_REF

using AtomicI32Ref = AtomicRef<int32_t>;
using AtomicU32Ref = AtomicRef<uint32_t>;
using AtomicI64Ref = AtomicRef<int64_t>;
using AtomicU64Ref = AtomicRef<uint64_t>;
using AtomicFloatRef = AtomicRef<float>;

inline void spinLock(u32 *v)
{
  AtomicU32Ref atomic(*v);
  while (atomic.exchange<sync::acquire>(1) == 1) {
    while (atomic.load<sync::relaxed>() == 1) {}
  }
}

inline bool spinTryLock(u32 *v)
{
  AtomicU32Ref atomic(*v);

  int32_t is_locked = atomic.load<sync::relaxed>();
  if (is_locked == 1) return false;

  int32_t prev_locked = atomic.exchange<sync::relaxed>(1);

  if (prev_locked) {
    return false;
  }

  std::atomic_thread_fence(sync::acquire);
  BRT_TSAN_ACQUIRE(v);

  return true;
}

inline void spinUnlock(u32 *v)
{
  AtomicU32Ref atomic(*v);
  atomic.store<sync::release>(0);
}

}
