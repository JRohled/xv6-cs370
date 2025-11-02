#include "user.h"

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
int main(){

}