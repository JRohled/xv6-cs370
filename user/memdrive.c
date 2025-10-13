#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



//Fragmenting memory across multiple pages
//Reccommended: Allocate at least 64 megabytes of memory but no more than 120
//Uses frag_bytes to report fragmented, wasted memory
int main()
{
    /*  Reg Alloc */
    int allocAmt = 5; // num of KB to allocate
    int kilaByte = 1024; // Size of KB in bytes
    void *allocated[allocAmt]; // Array to store allocated block pointers

    //Allocating one MB at a time to create more fragmentation
    for(int i = 0; i < allocAmt;i++){
        allocated[i] = malloc(kilaByte);
        //printf("%d. Allocating mem at: %p\n", i, allocated[i]);
        //printf("Frag bytes after allocating: %lu\n\n", frag_bytes());
        if(!allocated[i]){
            printf("Failed at allocated[%d]", i);
            break;
        }
    }

    printf("Regular Fragmented Bytes: %lu\n", frag_bytes());

    for(int i = 0; i < allocAmt; i++){
        free(allocated[i]);
    }


    /*  Seg List Alloc  */
    void *allocatedSeg[allocAmt];

    for(int i = 0; i < allocAmt; i++){
        allocatedSeg[i] = block_alloc(kilaByte);
        if(!allocatedSeg[i]) break;

        //printf("Allocated at %p\n", allocated[i]);
    }

    printf("\nSegList Fragmented Bytes: %lu\n\n", frag_bytes_segList());

    for(int i = 0; i < allocAmt; i++){
        block_free(allocatedSeg[i]);
    }

    return 0;
}