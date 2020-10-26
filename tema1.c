#include <stdlib.h> // standard library
#include <unistd.h> // universal standard
#include <stdio.h> // standard input/output
#include <fcntl.h> // function control
#include <errno.h> // error
#include <sys/types.h> // for files
#include <sys/stat.h> // for files
#include <sys/wait.h> // for function wayt
#include <sys/socket.h> // sockets
#include <string.h> // strings
#include <dirent.h> // director 
#include <pwd.h> // definition for struct password 
#include <stdbool.h> // for using bool. apparently bool isnt in C
#include <time.h> // pretty clear here

/*
    IMPORTANT FAX
    1. pipes, sockets, fifos 
    2. if child writes through p[1], then p[0] should be closed
    3. if parent reads through p[0], then p[1] should be closed
    4. for a bidirectional communication, we need two pipes
    5. for a bidirectional communication, we need 1 fifo
    6. fflush(stdout) is your new bff
    7. same for memset
*/

int logged_in = 0; 
int main(){
    int pipe1[2], pipe2[2]; 
    pipe(pipe1);
    pipe(pipe2); 

    pid_t pid; // Process ID 
    pid = fork();
    //PARENT
    if(pid){
        //scriem prin p1[1]
        //citim prin p2[0]
        close(pipe1[0]);
        close(pipe2[1]); 
        
        while(logged_in == 0){ // daca e logged_in = true => s-a logat baietu
            char username[256]; 
            memset(username, 0, sizeof(username)); 
            printf("Your username: \n");
           scanf("%s", username); //printf("%s\n", username);

            // sending to child 
            int length = strlen(username); 
            fflush(stdout);
            write(pipe1[1], &length, sizeof(int)); //4 
            write(pipe1[1], username, length); 
            // am trimis dimensiunea username-ului si username-ul

            read(pipe2[0], &logged_in, sizeof(int));

            if(logged_in == 0){
                printf("Mai incearca!\n");
            }
            else{
                printf("Logged succesfully"); 
                logged_in = 1;
            }

        }


    }
    //CHILD 
    if(!pid){
        //scriem prin p2[1]
        //citim prin p1[0]
        close(pipe1[1]);
        close(pipe2[0]); 
        //printf("here");
        while(logged_in == 0){
            char received_name[300]; 
            int l = 0; 
            memset(received_name, 0, sizeof(received_name)); 
            //printf("%s\n", received_name);
            read(pipe1[0], &l, sizeof(int)); 
            read(pipe1[0], received_name, l); 
            //printf("%s\n", received_name);
            FILE *file = fopen("users.txt","r"); 
            char name[300]; 
            while(!feof(file)){
                memset(name, 0, sizeof(name)); 
                fgets(name, sizeof(name), file);
                //printf("%s\n", name);
                if(name[strlen(name)-1]=='\n') 
                    name[strlen(name)-1]=0; 
                //printf("name: %s\n", name);
                //printf("received: %s\n", received_name);
                if(strcmp(name, received_name)==0){
                    //printf("aloha");
                    logged_in = 1;
                    break;
                }                 
            }
            write(pipe2[1], &logged_in, sizeof(int)); 
        }
        
        

    }
}

