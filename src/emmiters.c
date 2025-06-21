#include "lvbfc.h"

bool	match_expr(t_vec *v, size_t start, const char *pattern)
{
	size_t len = strlen(pattern);
	if (v->size < start + len) return false;

	for (size_t i = 0; i < len; i++)
	{
		const t_tokenseq *ts = lv_vec_get(v, start + i);
		if (!ts || ts->op != pattern[i]) return false;
	}
	return true;
}

void emit(t_vec *v)
{
#ifdef __WIN32
	FILE *f = fopen("C:"SEP"tmp"SEP"bf.c", "w");
# else
	FILE *f = fopen(SEP"tmp"SEP"bf.c", "w");
#endif
	if (!f)
	{
		fprintf(stderr, "\033[1;31mUnable to create .c file\033[0m\n");
		lv_vec_free(v);
		exit(EXIT_FAILURE);
	}
	fprintf(f,
		"#include <stdio.h>\n"
		"#include <string.h>\n"
		"#include <stdint.h>\n"
		"#include <stdlib.h>\n\n"
		"int main(void) {"
		"uint8_t arr[65536] = {0};"
		"uint8_t *buf = &(arr[0]);");
	size_t i = 0;
	while (i < v->size)
	{
		t_tokenseq x = *((t_tokenseq *)lv_vec_get(v, i));
		if (match_expr(v, i, "[-]")) {
			const t_tokenseq *c = lv_vec_get(v, i + 1);
			fprintf(f,
				"/* [-] */__builtin_memset(buf, 0, %lu);",
				c->len);
				i += 3;
			continue;
		}
		if (match_expr(v, i, "->+<")) {

			const t_tokenseq *a = lv_vec_get(v, i + 1);
			const t_tokenseq *c = lv_vec_get(v, i + 2);
			const t_tokenseq *d = lv_vec_get(v, i + 3);
			if (a->len == c->len && c->len == d->len)
			{
				size_t offset = d->len;
				size_t scale = c->len;
				fprintf(f,
					"/* ->+< */"
					"*(buf + %lu) += *buf * %lu; "
					"*buf = 0;",
					offset, scale);
				i += 4;
			continue;
			}
		}
		if (match_expr(v, i, "Z+"))
		{
			const t_tokenseq *x = lv_vec_get(v, i + 1);
			fprintf(f, " /* Z+ */ *buf = %lu;", x->len);
			i += 2;
			continue;
		}
		if (match_expr(v, i, "Z-"))
		{
			const t_tokenseq *x = lv_vec_get(v, i + 1);
			fprintf(f, " /* Z- */ *buf = *buf - %lu;", 256 - x->len);
			i += 2;
			continue;
		}
		switch (x.op)
		{
			case '>':
				fprintf(f, "buf = arr + ((buf - arr + %lu) %% 65536);", x.len);
				break;

			case '<':
				fprintf(f, "buf = arr + ((buf - arr + 65536 - %lu) %% 65536);", x.len);
				break;
			case '+': fprintf(f, "*buf += %lu;", x.len); break;
			case '-': fprintf(f, "*buf -= %lu;", x.len); break;
			case '.': fprintf(f, "fputc(*buf, stdout);"); break;
			case ',': 
				for (size_t j = 0; j < x.len; j++)
					fprintf(f, "*buf = getchar();");
				break;
			case '[':
				for (size_t j = 0; j < x.len; j++)
					fprintf(f, "while (*buf) {");
				break;

			case ']':
				for (size_t j = 0; j < x.len; j++)
					fprintf(f, "}");
				break;
			case 'Z':
					fprintf(f, "__builtin_memset(buf, 0, %lu);", x.len);
				break;
			default: break;
		}
		i++;
	}
	fprintf(f,
		"return 0;"
		"}\n");

	fclose(f);
	lv_vec_free(v);
}
