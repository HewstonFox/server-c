#include "../inc/utils/utils.h"

int sc_atoi(const char *str) {
    int i = 0;
    int is_minus = 0;
    long long res_res = 0;
    long res = 0;
    while (sc_isspace(str[i]) == 1) {
        i++;
    }
    if (str[i] == '+' || str[i] == '-') {
        if (str[i] == '-')
            is_minus = 1;
        i++;
    }

    while (str[i] == '0')
        i++;

    while (sc_isdigit(str[i]) == 1) {
        res_res = res;
        res = res * 10 + str[i] - '0';
        i++;
        if (res <= res_res) {
            return res * is_minus >= 0 ? 0 : -1;
        }
    }
    if (is_minus == 1) {
        return -res;
    }
    return res;
}

char *sc_itoa(int number) {
    if (number == 0)
        return sc_strdup("0");
    if (number == -2147483648)
        return sc_strdup("-2147483648");
    int length = 0;
    int num_copy = number;
    if (num_copy < 0) {
        length++;
        num_copy *= -1;
    }
    while (num_copy != 0) {
        num_copy /= 10;
        length++;
    }
    char *num_str = sc_strnew(length);
    if (number < 0) {
        num_str[0] = '-';
        number *= -1;
    }
    num_str[length--] = '\0';
    while (number != 0) {
        num_str[length--] = (number % 10) + '0';
        number /= 10;
    }
    return num_str;
}

bool sc_isdigit(char c) { return c >= '0' && c <= '9'; }

bool sc_isalpha(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool sc_islower(int c) { return c >= 'a' && c <= 'z'; }

bool sc_isupper(int c) { return c >= 'A' && c <= 'Z'; }

bool sc_isspace(int c) { return (c >= '\t' && c <= '\r') || c == ' '; }

char sc_tolower(int c) {
    if (sc_isupper(c))
        return c + 32;
    return c;
}

char sc_toupper(int c) {
    if (sc_islower(c))
        return c - 32;
    return c;
}

int sc_strlen(const char *s) {
    int count = 0;
    while (s[count])
        count++;
    return count;
}

void sc_strdel(char **str) {
    free(*str);
    *str = NULL;
}

char *sc_strcpy(char *dst, const char *src) {
    char *orig = dst;
    while (*src)
        *dst++ = *src++;
    *dst = *src;
    return orig;
}

char *sc_strdup(const char *s1) {
    char *dup = sc_strnew(sc_strlen(s1));
    sc_strcpy(dup, s1);
    return dup;
}

char *sc_strndup(const char *s1, size_t n) {
    size_t len = (size_t) sc_strlen(s1);
    if (n < len)
        len = n;
    char *dup = sc_strnew(len);
    sc_strncpy(dup, s1, len);
    return dup;
}

char *sc_strncpy(char *dst, const char *src, int len) {
    int i = 0;
    while (src[i] && i != len) {
        dst[i] = src[i];
        i++;
    }
    if (i != len)
        dst[i] = src[i];
    return dst;
}

int sc_strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int sc_strcmpi(const char *s1, const char *s2) {
    while (*s1 && sc_tolower(*s1) == sc_tolower(*s2)) {
        s1++;
        s2++;
    }
    return sc_tolower(*s1) - sc_tolower(*s2);
}

bool sc_streq(const char *s1, const char *s2) { return !sc_strcmp(s1, s2); }

bool sc_streqi(const char *s1, const char *s2) { return !sc_strcmpi(s1, s2); }

int sc_strncmp(const char *s1, const char *s2, int n) {
    while (n--) {
        if (*s1 != *s2)
            return *(const unsigned char *) s1 - *(const unsigned char *) s2;
        s1++;
        s2++;
    }
    return 0;
}

char *sc_strcat(char *restricts1, const char *restricts2) {
    for (char *i = restricts1 + sc_strlen(restricts1); *restricts2; i++) {
        *i = *restricts2;
        restricts2++;
    }
    return restricts1;
}

char *sc_strchr(const char *s, int c) {
    for (; *s; s++)
        if (*s == c)
            return (char *) s;
    return NULL;
}

char *sc_strstr(const char *haystack, const char *needle) {
    int len = sc_strlen(needle);
    while (*haystack) {
        if (!sc_strncmp(haystack, needle, len))
            return (char *) haystack;
        haystack++;
    }
    return NULL;
}

char *sc_strnew(const int size) {
    char *str = (char *) malloc((size + 1) * sizeof(char));
    if (str == NULL)
        return NULL;
    for (int i = 0; i <= size; ++i)
        str[i] = '\0';
    return str;
}

char *sc_strtrim(const char *str) {
    if (!str)
        return NULL;
    while (sc_isspace(*str))
        str++;
    int len = sc_strlen(str);
    while (sc_isspace(str[len - 1]))
        len--;
    return sc_strndup(str, len);
}

int sc_count_char(const char *str, char c) {
    int count = 0;
    while (*str)
        if (*str++ == c)
            count++;
    return count;
}

int sc_count_words(const char *str, char c) {
    while (*str == c)
        str++;
    if (!*str)
        return 0;
    int count = 0;
    int skip = 0;
    while (*str) {
        char tmp = *str++;
        if (tmp == c && skip)
            continue;
        if (tmp == c) {
            skip = 1;
            count++;
            continue;
        }
        skip = 0;
    }
    if (*(str - 1) != c)
        count++;
    return count;
}

/**
 * @brief
 *
 * @param s str to split
 * @param c char to split str by
 * @param splits_count max count of splits, -1 to split without limit
 * @param skip_empty skip empty entries
 * @return char** NULL terminated array of strings
 */
char **sc_strsplit(const char *s, char c, int splits_count, bool skip_empty) {
    if (s == NULL)
        return NULL;
    int len = (skip_empty ? sc_count_words(s, c) : sc_count_char(s, c) + 1) + 1;
    char **words = (char **) malloc(len * sizeof(char *));
    sc_memset(words, 0, len);
    int skip = 0;
    int char_count = 0;
    int word_idx = 0;
    while (*s == c && splits_count) {
        if (!skip_empty) {
            words[word_idx++] = sc_strdup("");
            splits_count--;
        }
        s++;
    }
    while (*s && splits_count) {
        if (*s++ == c) {
            if (!skip || !skip_empty) {
                words[word_idx] = sc_strnew(char_count);
                sc_strncpy(words[word_idx], s - char_count - 1, char_count);
                word_idx++;
                skip = 1;
                char_count = 0;
                splits_count--;
            }
            continue;
        }
        skip = 0;
        char_count++;
        if (*s == '\0')
            words[word_idx++] =
                    sc_strncpy(sc_strnew(char_count), s - char_count, char_count);
    }
    if (!splits_count) {
        words[word_idx++] = sc_strdup(s);
    }
    words[word_idx] = NULL;
    return words;
}

int sc_get_char_index(const char *str, char c) {
    if (!str || !*str)
        return -2;
    int len = sc_strlen(str);
    for (int i = 0; i < len; i++)
        if (str[i] == c)
            return i;
    return -1;
}

char **sc_strdivide(const char *s, char c) {
    int divider = sc_get_char_index(s, c);
    char **res = (char **) (malloc(sizeof(char *) * 3));
    res[2] = NULL;
    if (divider == -1) {
        res[0] = sc_strdup(s);
        res[1] = sc_strdup("");
    } else {
        res[0] = sc_strndup(s, divider);
        res[1] = sc_strdup(s + divider + 1);
    }
    return res;
}

char *sc_strjoin(const char *s1, const char *s2) {
    if (s1 == NULL && s2 == NULL)
        return NULL;
    if (s1 == NULL)
        return sc_strdup(s2);
    if (s2 == NULL)
        return sc_strdup(s1);
    char *str = sc_strnew(sc_strlen(s1) + sc_strlen(s2));
    sc_strcat(str, s1);
    sc_strcat(str, s2);
    return str;
}

char *sc_del_extra_spaces(const char *str) {
    if (!str)
        return NULL;
    char *trimmed = sc_strtrim(str);
    int pointer = 0;
    int current = 0;
    while (trimmed[pointer]) {
        trimmed[current++] = trimmed[pointer++];
        if (!sc_isspace(trimmed[pointer]))
            continue;
        while (sc_isspace(trimmed[pointer]))
            pointer++;
        trimmed[current++] = ' ';
    }
    trimmed[current] = '\0';
    char *result = sc_strncpy(sc_strnew(current), trimmed, current);
    sc_strdel(&trimmed);
    return result;
}

int sc_get_substr_index(const char *str, const char *sub) {
    if (!str || !*str || !sub || !*sub)
        return -2;
    char *res_point = sc_strstr(str, sub);
    if (!res_point)
        return -1;
    return res_point - str;
}

char *sc_replace_substr(const char *str, const char *sub, const char *replace) {
    char *result;
    char *ins;
    char *tmp;
    int len_front;
    int count;
    if (!str)
        return NULL;
    if (!sub || !replace)
        return sc_strdup(str);
    int len_rep = sc_strlen(sub);
    if (len_rep == 0)
        return sc_strdup(str);
    int len_with = sc_strlen(replace);
    ins = (char *) str;
    for (count = 0; (tmp = sc_strstr(ins, sub)); ++count)
        ins = tmp + len_rep;
    tmp = result =
            (char *) malloc(sc_strlen(str) + (len_with - len_rep) * count + 1);
    if (!result)
        return NULL;
    while (count--) {
        ins = sc_strstr(str, sub);
        len_front = ins - str;
        tmp = sc_strncpy(tmp, str, len_front) + len_front;
        tmp = sc_strcpy(tmp, replace) + len_with;
        str += len_front + len_rep;
    }
    sc_strcpy(tmp, str);
    return result;
}

void *sc_memset(void *b, int c, size_t len) {
    for (size_t i = 0; i < len; i++) ((unsigned char *) b)[i] = (unsigned char) c;
    return b;
}

void *sc_memcpy(void *restrictdst, const void *restrictsrc, size_t n) {
    for (size_t i = 0; i < n; i++)
        ((unsigned char *) restrictdst)[i] = ((unsigned char *) restrictsrc)[i];
    return restrictdst;
}

void *sc_memccpy(void *restrictdst, const void *restrictsrc, int c, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((unsigned char *) restrictdst)[i] = ((unsigned char *) restrictsrc)[i];
        if (((unsigned char *) restrictdst)[i] == c)
            return (unsigned char *) restrictdst + i + 1;
    }
    return NULL;
}

int sc_memcmp(const void *s1, const void *s2, size_t n) {
    if (n == 0) return 0;
    const unsigned char *_s1 = (const unsigned char *) s1;
    const unsigned char *_s2 = (const unsigned char *) s2;
    for (size_t i = 0; *_s1 == *_s2 && i != n; i++) {
        _s1++;
        _s2++;
    }
    return *_s1 - *_s2;
}

void *sc_memchr(const void *s, int c, size_t n) {
    for (size_t i = 0; i < n; i++)
        if (((unsigned char *) s)[i] == c) return (unsigned char *) s + i;
    return NULL;
}

void *sc_memrchr(const void *s, int c, size_t n) {
    for (int i = n - 1; i >= 0; i--)
        if (((unsigned char *) s)[i] == c) return (unsigned char *) s + i;
    return NULL;
}

void *sc_memmem(const void *big, size_t big_len, const void *little,
                size_t little_len) {
    if (big_len < little_len || !big_len || !little_len) return NULL;
    unsigned char *_big = (unsigned char *) big;
    for (size_t i = 0; i < big_len; i++) {
        if (sc_memcmp(_big, little, little_len - 1) == 0) return _big;
        _big++;
    }
    return NULL;
}

void *sc_memmove(void *dst, const void *src, size_t len) {
    void *temp = malloc(len);
    temp = sc_memcpy(temp, src, len);
    dst = sc_memcpy(dst, temp, len);
    free(temp);
    temp = NULL;
    return dst;
}

void *sc_realloc(void *ptr, size_t size) {
    if (!ptr) return malloc(size);
    size_t cur_size = malloc_size(ptr);
    if (size <= cur_size) return ptr;
    void *new_ptr = malloc(size);
    sc_memcpy(new_ptr, ptr, cur_size);
    free(ptr);
    return new_ptr;
}

long sc_get_current_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (long) (t.tv_sec * 1000000 + t.tv_usec);
}
