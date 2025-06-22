#include "lvbfc.h"

char	*read_file(const char *name, bool x)
{
	FILE *f = fopen(name,"rb");
	if (!f)
	{
		fprintf(stderr, "unable to read %s\n", name);
		return (NULL);
	}
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	rewind(f);
	char *src = malloc(fsize + 1);
	if (!src)
	{
		fprintf(stderr, "unable to allocate space for %s\n", name);
		return (NULL);
	}
	int c;
	size_t i = 0;
	while ((c = fgetc(f)) != EOF)
	{
		if (!x && !(c == '>' || c == '<' || c == '+' || c == '-' ||
			c == '.' || c == ',' || c == '[' || c == ']'))
			continue ;
		else if (x && !(c == '>' || c == '<' || c == '+' || c == '-' ||
			c == '.' || c == ',' || c == '[' || c == ']' || c == ';'))
			continue ;
		src[i++] = c;
	}
	src[i] = 0;
	fclose(f);
	return (src);
}

t_vec	lex(const char *src, bool strict, bool dmp)
{
	char		curr = 0;
	char		*start = 0;
	t_tokenseq	x = {0};
	t_tokenseq	*prev = {0};
	t_vec		out = {0};
	const char 		*ogp = src;
	size_t		wc = 0;

	out = lv_vec(100, sizeof(t_tokenseq));
	while (*src)
	{
		curr = *src;
		start = (char *)src;
		while (*src && *src == curr)
			src++;
		x = (t_tokenseq) {.op = curr, .len = (size_t)(src - start)};
		if (out.size > 0)
			prev = lv_vec_peek_last(&out);
		if (prev && prev->op && prev->len)
		{
			if (x.op == ']' && prev->op == '[')
			{
				ssize_t	diff = x.len - prev->len;
				if (!diff)
				{
					lv_vec_pop(&out);
					continue ;
				}
				else if (diff > 0)
				{
					x.len -= prev->len;
					lv_vec_pop(&out);
				}
				else
				{
					prev->len -= x.len;
					continue ;
				}
			}
			else if (((x.op == '+' && prev->op == '-') || (x.op == '-' && prev->op == '+'))
				&& x.len == prev->len)
			{
				lv_vec_pop(&out);
				continue ;
			}
			else if ((x.op == '+' && prev->op == '-') || (x.op == '-' && prev->op == '+')
				|| (x.op == '>' && prev->op == '<') || (x.op == '<' && prev->op == '>'))
			{
				ssize_t	diff = prev->len - x.len;
				if (diff > 0)
					prev->len = prev->len - x.len;
				else
				{
					size_t	pl = prev->len;
					*prev = x;
					prev->len = x.len - pl;
				}
				continue ;
			}
			if (x.op == '[' && *(src) == '-' && *(src + 1) == ']')
			{
				x.len--;
				if (!x.len)
				{
					x.op = 'Z';
					x.len = 1;
					src += 2;
				}
				else
				{
					lv_vec_push(&out, &x, 1);
					x.op = 'Z';
					x.len = 1;
					src += 2;
				}
			}
		}
		lv_vec_push(&out, &x, 1);
	}
	size_t plevel = 0;
	for (size_t i = 0; i < out.size; i++)
	{
		t_tokenseq *x2 = ((t_tokenseq *)lv_vec_get(&out, i));
		if (x2->op == '[')
			plevel += x2->len;
	  	else if (x2->op == ']')
			plevel -= x2->len;
		if (x2->op == '[' && i + 2 < out.size
			&& (x2 + 1)->op != '-' && (x2 + 1)->len == 1 && (x2 + 2)->op == ']')
		{
				if (strict)
				{
					free((char *)ogp);
					THROW_ERR_AT(i, "potential infinite loop [%c] detected in the code (run with --no-strict to ignore)", out, (x2 + 1)->op);
				}
				else if (strict && wc < 5)
			{
				fprintf(stderr, "\033[1;33mWARNING: potential infinite loop [%c] detected @ seq. %lu\033[0m\n", (x2 + 1)->op, i);
				++wc;
			}
		}
		if (x2->op == '[')
		{
			size_t i2 = i + 1;
			bool hasm = false;
			while (i2 < out.size)
			{
				const t_tokenseq *y = lv_vec_get(&out, i2);
				if (y->op == ']')
					break;
				if (y->op == '-')
				{
					hasm = true;
					break;
				}
				i2++;
			}
			if (strict && !hasm)
			{
				free((char *)ogp);
				THROW_ERR_AT(i, "potential infinite loop (no '-') detected (run with --no-strict to ignore)", out);
			}
			else if (!strict && !hasm && wc < 5)
			{
				fprintf(stderr, "\033[1;33mWARNING: potential dangerous loop (no '-' in loop) detected @ seq. %lu\033[0m\n", i);
				++wc;
			}
			if (wc == 5)
			{
				fprintf(stderr, "\033[1;33mExceeded max number warnings\033[0m\n");
				++wc;
			}
		}

		if (dmp)
			printf("op: %c  len: %lu plevel: %ld\n", x2->op, x2->len, plevel);
	}
	if (plevel != 0)
	{
		free((char *)ogp);
		THROW_ERR("unmatched loop guards", out);
	}
	return (free((char *)ogp), out);
}
