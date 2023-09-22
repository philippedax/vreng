#include "vred.hpp"
#include "safe.hpp"


char* Safe::safe_strdup(char **dest, const char *src)
{
  if (dest == NULL) {
    printf("DANGER: Safe::strdup called with dest == NULL\n");
    return NULL;
  }
  *dest = src != NULL ? strdup(src) : NULL;
  return *dest;
}

char* Safe::strcpy(char *dest, const char *src)
{
  if (src == NULL)
    dest[0] = 0;
  else
    strcpy(dest, src);
  return dest;
}
