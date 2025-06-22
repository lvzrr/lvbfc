#include "lvbfc.h"

void emit(t_vec *v, bool w, size_t s, size_t l)
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
	optimize(v, l);
	while (i < v->size)
	{
		t_tokenseq x = *((t_tokenseq *)lv_vec_get(v, i));
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
			case '+':
				fprintf(f, "*buf += %lu;", x.len);
				break;

			case '-':
				fprintf(f, "*buf -= %lu;", x.len);
				break;

			case '.':
				for (size_t j = 0; j < x.len; j++)
					fprintf(f, "fputc(*buf, stdout);");
				break;
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
			case 'A':
				fprintf(f, "*buf = %lu;", x.len);
			break;
			case 'S':
				fprintf(f, "*buf = -((unsigned char)%lu);", x.len);
			break;
			case 'Z':
				fprintf(f, "*buf = 0;");
			break;
			case 'M':
				fprintf(f,
					"{ uint8_t tmp = buf[0]; buf[0] = 0;"
					"for (size_t j = 0; j < %lu; j++) buf[j + 1] += tmp; }",
					x.len);
				break;
			case 'C':
				fprintf(f,
					"{ uint8_t tmp = buf[0]; buf[0] = 0;"
					"for (size_t j = 0; j < %lu; j++) buf[j + 1] += tmp; }",
					x.len);
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


void	emit_heap(t_vec *v, size_t op)
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
		"#include <sys/mman.h>\n"
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
	optimize(v, op);
	while (i < v->size)
	{
		t_tokenseq x = *((t_tokenseq *)lv_vec_get_mut(v, i));
		switch (x.op)
		{
			case '>':
				fprintf(f, "GROW_BUF(%lu); buf += %lu;", x.len, x.len);
				break;

			case '<':
				fprintf(f,
					"buf = (buf - safeg < %lu) ? safeg + size - %lu : buf - %lu;",
					x.len, x.len, x.len);
				break;

			case '+':
				fprintf(f, "*buf += %lu;", x.len);
				break;

			case '-':
				fprintf(f, "*buf -= %lu;", x.len);
				break;

			case '.':
				for (size_t j = 0; j < x.len; j++)
					fprintf(f, "fputc(*buf, stdout);");
				break;
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
				fprintf(f, "*buf = 0;");
			break;
			case 'A':
				fprintf(f, "*buf = %lu;", x.len);
			break;
			case 'S':
				fprintf(f, "*buf = -((unsigned char)%lu);", x.len);
			break;
			case 'M':
				fprintf(f,
					"{ uint8_t tmp = buf[0]; buf[0] = 0;"
					"for (size_t j = 0; j < %lu; j++) buf[j + 1] += tmp; }",
					x.len);
				break;

			case 'C':
				fprintf(f,
					"{ uint8_t tmp = buf[0]; buf[0] = 0;"
					"for (size_t j = 0; j < %lu; j++) buf[j + 1] += tmp; }",
					x.len);
				break;
			case ';':
				fprintf(f,
					"void *execbuf = mmap(NULL, %lu, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);"
					"if (execbuf == MAP_FAILED) { perror(\"mmap failed\"); exit(EXIT_FAILURE); }"
					"memcpy(execbuf, buf, %lu);"
					"((void(*)(uint8_t *))execbuf)(buf);"
					"munmap(execbuf, %lu);",
					x.len, x.len, x.len);
				break;
			default:
				break;
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
