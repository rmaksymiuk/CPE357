#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    sleep(1);
    int fd = shm_open("num", O_RDWR, 0600);
    if(fd != 0)
    {
        printf("Program 2 started Successfully!\n");
    }
    char *pi = (char*)mmap(NULL, 100, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    
    for(;;)
    {
        sleep(1);
        if(pi[0] != 0)
        {
            write(STDOUT_FILENO, pi, 100);
            break;
        }
        else
        {
            continue;
        }
    }
    
    close(fd);
    shm_unlink("num");
    munmap(pi, 100);
       
    return 0;
}