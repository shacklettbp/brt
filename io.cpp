#include <brt/io.hpp>

#include <brt/err.hpp>

#include <fstream>

namespace madrona {

char * readBinaryFile(const char *path,
                      size_t buffer_alignment,
                      size_t *out_num_bytes)
{
  (void)buffer_alignment;

  // FIXME: look into platform specific alternatives for better
  // errors

  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    return nullptr;
  }

  size_t num_bytes = file.tellg();
  file.seekg(std::ios::beg);

  if (buffer_alignment < sizeof(void *)) {
    buffer_alignment = sizeof(void *);
  }

  size_t alloc_size = utils::roundUpPow2(num_bytes, buffer_alignment);

  char *data = (char *)malloc(alloc_size);
  file.read(data, num_bytes);
  if (file.fail()) {
    rawDeallocAligned(data);
    return nullptr;
  }

  *out_num_bytes = num_bytes;
  return data;
}

}
