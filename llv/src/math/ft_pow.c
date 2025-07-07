#include "math.h"

t_u8	lv_pow_u8(t_u8 x, t_u8 n)
{
	t_u8	res;

	res = 1;
	while (n)
	{
		if (n & 1)
			res *= x;
		x *= x;
		n >>= 1;
	}
	return (res);
}

t_u32	lv_pow_u32(t_u32 x, t_u32 n)
{
	t_u32	res;

	res = 1;
	while (n)
	{
		if (n & 1)
			res *= x;
		x *= x;
		n >>= 1;
	}
	return (res);
}

t_u64	lv_pow_u64(t_u64 x, t_u64 n)
{
	t_u64	res;

	res = 1;
	while (n)
	{
		if (n & 1)
			res *= x;
		x *= x;
		n >>= 1;
	}
	return (res);
}

t_u128	lv_pow_u128(t_u128 x, t_u128 n)
{
	t_u128	res;

	res = 1;
	while (n)
	{
		if (n & 1)
			res *= x;
		x *= x;
		n >>= 1;
	}
	return (res);
}
