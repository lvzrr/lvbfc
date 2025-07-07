#include "math.h"

float	lv_q_sqrt(float number)
{
	long		i;
	float		x2;
	float		y;
	float		threehalfs;

	if (number < 0)
		return (-1);
	threehalfs = 1.5F;
	x2 = number * 0.5F;
	y = number;
	lv_memcpy(&i, &y, sizeof(float));
	i = 0x5f3759df - (i >> 1);
	lv_memcpy(&y, &i, sizeof(float));
	y = y * (threehalfs - (x2 * y * y));
	y = y * (threehalfs - (x2 * y * y));
	return (number * y);
}

float	lv_q_sqrt_round(float number, t_u8 n)
{
	return (lv_roundf(lv_q_sqrt(number), n));
}

float	lv_q_sqrt_fround(float number)
{
	return (lv_roundff(lv_q_sqrt(number)));
}
