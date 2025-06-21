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

void emit(t_vec *v, bool w, size_t s)
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
		"uint8_t arr[%lu] = {0};"
		"uint8_t *buf = &(arr[0]);", s);
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
				if (w)
					fprintf(f, "buf = arr + ((buf - arr + %lu) %% %lu);", x.len, s);
				else
					fprintf(f, "buf += %lu;", x.len);
				break;

			case '<':
				if (w)
					fprintf(f, "buf = arr + ((buf - arr + %lu - %lu) %% %lu);", s, x.len, s);
				else
					fprintf(f, "buf -= %lu;", x.len);
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


void	emit_heap(t_vec *v)
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
		"#define GROW_BUF(n) \\\n"
		"do { \\\n"
		"    size_t offset = buf - safeg; \\\n"
		"    if (offset + (n) >= size) { \\\n"
		"        size_t new_size = (offset + (n)) * 2; \\\n"
		"        uint8_t *new_buf = realloc(safeg, new_size); \\\n"
		"        if (!new_buf) { perror(\"realloc failed\"); exit(EXIT_FAILURE); } \\\n"
		"        safeg = new_buf; \\\n"
		"        buf = safeg + offset; \\\n"
		"        __builtin_memset(safeg + size, 0, new_size - size); \\\n"
		"        size = new_size; \\\n"
		"    } \\\n"
		"} while (0)\n\n"
		"int main(void) {"
		"uint8_t *buf = aligned_alloc(512, 65536);"
		"if (!buf) { perror(\"unable to allocate space\"); return EXIT_FAILURE; }"
		"size_t size = 65536;"
		"__builtin_memset(buf, 0, 65536);"
		"uint8_t *safeg = buf;");
	size_t i = 0;
	while (i < v->size)
	{
		t_tokenseq x = *((t_tokenseq *)lv_vec_get_mut(v, i));
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
					"/* ->+< */ GROW_BUF(%lu); "
					"*(buf + %lu) += *buf * %lu; "
					"*buf = 0;",
					offset, offset, scale);
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
			case '>': fprintf(f, "GROW_BUF(%lu);buf += %lu;", x.len, x.len); break;
			case '<':
			fprintf(f,
				"if ((size_t)(buf - safeg) < %lu) { fprintf(stderr, \"underflow at runtime\\n\"); free(safeg);exit(1); } buf -= %lu;",
				x.len, x.len);
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
					fprintf(f, "/* Z */ __builtin_memset(buf, 0, %lu);", x.len);
				break;
			default: break;
		}
		i++;
	}

	fprintf(f,
		"free(safeg);"
		"return 0;"
		"}");

	fclose(f);
	lv_vec_free(v);
}
