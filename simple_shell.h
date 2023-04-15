#pragma once

#include <stdio.h>
#include <stdlib.h>

void shell_loop(void){
    char* line;// the command
    char** args;//the command arguments
    int status;// determines when to exit
    do{
        printf("$ ");
        line = shell_read_line();//read the command
        args = shell_split_line();//split the arguments
        status = shell_execute(args);//execute the command

        free(line);
        free(args);
    }while(status);
}

#define SH_L_BUFFER 1024
char* shell_read_line(void){
    int buff_size = SH_L_BUFFER;
    int position = 0;//track the current position in the command
    char* buffer = (char*) malloc(sizeof(char) * buff_size);
    int c;//stores the current char

    if(!buffer){
        fprintf(stderr, "sh: allocation error");
        exit(EXIT_FAILURE);
    }

    while(1){
        c = getchar();
        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;  
        }else {
            buffer[position] = c;
        }
        position++;
    }

    //If we exceed the allowed buffer size we reallocate double the size
    if(position >= buff_size){
        buff_size += SH_L_BUFFER;
        buffer = (char*) realloc(buffer,buff_size);
        if(!buffer){
            fprintf(stderr,"sh: allocation error");
            exit(EXIT_FAILURE);
        }
    }
}

char* shell_get_line(void){
    char* line = NULL;
    ssize_t buffer_size = 0;

    if(getline(&line,&buffer_size,stdin) == -1){
        if(feof(stdin)){
            exit(EXIT_SUCCESS);
        }else {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}