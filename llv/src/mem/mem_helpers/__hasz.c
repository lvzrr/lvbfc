#include "mem.h"

lv_INLINE_HOT inline int	__hasz64(t_u64 x)
{
	return (((x) -LONES_64) & (~x) & HIGHS_64);
}

lv_INLINE_HOT inline int	__hasz128(t_u128 x)
{
	t_u64	low_part;
	t_u64	high_part;

	high_part = (t_u64)(x >> 64);
	low_part = (t_u64)x;
	return (__hasz64(low_part) || __hasz64(high_part));
}
