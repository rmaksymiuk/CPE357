#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>


int main()
{
    // Creates shared memory
    int fd = shm_open("num", O_CREAT|O_RDWR, 0600);
    // Allocate size in the file
    ftruncate(fd, 100);
    // Mmap and make that the secret pointer
    char *pi = (char*)mmap(NULL, 100, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    pi[0] = 0;
    read(STDIN_FILENO, pi, 100);
    for(;;)
    {
        read(STDIN_FILENO, pi, 100);
        if(strcmp(pi, "quit\n") == 0)
        {
           break;
        }
    
    }

   
    
    close(fd);
    shm_unlink("num");
    munmap(pi, 100);
       
    return 0;
}