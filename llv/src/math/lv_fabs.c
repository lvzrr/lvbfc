#include "math.h"

double	lv_fabs(double x)
{
	t_fp	u;

	u = (t_fp){x};
	u.i &= -1ULL >> 1;
	return (u.f);
}
