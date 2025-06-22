#include "lvbfc.h"

static inline void	remove_t(t_tokenseq *t)
{
	if (!t || !(t->op))
		return ;
	*t = (t_tokenseq) {0};
}

void	opt_openl(t_vec *v, size_t i)
{

	/* remove loop for just-zeroed cell */

	if (i > 0)
	{
		const t_tokenseq *prev = lv_vec_get(v, i - 1);
		t_tokenseq *curr = lv_vec_get_mut(v, i);
		if (prev && (prev->op == ']' || prev->op == 'Z'))
		{
			while (curr->op != ']')
			{
				remove_t(curr);
				i++;
				curr++;
			}
			curr->len--;
			if (!curr->len) remove_t(curr);
			i++;
			curr++;
		}
	}

	/* Remove all loops @ the beggining */

	if (i == 0)
	{
		t_tokenseq *curr = lv_vec_get_mut(v, i);
		while (curr->op != ']')
		{
			remove_t(curr);
			i++;
			curr++;
		}
		remove_t(curr);
		i++;
		curr++;
	}

	/* [-] -> Z */
	if (i + 2 >= v->size) return;
	t_tokenseq *a = lv_vec_get_mut(v, i);
	t_tokenseq *b = lv_vec_get_mut(v, i + 1);
	t_tokenseq *c = lv_vec_get_mut(v, i + 2);
	if (!a || !b || !c) return;
	if (a->op == '[' && b->op == '-' && c->op == ']' &&
		b->len == 1 && a->len == c->len)
	{
		remove_t(b);
		remove_t(c);
		remove_t(a);
		*a = (t_tokenseq){ .op = 'Z', .len = 1 };
	}
	else if (a->op == '[' && b->op == '-' && c->op == ']' &&
		b->len == 1)
	{
		remove_t(b);
		a->len--;
		if (!a->len) remove_t(a);
		c->len--;
		if (!c->len) remove_t(c);
		if (!a->op)
			*a = (t_tokenseq){ .op = 'Z', .len = 1 };
		else
			*(a + 1) = (t_tokenseq){ .op = 'Z', .len = 1 };
	}

	// /* [->+<] -> 'M' */
	if (i + 5 >= v->size) return;
	t_tokenseq *lop	= lv_vec_get_mut(v, i);
	t_tokenseq *min	= lv_vec_get_mut(v, i + 1);
	t_tokenseq *mr	= lv_vec_get_mut(v, i + 2);
	t_tokenseq *add	= lv_vec_get_mut(v, i + 3);
	t_tokenseq *ml	= lv_vec_get_mut(v, i + 4);
	t_tokenseq *lc	= lv_vec_get_mut(v, i + 5);

	if (lop && lop->op == '['
		&& min	&& min->op == '-'
		&& mr	&& mr->op == '>'
		&& add	&& add->op == '+'
		&& ml	&& ml->op == '<'
		&& lc	&& lc->op == ']'
		&& (mr->len == add->len && add->len == ml->len))
	{
		size_t offset = mr->len;
		remove_t(min);
		remove_t(mr);
		remove_t(add);
		remove_t(ml);
		lop->len--;
		if (!lop->len) remove_t(lop);
		lc->len--;
		if (!lc->len) remove_t(lc);
		// if '[' still exists, we cannot override
		// it as it'd break paren level and control
		// flow
		if (!(lop->op))
			*lop = (t_tokenseq){ .op = 'M', .len = offset };
		else
			*(lop + 1) = (t_tokenseq){ .op = 'M', .len = offset };
	}

	/* [->+>...+<] -> 'C' */
	t_tokenseq *l = lv_vec_get_mut(v, i);
	t_tokenseq *m = lv_vec_get_mut(v, i + 1);
	if (!l || !m || l->op != '[' || m->op != '-') return;
	size_t pos = i + 2;
	size_t dests = 0;
	size_t total_right = 0;
	while (pos + 1 < v->size)
	{
		t_tokenseq *mv = lv_vec_get_mut(v, pos);
		t_tokenseq *add = lv_vec_get_mut(v, pos + 1);
		if (!mv || !add || mv->op != '>' || add->op != '+' || mv->len != add->len) break;
		dests++;
		total_right += mv->len;
		pos += 2;
	}
	t_tokenseq *ret = lv_vec_get_mut(v, pos++);
	t_tokenseq *rc = lv_vec_get_mut(v, pos);
	if (!ret || !rc || ret->op != '<' || rc->op != ']') return;
	if (ret->len != total_right) return;
	for (size_t j = i + 2; j < pos - 1; ++j) remove_t(lv_vec_get_mut(v, j));
	remove_t(m);
	remove_t(ret);
	l->len--;
	if (!l->len) remove_t(l);
	rc->len--;
	if (!rc->len) remove_t(rc);
	if (!l->len)
		*l = (t_tokenseq){ .op = 'C', .len = dests };
	else
		*(l + 1) = (t_tokenseq){ .op = 'C', .len = dests };
}

void	compact_vector(t_vec *v)
{
	t_tokenseq *buf = (t_tokenseq *)v->data;
	size_t j = 0;

	for (size_t i = 0; i < v->size; i++)
	{
		if (buf[i].op == 0 || buf[i].len == 0)
			continue;
		if (j != i)
			buf[j] = buf[i];
		j++;
	}
	v->size = j;
}

void	optimize(t_vec *v, size_t level)
{
	for (size_t pass = 0; pass < level; pass++)
	{
		for (size_t i = 0; i < v->size; i++)
		{
			const t_tokenseq *x = lv_vec_get(v, i);
			if (!x) break; // only returns null on oob, so don't deref
			if (x->op == 0 || x->len == 0) continue;
			if (x->op == '[')
				opt_openl(v, i);
		}
		compact_vector(v);
	}
}
