#include "llv.h"

int	lv_lstsize(t_list *lst)
{
	int	l;

	l = 0;
	if (!lst)
		return (0);
	while (lst)
	{
		lst = lst->next;
		l++;
	}
	return (l);
}
