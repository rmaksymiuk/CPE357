#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

#define BUFFERSIZE 100


// int pi[2];

// void mySignal(int i)
// {
//     dup2(pi[0], STDIN_FILENO);
// }


// int main()
// {
//     signal(SIGUSR1, mySignal);
//     pipe(pi);
//     //flag to check if the parent wrote something to the terminal or not, check it in the child
//     int *flag = (int*)mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//     int pid = fork();
//     int saved = dup(STDIN_FILENO);

//     if(pid == 0)
//     {
        
//         for(;;)
//         {   
//             //write(STDOUT_FILENO, "in child\n", strlen("in child\n"));
//             sleep(10);
//             if(*flag == 0)
//             {
//                 //child
//                 write(pi[1], "no activity detected!\n", strlen("no activity detected!\n"));
//                 kill(pid, SIGUSR1);
        
//             }
//             else if(*flag == 1)
//             {
//                 *flag =0;
//             }
//         }
//         return 0;
//     }
//     //parent
//     for(;;)
//     {
//         dup2(saved, STDIN_FILENO);
//         char* buffer = (char*)malloc(1000);
//         *flag = 0;
//          int readText = read(STDIN_FILENO, buffer, 1000);
//          buffer[readText] = 0;
//         *flag = 1;
//         if(strcmp(buffer, "quit\n") == 0)
//         {
//             kill(pid, SIGKILL);
//             free(buffer);
//             return 0;
//         }
//         //int fd = dup2(STDIN_FILENO, strlen(buffer)-1);
//         strcat(buffer, "!");
//         //buffer[fd] = '!';
//         //buffer[fd + 1] = '\n';
//         printf("!%s", buffer);
//         //fflush(stdout);
        
    
//     }

//     wait(0);
    

//     return 0;

// }


//Getting all the aruguments in from the input from user
char **tokenizeArgs(char* args, char** arg_tokens)
{
    int num_args  = 0;
    char* token = strtok(args, " ");

    while( token != NULL)
    {
        arg_tokens[num_args] = token;
        token = strtok(NULL, " ");
        num_args++;
    }

    return arg_tokens;
}

//Length of the arguments
int getArgLen(char **arg_tokens)
{
    int i = 0;
    while(arg_tokens[i])
    {
        i++;
    }
    return i;
}



//global variables
int fd[2];
char print = 0;

//sigHandler - redirects stdin
void sigHandler(int i)
{
    dup2(fd[0], STDIN_FILENO);
    print = 1;
    return;
}


/*
cwd - current directory
filename - file that we're looking for
retVal - path to the file
*/
// find a file in a directory and subdirctory
char* findFile(char* cwd, char* filename, char* retVal) { 

    DIR *dir = opendir(cwd);
    struct dirent *entry;

    while((entry = readdir(dir))!= NULL) {
        //for each entry check:
        // 1. does the entry name match filename?
        if(strcmp(entry->d_name, filename) == 0) {
             strcat(retVal, cwd);
        }

        // 2. is the entry a directory? recursion!
        if(entry->d_type == DT_DIR && strncmp(entry->d_name, ".", 1) != 0 && strncmp(entry->d_name, "..", 2) != 0) {
            char newcwd[5000];
            strcat(newcwd, cwd);
            strcat(newcwd, entry->d_name);
            findFile(newcwd, filename, retVal);
        }
    }

    closedir(dir);
    return retVal;

}

//Outline:
//pipe(fd)
//signal(SIGUSR1, sigHandler)


int main()
{
    char args[BUFFERSIZE];
    char* arg_tokens[10] = {0};
    // setup pipe and signals
    signal(SIGUSR1, sigHandler);
    pipe(fd);
    //fd[0] -read from
    //fd[1] - write to
    int *childrenListPID = (int*)mmap(NULL, 10*sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *childCounter = (int*)mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *child_pid = (int*)mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int saved_stdin = dup(STDIN_FILENO);
    int status_pid = 0;
    int parent_pid = getpid();
    bool limitReached = false;

    *childCounter = 0;
    while(1) 
    {
        //restore
        dup2(saved_stdin, STDIN_FILENO);
        write(STDOUT_FILENO, "\nfindstuff$", 11);
        read(STDIN_FILENO, args, BUFFERSIZE);
        tokenizeArgs(args, arg_tokens);
        getArgLen(arg_tokens);

        if(strcmp(arg_tokens[0], "find") == 0) 
        {
            // find and filename
            char *filename = arg_tokens[1];
            if(*childCounter >= 10)
                limitReached = true;
            if(fork() == 0 && !limitReached)
            {

                int arrLength =  getArgLen(arg_tokens);
                close(fd[0]);
                *child_pid = getpid();
                childrenListPID[*childCounter] = *child_pid;
                ++*childCounter;
                write(fd[1], "In Child!\n", strlen("In Child!\n" )+1);
                kill(parent_pid, SIGUSR1);
                if(arrLength == 2)
                {
                    printf("In current directory\n");
                    //current directory
                    char cwd[1000];
                    getcwd(cwd, 1000);
                    DIR *dir = opendir(cwd);
                    struct dirent *entry;

                    while((entry = readdir(dir))!= NULL) 
                    {
                        if(strcmp(entry->d_name, arg_tokens[1]))
                        {
                            write(fd[1], arg_tokens[1], strlen(arg_tokens[1]));
                            write(fd[1], '\t' + cwd + '\n', strlen('\t' + cwd + '\n'));

                        }
                    }

                }
                else if(strncmp(arg_tokens[2], "-f", 2) == 0) 
                {
                    //find the root directory and subdirectories
                    printf("in -f\n");
                    char* result = "";
                    result = findFile("/home", filename, result);
                    //  root/usr/bin instead of root
                    // or using home as the root

                }
                else if(strncmp(arg_tokens[2], "-s",2) == 0) 
                {
                    //current directory and subdirectories
                    printf("in -s\n");
                    char buffer[1000];
                    getcwd(buffer, 1000);
                    char* result = "";
                    result = findFile(buffer, filename, result);

                    //write to pipe fd[1]
                    //interrupt the parent
                    //close everything and return
                    
                }
                close(fd[1]);
                return 0;
            }
            else if(*childCounter >= 10)
            {
                
                printf("\nThere are already at least 10 children running.\n");
            }
        }
        else if(strncmp(arg_tokens[0], "-q",2) == 0) 
        {
            //kill the children
            if(*childCounter > 0)
            {
                for(int i = 0; i < *childCounter; i++)
                {
                    kill(childrenListPID[i], SIGKILL);
                     //wait for the children
                    if(waitpid(childrenListPID[i], &status_pid, WNOHANG) == -1)
                    {   
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            // quit
            return 0;
        }
        else 
        {
            //invalid
            printf("\ncommand not found: %s\n", arg_tokens[0]);
        }
        //wait for the finshed children and take off list
        sleep(10);
        if(waitpid(*child_pid, &status_pid, WNOHANG) == -1)
        {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }  
        fflush(stdout);
    }
    
    return 0;
}
