//sumfact.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void sumfact()
{
    int n1;
    printf("Enter an integer:\n");
    scanf("%d", &n1);

    if(fork() == 0)
    {
        int i, sum=0;
        for(i = 1; i <= n1; i++)
        {
            sum += i;
        }
        printf("[ID = %d] Sum of Positive integers up to %d is %d\n", getpid(), n1, sum);
        exit(0);
    }
    if(fork() == 0)
    {
        int i, factSum=1;
        for(i = 1; i <= n1; i++)
        {
            factSum *= i;
        }
        printf("[ID = %d] Factorial of %d is %d\n", getpid(), n1, factSum);
        exit(0);
    }
    
    wait(NULL);
    wait(NULL);
    printf("[ID = %d] Done", getpid());
}

int main()
{
    sumfact();
    return 0;
}