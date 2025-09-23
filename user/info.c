#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
    int pages = freec();
    int bytes = pages * 4096;
    int megabytes = bytes / (1024 * 1024);
    
    printf("Free pages according to freec: %d\n", pages);
    printf("Total bytes free: %d\n", bytes);
    printf("Total megabytes free: %d\n", megabytes);

    return 0;
}