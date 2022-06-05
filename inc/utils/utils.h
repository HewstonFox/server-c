#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>

#if __linux__

#include <malloc.h>

#define malloc_size malloc_usable_size
#elif __APPLE__

#include <malloc/malloc.h>

#endif

// UTILS PACK

int sc_atoi(const char *str);

char *sc_itoa(int number);

bool sc_isdigit(char c);

bool sc_isalpha(char c);

bool sc_islower(int c);

bool sc_isupper(int c);

bool sc_isspace(int c);

bool sc_ishex(char c);

char sc_toupper(int c);

char sc_tolower(int c);

// STRINGS

int sc_strlen(const char *s);

void sc_strdel(char **str);

int sc_get_char_index(const char *str, char c);

char *sc_strdup(const char *s1);

char *sc_strndup(const char *s1, size_t n);

char *sc_strcpy(char *dst, const char *src);

bool sc_streq(const char *s1, const char *s2);

bool sc_streqi(const char *s1, const char *s2);

char *sc_strncpy(char *dst, const char *src, int len);

int sc_strcmp(const char *s1, const char *s2);

int sc_strcmpi(const char *s1, const char *s2);

int sc_strncmp(const char *s1, const char *s2, int n);

char *sc_strcat(char *restricts1, const char *restricts2);

char *sc_strchr(const char *s, int c);

char *sc_strstr(const char *haystack, const char *needle);

int sc_get_substr_index(const char *str, const char *sub);

char *sc_strnew(int size);

char *sc_strtrim(const char *str);

char *sc_del_extra_spaces(const char *str);

int sc_count_char(const char *str, char c);

int sc_count_words(const char *str, char c);

char **sc_strsplit(const char *s, char c, int splits_count, bool skip_empty);

char **sc_strdivide(const char *s, char c);

char *sc_strjoin(const char *s1, const char *s2);

char *sc_replace_substr(const char *str, const char *sub, const char *replace);


// MEMORY

void *sc_memset(void *b, int c, size_t len);

void *sc_memcpy(void *restrictdst, const void *restrictsrc, size_t n);

void *sc_memccpy(void *restrictdst, const void *restrictsrc, int c, size_t n);

int sc_memcmp(const void *s1, const void *s2, size_t n);

void *sc_memchr(const void *s, int c, size_t n);

void *sc_memrchr(const void *s, int c, size_t n);

void *sc_memmem(const void *big, size_t big_len, const void *little,
                size_t little_len);

void *sc_memmove(void *dst, const void *src, size_t len);

void *sc_realloc(void *ptr, size_t size);

long sc_get_current_time();
