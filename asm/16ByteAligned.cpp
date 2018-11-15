#include <cstring>
#include <stdio.h>

void *copy1(void *dst, const void *src, size_t n) {
    size_t i = 0;
    while (n > 0) {
        *((char *) dst + i) = *((char *) src + i);
        i++;
        n--;
    }
    return dst;
}

void *copy8(void *dst, const void *src, size_t n) {
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
    return dst;
}

void *copy16(void *dst, const void *src, size_t n) {
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

void *copy16Aligned(void *dst, const void *src, size_t n) {
    size_t i = 0;
    while ((size_t) dst % 16/*4?*/ != 0 && n > 0) {
        *((char *) dst + i) = *((char *) src + i);
        i++;
        n--;
    }

    while (n > 16) {
        __asm__ (R"(
.intel_syntax noprefix
movdqa xmm1, [%0]
movdqa [%1], xmm1
)"
        :
        :"r"(((char *) src + i)), "r"(((char *) dst + i))
        :"%xmm1"
        );
        i += 16;
        n -= 16;
    }

    while (n > 0) {
        *((char *) dst + i) = *((char *) src + i);
        i++;
        n--;
    }

    return dst;
}

int main() {
    int n = 19;
    char src[21] = "01234567890123456789";
    char dst[20];
    n -= 3;
    copy16Aligned(dst, ((char *) src + 3) , n );
    dst[n] = '\0';
    printf("%s\n", dst);
}
