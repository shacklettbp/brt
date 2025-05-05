#pragma once

#include <stdint.h>

namespace brt {

constexpr uint32_t compileHash(
    char const * str, unsigned long size)
{
  return ((size ? compileHash(str, size - 1) :
        2166136261u) ^ str[size]) * 16777619u;
}

struct StringID {
  const char *ptr;
  uint32_t hash;
};

inline constexpr StringID operator""_hash(
    const char *str, unsigned long size)
{
  return {
    str,
    compileHash(str, size)
  };
}

}
