#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

int pi[2];

void mySignal(int i)
{
    dup2(pi[0], STDIN_FILENO);
}


int main()
{
    signal(SIGUSR1, mySignal);
    pipe(pi);
    //flag to check if the parent wrote something to the terminal or not, check it in the child
    int *flag = (int*)mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *child_pid = (int*)mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int pid = fork();
    int saved = dup(STDIN_FILENO);

    if(pid == 0)
    {
        child_pid = getpid();
        for(;;)
        {   
            sleep(10);
            if(*flag == 0)
            {
                //child
                write(pi[1], "no activity detected!\n", strlen("no activity detected!\n"));
                kill(pid, SIGUSR1);
        
            }
            else if(*flag == 1)
            {
                *flag =0;
            }
        }
        return 0;
    }
    //parent
    for(;;)
    {
        dup2(saved, STDIN_FILENO);
        char* buffer = (char*)malloc(1000);
        //*flag = 0;
        int readText = read(STDIN_FILENO, buffer, 100);
        *flag = 1;
        if(strcmp(buffer, "quit\n") == 0)
        {
            kill(pid, SIGKILL);
            kill(child_pid, SIGKILL);
            free(flag);
            free(pi);
            free(buffer);
            return 0;
        }
        int fd = dup2(STDIN_FILENO, strlen(buffer)-1);
        buffer[fd] = '!';
        buffer[fd + 1] = '\n';
        printf("!%s\n", buffer);
        
    
    }

    wait(0);
    

    return 0;

}