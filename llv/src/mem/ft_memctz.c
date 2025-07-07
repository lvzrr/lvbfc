#include "mem.h"

lv_INLINE_HOT inline t_u8	lv_memctz_u32(t_u32 x)
{
	t_u8	c;

	c = 0;
	if (x == 0)
		return (32);
	while (!(x & 0xF))
	{
		c += 4;
		x >>= 4;
	}
	while (!(x & 0x3))
	{
		c += 2;
		x >>= 2;
	}
	while (!(x & 1))
	{
		c++;
		x >>= 1;
	}
	return (c);
}

lv_INLINE_HOT inline t_u8	lv_memctz_u64(t_u64 x)
{
	t_u8	c;

	c = 0;
	if (x == 0)
		return (32);
	while (!(x & 0xF))
	{
		c += 4;
		x >>= 4;
	}
	while (!(x & 0x3))
	{
		c += 2;
		x >>= 2;
	}
	while (!(x & 1))
	{
		c++;
		x >>= 1;
	}
	return (c);
}

lv_INLINE_HOT inline t_u8	lv_memctz_u128(t_u128 x)
{
	t_u8	c;

	c = 0;
	if (x == 0)
		return (32);
	while (!(x & 0xF))
	{
		c += 4;
		x >>= 4;
	}
	while (!(x & 0x3))
	{
		c += 2;
		x >>= 2;
	}
	while (!(x & 1))
	{
		c++;
		x >>= 1;
	}
	return (c);
}
