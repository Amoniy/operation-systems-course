#include <cstring>
#include <stdio.h>

void *copy(void *dst, const void *src, size_t n) {
//    char *dst8 = (char *)dst;
//    char *src8 = (char *)src;
//    while(n--){
//        *dst8++ = *src8++;
//    }
    size_t i = 0;
    while (n > 0) {
        *((char *) dst + i) = *((char *) src + i);
        i++;
        n--;
    }
    return dst;
}
