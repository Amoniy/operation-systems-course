#include <cstring>
#include <stdio.h>

void *copy(void *dst, const void *src, size_t n) {
    size_t i = 0;
    while (n > 8) {
        __asm__ (R"(
.intel_syntax noprefix
mov rax, [%0]
mov [%1], rax
)"
        : // output
        :"r"(((char *) src + i)), "r"(((char *) dst + i)) // input
        :"%rax" // registers to clean
        );
        i += 8;
        n -= 8;
    }
    while (n > 0) {
        *((char *) dst + i) = *((char *) src + i);
        i++;
        n--;
    }
    dst -= n;
    return dst;
}
