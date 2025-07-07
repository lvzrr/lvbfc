#include "mem.h"

lv_INLINE inline t_u128	__populate(t_u8 y)
{
	t_u128	x;

	x = (t_u128)y;
	x |= x << 8;
	x |= x << 16;
	x |= x << 32;
	x |= x << 64;
	return (x);
}
