#include "mem.h"

t_u128	__maxu128(t_u128 x, t_u128 y)
{
	if (x > y)
		return (x);
	return (y);
}

t_u64	__maxu64(t_u64 x, t_u64 y)
{
	if (x > y)
		return (x);
	return (y);
}

t_u32	__maxu32(t_u32 x, t_u32 y)
{
	if (x > y)
		return (x);
	return (y);
}

t_u8	__maxu8(t_u8 x, t_u8 y)
{
	if (x > y)
		return (x);
	return (y);
}
