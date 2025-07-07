#include "ctype.h"

t_u8	lv_isspace(int c)
{
	return (c == 0x20 || (c >= 0x07 && c <= 0x0D));
}
