#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/*
    TO-DO:
    Make function that computes all Armstrong numbers between 
    2 and some LIMIT(initially 1 million). 
    
    Armstrong number: # == sum of their digits 
    raised to the power of the number of digits.
    Ex: 1^3 + 5^3 + 3^3 = 153 -> 3 digits

    In main: sets scheduler to debug. 
    Then create 8 child processes by calling priority_fork().
       - 2 start with priority = 0
       - 2 with priority = 5
       - 2 with priority = 10
       - 2 with priority = 15

    Each child performs same function
    LIMIT must be >= 1 million
    Reset mode to run when all child processes terminate
*/
const int LIMIT = 10000000; //Max compute num

int findArm(int num);

int main(){
    int pid;
    int count = 0;
    debug(1); //Set to debug mode

    //Create 8 child processes
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 2; j++){
            pid = priority_fork(5*i);
            if(pid < 0) {
                printf("ERROR: Fork Failed");
            }else if(pid == 0) {
                //Inside child process
                count = findArm(2);
                printf("Child PID %d (priority %d) found %d Armstrong Numbers\n", getpid(), 5*i, count);
                exit(0);
            }
        }
    }

    //Parent waits for children to end
    for(int k = 0; k < 8; k++) {
        wait(0);
    }

    return 0;    
}

int findArm(int num){
    int count = 0;
    while(num <= LIMIT) {
        int armstrong = 0;
        int x = num;
        int digits = 0;
        // Get Digits for exponent
            
        while(x > 0) {
            x /= 10;
            digits++;
        }

        //Get all the nums separate
        x = num;
        int dig = 0;
        for(int i = 0; i < digits; i++) {
            dig = x % 10;
            int y = 1;
            for(int j = 0; j < digits; j++) {
                y *= dig;
            }
            armstrong += y;
            x /= 10;
        }

        if(num == armstrong) {
            //printf("Armstrong Number: %d\n", armstrong);
            count++;    
        }
        num++;
    }
    return count; 
}