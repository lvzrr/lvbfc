#include "cstr.h"

int	lv_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return (c - ('A' - 'a'));
	return (c);
}
