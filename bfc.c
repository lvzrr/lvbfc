#include <stdio.h>
#include <stdlib.h>
#include "llv/vec.h"
#include <string.h>


#define THROW_ERR(msg, vec, ...) \
	do { \
		fprintf(stderr, "\033[1;31m" msg "\033[0m\n", ##__VA_ARGS__); \
		lv_vec_free(&vec); \
		exit(EXIT_FAILURE); \
	} while (0)

#define THROW_ERR_AT(i, msg, vec, ...) \
	do { \
		fprintf(stderr, "\033[1;31merror in %lu: " msg "\033[0m\n", i, ##__VA_ARGS__); \
		lv_vec_free(&vec); \
		exit(EXIT_FAILURE); \
	} while (0)

#ifdef __WIN32
# define SEP "\\"
# else
# define SEP "/"
#endif

typedef	struct tokenseq
{
	char 	op;
	size_t	len;
}	t_tokenseq;

char	*read_file(const char *name)
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
		if (!(c == '>' || c == '<' || c == '+' || c == '-' ||
			c == '.' || c == ',' || c == '[' || c == ']'))
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
					THROW_ERR_AT(i, "potential infinite loop [%c] detected in the code", out, (x2 + 1)->op);
				}
				else
					fprintf(stderr, "\033[1;33mWARNING: potential infinite loop [%c] detected @ seq. %lu\033[0m\n", (x2 + 1)->op, i);
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
				THROW_ERR_AT(i, "potential infinite loop (no '-') detected", out);
			}
			else if (!strict && !hasm)
				fprintf(stderr, "\033[1;33mWARNING: potential dangerous loop (no '-' in loop) detected @ seq. %lu\033[0m\n", i);
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
		t_tokenseq x = *((t_tokenseq *)lv_vec_get(v, i));
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
					fprintf(f, "__builtin_memset(buf, 0, %lu);", x.len);
				break;
			default: break;
		}
		i++;
	}
	fprintf(f,
		"free(safeg);"
		"return 0;"
		"}\n");

	fclose(f);
	lv_vec_free(v);
}

bool match_expr(t_vec *v, size_t start, const char *pattern) {
	size_t len = strlen(pattern);
	if (v->size < start + len) return false;

	for (size_t i = 0; i < len; i++) {
		const t_tokenseq *ts = lv_vec_get(v, start + i);
		if (!ts || ts->op != pattern[i]) return false;
	}
	return true;
}

void emit_opt(t_vec *v)
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

void compile_c(char *name)
{
	char cmd[256] = {0};
	bool ok = false;
	snprintf(cmd, sizeof(cmd),
		"cc -Wall -Wextra -Werror -O3 -march=native -msse -msse2 -mavx -mavx2 "SEP"tmp"SEP"bf.c -o %s", name);
	if (system(cmd) == 0)
		ok = true;
	else
	{
		snprintf(cmd, sizeof(cmd),
			"clang -Wall -Wextra -Werror -O3 -march=native -msse -msse2 -mavx -mavx2 "SEP"tmp"SEP"bf.c -o %s", name);
		if (system(cmd) == 0)
			ok = true;
	}
	if (!ok)
	{
		fprintf(stderr, "\033[1;31mNo working C compiler detected.\033[0m\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	char *filename = NULL;
	char *outname = "bfout";
	bool strict = true;
	bool opts = false;
	bool shstrm = false;

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--no-strict") == 0) 
			strict = false;
		else if (strcmp(argv[i], "--dmp-tok") == 0) 
			shstrm = true;
		else if (strcmp(argv[i], "--opt") == 0) 
			opts = true;
		else if (strncmp(argv[i], "--", 2) == 0) 
		{
			fprintf(stderr, "Unknown flag: %s\n", argv[i]);
			return EXIT_FAILURE;
		}
		else if (!filename) 
			filename = argv[i];
		else
			outname = argv[i];
	}
	if (!filename)
	{
		fprintf(stderr, "no file to compile\n");
		return EXIT_FAILURE;
	}
	char *src = read_file(filename);
	if (!src)
		return EXIT_FAILURE;
	t_vec o = lex(src, strict, shstrm);
	if (!opts)
	{
		printf("[bfc] compiling unoptimized\n");
		emit(&o);
	}
	else
	{
		printf("[bfc] compiling optimized\n");
		emit_opt(&o);
	}
	compile_c(outname);
	return (0);
}
