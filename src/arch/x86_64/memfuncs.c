#include "memfuncs.h"
void *memset(void *dst, int c, size_t n) {
    unsigned int i = 0;

    int *dest = (int *) dst;

    while (i < n) {
        dest[i] = c;
        i++;
    }

    return dst;
}

void *memcpy(void *dest, const void *src, size_t n) {
    char *destPtr = (char *) dest;
    char *srcPtr = (char *) src;

    unsigned int i = 0;

    while (i < n) {
        destPtr[i] = srcPtr[i];
        i++;
    }

    return dest;

}

size_t strlen(const char *s) {
    const char *temp = s;
    int size = 0;

    while (*temp != '\0') {
        size++;
        temp++;
    }

    return size;
}

char *strcpy(char *dest, const char *src) {
    const char *temp = src;
    int idx = 0;

    while (*temp != '\0') {
        dest[idx] = *temp;
        idx++;
        temp++;
    }
    dest[idx] = '\0';

    return dest;
}

/*
int strcmp(const char *s1, const char *s2) {
    return 0;
}

const char *strchr(const char *s, int c) {
    return NULL;
}

char *strdup(const char *s) {
    return NULL;
}
*/

