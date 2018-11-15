#include <ucontext.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

jmp_buf jumpBuffer;

void innerCatcher(int signum, siginfo_t *info, void *ucontextVoid) {
    if (info->si_signo == SIGSEGV) {
        longjmp(jumpBuffer, 1);
    }
}

void tryToDump(int pointer) {
    struct sigaction sa;
    sa.sa_sigaction = innerCatcher;
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGSEGV, &sa, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }
    if(setjmp(jumpBuffer) == 0) {
        printf("Memory pointer = %x\n", pointer);
    } else {
        printf("Memory failed at %x\n", pointer);
        longjmp(jumpBuffer, 0);
    }
}

void catch(int signum, siginfo_t *info, void *ucontextVoid) {
    printf("Error happened at %p\n", info->si_addr);
    ucontext_t *uc = (ucontext_t *) ucontextVoid;
    for (int i = 0; i < NGREG; ++i) {
        void *call_addr;
        call_addr = (void *) uc->uc_mcontext.gregs[i];
        printf("Register [%d] %x\n", i, call_addr);
    }

    char *memory = info->si_addr - 16;
    for (int i = 0; i < 32; ++i) {
        tryToDump((int) memory + i);
    }
    exit(1);
}

void fail() {
    int p[2];
    p[1000000] = 2;

}

void invokeFail() {
    fail();
}

int main() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = catch;

    if (sigaction(SIGSEGV, &sa, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }
    printf("%s\n", "123");

    int p[2];
    p[100000] = 2;

    return 0;
}
