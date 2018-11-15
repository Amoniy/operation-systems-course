#include<stdio.h>
#include <cstring>
#include <iostream>
#include <nmmintrin.h>

using namespace std;

int countNaive(char *string, size_t size) {
    int wordCount = 0;
    if (size == 0) {
        return 0;
    }
    size_t i = 0;
    if (string[i] != ' ') {
        wordCount++;
    }
    i++;
    while (i < size) {
        if (string[i] != ' ' && string[i - 1] == ' ') {
            wordCount++;
        }
        i++;
    }
    return wordCount;
}

const __m128i SPACES_MASK = _mm_set1_epi8((char) 32);
const __m128i ONE_MASK = _mm_set1_epi8((char) 1);

int countSSE(char *string, size_t size) {
    size_t i = 0;
    int ans = 0;
    while (size > 16) {
        __m128i stringLeftShift = _mm_and_si128(_mm_cmpgt_epi8(_mm_loadu_si128((__m128i *) (string + i - 1)),
                                                               SPACES_MASK), ONE_MASK);
        __m128i actualString = _mm_and_si128(_mm_cmpgt_epi8(_mm_loadu_si128((__m128i *) (string + i)), SPACES_MASK),
                                             ONE_MASK);
        __m128i stringXor = _mm_xor_si128(stringLeftShift, actualString);
        int a = __builtin_popcountll((long long) (reinterpret_cast<uint64_t *> (&stringXor)[0])) +
                __builtin_popcountll((long long) (reinterpret_cast<uint64_t *> (&stringXor)[1]));
        ans += a;
        i += 16;
        size -= 16;
    }
    ans /= 2;
    if (string[i - 1] != ' ' && string[i] == ' ') {
        ans++;
    }
    ans += countNaive(string + i, size);
    return ans;
}

int main() {
    char src[288] = "  abc bbbbbb bbbbb bbbbbbbbbbbbbbbbbbbb bbbbbbbbbbbbbbbbbbbb bbbbbbbbbbbbbbbbbbbb bbbbbbbbbbbbbbbbb    ";
    int n = strlen(src);
    printf("Word count countSSE = %d\n", countSSE(src, n));
    printf("Expected word count = %d\n", countNaive(src, n));
}
