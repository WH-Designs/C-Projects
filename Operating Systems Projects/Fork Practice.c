// fork7.c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void fork7()
{
int ret;
ret = fork();

if (ret == 0)
{
    printf("\n [%d] Running Child \n", getpid());
    sleep(2);
    printf("\n [%d] Ending Child \n", getpid());
}
else
{
    printf("\n [%d] Waiting Parent \n", getpid());
    wait(NULL);
    printf("\n [%d] Ending Parent \n", getpid());
}
}
int main()
{
fork7();
return 0;
}