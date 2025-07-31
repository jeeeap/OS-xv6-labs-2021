#include "kernel/types.h"
#include "user/user.h"

void sieve(int input_fd) {
    int prime, num;
    if(read(input_fd, &prime, sizeof(int))) {
        printf("prime %d\n", prime);

        int p[2];
        pipe(p);

        if(fork() == 0) {
            close(p[1]);
            sieve(p[0]);
        } else {
            while(read(input_fd, &num, sizeof(int))) {
                if(num % prime != 0)
                    write(p[1], &num, sizeof(int));
            }
            close(p[1]);
            wait(0);
        }
    }
    exit(0);
}

int main() {
    int p[2];
    pipe(p);

    for(int i=2; i<=34; i++) {
        write(p[1], &i, sizeof(int));
    }

    if(fork() == 0) {
        close(p[1]);
        sieve(p[0]);
    } else {
        close(p[1]);
        wait(0);
    }
    exit(0);
}