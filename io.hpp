#pragma once

#include <brt/types.hpp>
#include <brt/span.hpp>

namespace brt {

char * readBinaryFile(const char *path,
                      size_t buffer_alignment,
                      size_t *out_num_bytes);

}
