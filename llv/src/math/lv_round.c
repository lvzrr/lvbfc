#include "math.h"

float	lv_floorf(float x)
{
	long long	i;

	i = (long long)x;
	if ((float)i > x)
		return (i - 1);
	return (i);
}

float	lv_ceilf(float x)
{
	long long	i;

	i = (long long)x;
	if ((float)i < x)
		return (i + 1);
	return (i);
}

float	lv_roundf(float x, t_u8 n)
{
	float	p10;

	if (n > 7)
		n = 7;
	p10 = 0.1f;
	while (n--)
		p10 *= 10.0f;
	if (x >= 0)
		return (lv_floorf(x * p10 + 0.5f) / p10);
	else
		return (lv_ceilf(x * p10 - 0.5f) / p10);
}

float	lv_roundff(float x)
{
	if (x >= 0.0f)
		return ((float)((int)(x + 0.5f)));
	else
		return ((float)((int)(x - 0.5f)));
}
