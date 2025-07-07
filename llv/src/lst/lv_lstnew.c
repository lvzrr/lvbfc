#include "llv.h"

t_list	*lv_lstnew(void *content)
{
	t_list	*newl;

	newl = (t_list *)lv_alloc(sizeof(t_list));
	if (!newl)
		return (NULL);
	newl->content = content;
	newl->next = NULL;
	return (newl);
}
