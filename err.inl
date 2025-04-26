namespace brt {

void checkAssert(bool cond, const char *str, const char *file, int line)
{
#ifdef BRT_DISABLE_ASSERTS
  (void)cond;
  (void)str;
  (void)file;
  (void)line;
#else
  if (!cond) [[unlikely]] {
    failAssert(str, file, line);
  }
#endif
}

}
