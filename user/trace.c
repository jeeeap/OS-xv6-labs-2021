#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) 
{
    if(argc < 2){
        fprintf(2, "Usage: trace mask command [args...]\n");
        exit(1);
    }

    int mask = atoi(argv[1]);
    if(mask <= 0){
        fprintf(2, "trace: mask must be positive\n");
        exit(1);
    }

    trace(mask);
    
    // 执行后续命令
    exec(argv[2], &argv[2]);
    fprintf(2, "trace: exec %s failed\n", argv[2]);
    exit(1);
}