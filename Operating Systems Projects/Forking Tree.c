#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void treeCreate()
{   
    int parentId = getpid();
    pid_t ret = fork();
    if(ret > 0)
    {
        printf("[ID = %d] I am the root parent\n", parentId);
    }
    if(ret == 0)
    {   
        printf("[ID = %d] My parent is [%d]\n", getpid(), parentId);

        if(fork() == 0)
        {   
            printf("[ID = %d] My parent is [%d]\n", getpid(), parentId);
            int childId = getpid();
            if(fork() == 0)
            {
                printf("[ID = %d] My parent is [%d]\n", getpid(), childId);
                childId = getpid();
                if(fork() == 0)
                {
                    printf("[ID = %d] My parent is [%d]\n", getpid(), childId);
                }
            }
        }
        
    }
    
}

int main()
{
    treeCreate();
    return 0;
}
