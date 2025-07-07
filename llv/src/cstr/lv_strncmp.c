#include "cstr.h"

int	lv_strncmp(const char *s1, const char *s2, size_t n)
{
	return (lv_memcmp((void *)s1, (void *)s2, n));
}
