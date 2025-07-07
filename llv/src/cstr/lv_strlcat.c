#include "cstr.h"

size_t	lv_strlcat(char *dest, const char *src, size_t n)
{
	size_t	srclen;
	size_t	dstlen;
	size_t	i;

	srclen = lv_strlen(src);
	dstlen = lv_strlen(dest);
	if (n <= dstlen)
		return (srclen + n);
	i = 0;
	while (dstlen + i < n - 1 && src[i])
	{
		dest[dstlen + i] = src[i];
		i++;
	}
	dest[dstlen + i] = 0;
	return (srclen + dstlen);
}
