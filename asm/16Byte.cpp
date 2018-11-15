#include <cstring>
#include <stdio.h>

void *copy(void *dst, const void *src, size_t n) {
    size_t i = 0;
    while (n >= 16) {
        __asm__ (R"(
.intel_syntax noprefix
movdqu xmm1, [%0]
movdqu [%1], xmm1
)"
        :
        :"r"(((char *) src + i)), "r"(((char *) dst + i))
        :"%xmm1"
        );
        n -= 16;
        i += 16;
    }
    while (n > 0) {
        *((char *) dst + i) = *((char *) src + i);
        i++;
        n--;
    }
    return dst;
}
