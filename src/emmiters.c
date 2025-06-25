#include "lvbfc.h"

void emit(t_vec *v, bool w, size_t s, size_t l, bool xc, bool turbo)
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
		 "#include <immintrin.h>\n"
		 "#include <emmintrin.h>\n"
		"#include <stdlib.h>\n\n"
		"int main(void) {"
		"__attribute__((aligned(256))) uint8_t arr[%lu] = {0};"
		"uint8_t *buf = &(arr[0]);\n",s);
	if (!turbo)
		fprintf(f, "size_t size = %lu;\n", s);
	if (xc)
		fprintf(f,"void *execbuf = mmap(NULL, 512, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);\n");
	size_t i = 0;
	optimize(v, l);
	while (i < v->size)
	{
		t_tokenseq x = *((t_tokenseq *)lv_vec_get(v, i));
		switch (x.op)
		{
			case '>':
				if (turbo)
					fprintf(f,
						"buf += %lu;\n",
						x.len);
				else if (w && !turbo)
					fprintf(f,
						"buf += %lu;\n"
						"if (buf >= arr + size) buf -= size;\n",
						x.len);
				else
					if (xc)
						fprintf(f, "if (buf > arr + size) {fprintf(stderr,\"Out of bounds access\"); munmap(execbuf, 512); exit(EXIT_FAILURE);}\n"
			  			"buf += %lu;\n", x.len);
					else
						fprintf(f, "if (buf > arr + size) {fprintf(stderr,\"Out of bounds access\"); exit(EXIT_FAILURE);}\n"
			  			"buf += %lu;\n", x.len);
				break;

			case '<':
				if (turbo)
					fprintf(f,
						"buf -= %lu;\n",
						x.len);
				else if (w && !turbo)
					fprintf(f,
						"buf -= %lu;\n"
						"if (buf < arr) buf += size;\n",
						x.len);
				else
					if (xc)
						fprintf(f, "buf -= %lu;\n"
			  			"if (buf < arr) {fprintf(stderr,\"Out of bounds access\"); munmap(execbuf, 512); exit(EXIT_FAILURE);}\n", x.len);
					else
						fprintf(f, "buf -= %lu;\n"
			  			"if (buf < arr) {fprintf(stderr,\"Out of bounds access\"); exit(EXIT_FAILURE);}\n", x.len);
				break;
			case '+':
				fprintf(f, "*buf += %lu;\n", x.len);
				break;

			case '-':
				fprintf(f, "*buf -= %lu;\n", x.len);
				break;

			case '.':
				for (size_t j = 0; j < x.len; j++)
					fprintf(f, "fputc(*buf, stdout);\n");
				break;
			case ',':
				for (size_t j = 0; j < x.len; j++)
					fprintf(f, "*buf = getchar();\n");
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
				fprintf(f, "*buf = %lu;\n", x.len);
			break;
			case 'S':
				fprintf(f, "*buf = -((unsigned char)%lu);\n", x.len);
			break;
			case 'Z':
				fprintf(f, "*buf = 0;\n");
			break;
			case 'M':
				fprintf(f,
					"{ uint8_t tmp = buf[0]; buf[0] = 0;\n"
					"for (size_t j = 0; j < %lu; j++) buf[j + 1] += tmp; }",
					x.len);
				break;
			case 'C':
				fprintf(f,
					"__builtin_memset(buf, *buf, %lu + 1); *buf = 0;"
					,x.len);
				break;
			case ';':
				if (x.len % 32 == 0) {
					fprintf(f,
						"if (execbuf == MAP_FAILED) { fprintf(stderr,\"mmap failed\"); exit(EXIT_FAILURE); }\n"
						"{\n"
						"    size_t chunks = %lu / 32;\n"
						"    for (size_t i = 0; i < chunks; ++i) {\n"
						"        __m256i chunk = _mm256_loadu_si256((const __m256i *)(buf + i * 32));\n"
						"        _mm256_storeu_si256((__m256i *)(execbuf + i * 32), chunk);\n"
						"    }\n"
						"}\n"
						"((void(*)(void))execbuf)();\n"
						"__builtin_memset(execbuf, 0, %lu);\n",
						x.len, x.len
					);
				} else if (x.len % 16 == 0) {
					fprintf(f,
						"if (execbuf == MAP_FAILED) { fprintf(stderr,\"mmap failed\"); exit(EXIT_FAILURE); }\n"
						"{\n"
						"    size_t chunks = %lu / 16;\n"
						"    for (size_t i = 0; i < chunks; ++i) {\n"
						"        __m128i chunk = _mm_loadu_si128((const __m128i *)(buf + i * 16));\n"
						"        _mm_storeu_si128((__m128i *)(execbuf + i * 16), chunk);\n"
						"    }\n"
						"}\n"
						"((void(*)(void))execbuf)();\n"
						"__builtin_memset(execbuf, 0, %lu);\n",
						x.len, x.len
					);
				} else {
					fprintf(f,
						"if (execbuf == MAP_FAILED) { fprintf(stderr,\"mmap failed\"); exit(EXIT_FAILURE); }\n"
						"__builtin_memcpy(execbuf, buf, %lu);\n"
						"((void(*)(void))execbuf)();\n"
						"__builtin_memset(execbuf, 0, %lu);\n",
						x.len, x.len
					);
				}
				break;
			case '&':
				fprintf(f, "if ((buf - &(arr[0])) + sizeof(uintptr_t) > size) {\n");
				if (xc)
					fprintf(f,"munmap(execbuf, 512);\n");
				fprintf(f,"fprintf(stderr,\"Overflow ptr write\");exit(EXIT_FAILURE);\n");
				fprintf(f,"}\n");
				fprintf(f, "uintptr_t __ptr =(uintptr_t)buf + %lu; __builtin_memcpy(buf, &__ptr, sizeof(uintptr_t));\n", x.len - 1);
			break;
			case '=':
				fprintf(f, "uintptr_t tmp;"
							"__builtin_memcpy(&tmp, buf, sizeof(uintptr_t));"
							"buf = (uint8_t *)tmp;\n");
			break;
			case '?':
				if (x.len == 1) {
					fprintf(f, "/*\n\n CANARY \n\n */\n");
				} else {
					fprintf(f,
						"/* CANARY: dump buffer up to and including buf (%%zu bytes) */\n"
						"{\n"
						"    size_t __n = (size_t)(buf - arr) + 1;\n"
						"    fprintf(stderr, \"[CANARY] Dumping %%zu bytes:\\n\", __n);\n"
						"    for (size_t __i = 0; __i < __n; __i++) {\n"
						"        fprintf(stderr, \"%%02X \", arr[__i]);\n"
						"        if ((__i + 1) %% 40 == 0) fprintf(stderr, \"\\n\");\n"
						"    }\n"
						"    if (__n %% 40 != 0) fprintf(stderr, \"\\n\");\n"
						"}\n"
					);
				}
				break;
			case '$':
			{
				switch (x.len)
				{
					case 1:
						fprintf(f, "*buf = 0;\n");
						break;
					case 2:
						fprintf(f,
							"if ((buf - arr) + *buf > %lu) { fprintf(stderr,\"memset overflow\"); exit(EXIT_FAILURE); }\n"
							"__builtin_memset(buf, 0, *buf);\n", s);
						break;
					case 3:
						fprintf(f,
							"if ((buf - arr) + *(buf + 1) > %lu) { fprintf(stderr,\"memset overflow\"); exit(EXIT_FAILURE); }\n"
							"__builtin_memset(buf, *buf, *(buf + 1));\n", s);
						break;
					case 5:
						fprintf(f,
							"if ((buf - arr) + 1 >= %lu) { fprintf(stderr,\"out-of-bounds multiplication\"); exit(EXIT_FAILURE); }\n"
							"*buf *= *(buf + 1);\n", s);
						break;
					default:
						break;
				}
				break;
			}
			default: break;
		}
		i++;
	}
	if (xc)
		fprintf(f,"munmap(execbuf, 512);\n");
	fprintf(f,
		"return 0;\n"
		"}\n");

	fclose(f);
	lv_vec_free(v);
}


void	emit_heap(t_vec *v, size_t ssize , size_t op, bool xc)
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
		"#include <immintrin.h>\n"
		"#include <emmintrin.h>\n"
		"#include <stdlib.h>\n\n"
		"#define GROW_BUF(n) \\\n"
		"do { \\\n"
		"    size_t offset = buf - safeg; \\\n"
		"    if (offset + (n) >= size) { \\\n"
		"        size_t new_size = (offset + (n)) * 2; \\\n"
		"        uint8_t *new_buf = realloc(safeg, new_size); \\\n"
		"        if (!new_buf) { fprintf(stderr,\"realloc failed\"); exit(EXIT_FAILURE); } \\\n"
		"        safeg = new_buf; \\\n"
		"        buf = safeg + offset; \\\n"
		"        __builtin_memset(safeg + size, 0, new_size - size); \\\n"
		"        size = new_size; \\\n"
		"    } \\\n"
		"} while (0)\n\n"
		"int main(void) {"
		"uint8_t *buf = aligned_alloc(512, %lu);\n"
		"if (!buf) { fprintf(stderr,\"unable to allocate space\"); return EXIT_FAILURE; }"
		"size_t size = %lu;\n"
		"__builtin_memset(buf, 0, %lu);\n"
		"uint8_t *safeg = buf;\n", ssize, ssize, ssize);
	if (xc)
		fprintf(f,"void *execbuf = mmap(NULL, 512, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);\n");
	size_t i = 0;
	optimize(v, op);
	while (i < v->size)
	{
		t_tokenseq x = *((t_tokenseq *)lv_vec_get_mut(v, i));
		switch (x.op)
		{
			case '>':
				fprintf(f, "GROW_BUF(%lu); buf += %lu;\n", x.len, x.len);
				break;

			case '<':
				fprintf(f,
					"buf = (buf - safeg < %lu) ? safeg + size - %lu : buf - %lu;\n",
					x.len, x.len, x.len);
				break;

			case '+':
				fprintf(f, "*buf += %lu;\n", x.len);
				break;

			case '-':
				fprintf(f, "*buf -= %lu;\n", x.len);
				break;
			case '.':
				for (size_t j = 0; j < x.len; j++)
					fprintf(f, "fputc(*buf, stdout);\n");
				break;
			case ',':
				for (size_t j = 0; j < x.len; j++)
					fprintf(f, "*buf = getchar();\n");
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
				fprintf(f, "*buf = 0;\n");
			break;
			case 'A':
				fprintf(f, "*buf = %lu;\n", x.len);
			break;
			case 'S':
				fprintf(f, "*buf = -((unsigned char)%lu);\n", x.len);
			break;
			case 'M':
				fprintf(f,
					"{ uint8_t tmp = buf[0]; buf[0] = 0;\n"
					"for (size_t j = 0; j < %lu; j++) buf[j + 1] += tmp; }",
					x.len);
				break;

			case 'C':
				fprintf(f,
					"__builtin_memset(buf, *buf, %lu + 1); *buf = 0;"
					,x.len);
				break;
			case ';':
				if (x.len % 32 == 0) {
					fprintf(f,
						"if (execbuf == MAP_FAILED) { fprintf(stderr,\"mmap failed\"); exit(EXIT_FAILURE); }\n"
						"{\n"
						"    size_t chunks = %lu / 32;\n"
						"    for (size_t i = 0; i < chunks; ++i) {\n"
						"        __m256i chunk = _mm256_loadu_si256((const __m256i *)(buf + i * 32));\n"
						"        _mm256_storeu_si256((__m256i *)(execbuf + i * 32), chunk);\n"
						"    }\n"
						"}\n"
						"((void(*)(void))execbuf)();\n"
						"__builtin_memset(execbuf, 0, %lu);\n",
						x.len, x.len
					);
				} else if (x.len % 16 == 0) {
					fprintf(f,
						"if (execbuf == MAP_FAILED) { fprintf(stderr,\"mmap failed\"); exit(EXIT_FAILURE); }\n"
						"{\n"
						"    size_t chunks = %lu / 16;\n"
						"    for (size_t i = 0; i < chunks; ++i) {\n"
						"        __m128i chunk = _mm_loadu_si128((const __m128i *)(buf + i * 16));\n"
						"        _mm_storeu_si128((__m128i *)(execbuf + i * 16), chunk);\n"
						"    }\n"
						"}\n"
						"((void(*)(void))execbuf)();\n"
						"__builtin_memset(execbuf, 0, %lu);\n",
						x.len, x.len
					);
				} else {
					fprintf(f,
						"if (execbuf == MAP_FAILED) { fprintf(stderr,\"mmap failed\"); exit(EXIT_FAILURE); }\n"
						"__builtin_memcpy(execbuf, buf, %lu);\n"
						"((void(*)(void))execbuf)();\n"
						"__builtin_memset(execbuf, 0, %lu);\n",
						x.len, x.len
					);
				}
			break;
			case '&':
				fprintf(f, "if ((buf - safeg) + sizeof(uintptr_t) > size) {\n");
				if (xc)
					fprintf(f,"munmap(execbuf, 512);\n");
				fprintf(f,"free(safeg);\n");
				fprintf(f,"fprintf(stderr,\"Overflow ptr write\");exit(EXIT_FAILURE);\n");
				fprintf(f,"}\n");
				fprintf(f, "uintptr_t __ptr =(uintptr_t)buf + %lu; __builtin_memcpy(buf, &__ptr, sizeof(uintptr_t));\n", x.len - 1);
			break;
			case '=':
				fprintf(f, "uintptr_t tmp;"
							"__builtin_memcpy(&tmp, buf, sizeof(uintptr_t));"
							"buf = (uint8_t *)tmp;\n");
			break;
			case '?':
				if (x.len == 1) {
					fprintf(f, "/*\n\n CANARY \n\n */\n");
				} else {
					fprintf(f,
						"/* CANARY: dump buffer up to and including buf (%%zu bytes) */\n"
						"{\n"
						"    size_t __n = (size_t)(buf - safeg) + 1;\n"
						"    fprintf(stderr, \"[CANARY] Dumping %%zu bytes:\\n\", __n);\n"
						"    for (size_t __i = 0; __i < __n; __i++) {\n"
						"        fprintf(stderr, \"%%02X \", safeg[__i]);\n"
						"        if ((__i + 1) %% 40 == 0) fprintf(stderr, \"\\n\");\n"
						"    }\n"
						"    if (__n %% 40 != 0) fprintf(stderr, \"\\n\");\n"
						"}\n"
					);
				}
				break;
			case '$':
			{
				switch (x.len)
				{
					case 1:
						fprintf(f, "*buf = 0;\n");
						break;
					case 2:
						fprintf(f,
							"if ((buf - safeg) + *buf > size) { fprintf(stderr,\"memset overflow\"); exit(EXIT_FAILURE); }\n"
							"__builtin_memset(buf, 0, *buf);\n");
						break;
					case 3:
						fprintf(f,
							"if ((buf - safeg) + *(buf + 1) > size) { fprintf(stderr,\"memset overflow\"); exit(EXIT_FAILURE); }\n"
							"__builtin_memset(buf, *buf, *(buf + 1));\n");
						break;
					case 5:
						fprintf(f,
							"if ((buf - safeg) + 1 >= size) { fprintf(stderr,\"out-of-bound multiplication\"); exit(EXIT_FAILURE); }\n"
							"*buf *= *(buf + 1);\n");
						break;
					default:
						break;
				}
				break;
			}
			default:
				break;
		}
		i++;
	}

	if (xc)
		fprintf(f,"munmap(execbuf, 512);\n");
	fprintf(f,
		"free(safeg);\n"
		"return 0;\n"
		"}");

	fclose(f);
	lv_vec_free(v);
}
