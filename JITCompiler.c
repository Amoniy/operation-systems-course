#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

const size_t SIZE = 1024;

void *allocateExecutableMemory(size_t size) {
    void *ptr = mmap(0, size,
                     PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == (void *) -1) {
        perror("mmap");
        return NULL;
    }
    return ptr;
}

void insertCode(unsigned char *memoryPointer) {
    unsigned char code[] = {
//            0x55, 0x48, 0x89, 0xe5, 0x48, 0x89, 0x7d, 0xf8, 0x48, 0x8b, 0x7d, 0xf8, 0x48, 0x83, 0xc7, 0x04, 0x48, 0x89, 0xf8, 0x5d, 0xc3 addFour
            0x55, 0x48, 0x89, 0xe5, 0x89, 0x7d, 0xfc, 0x8b, 0x7d, 0xfc, 0xc1, 0xe7, 0x01, 0x89, 0xf8, 0x5d,
            0xc3 // multiplyByTwo
    };
    memcpy(memoryPointer, code, sizeof(code));
}

int main() {
    void *memoryPointer = allocateExecutableMemory(SIZE);
    insertCode(memoryPointer);

    int (*functionPointer)(int) = memoryPointer;
    int result = functionPointer(2);

    printf("result = %d\n", result);
    munmap(memoryPointer, SIZE);
    return 0;
}