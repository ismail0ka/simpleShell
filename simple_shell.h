#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void shell_loop(void){
    char* line;// the command
    char** args;//the command arguments
    int status;// determines when to exit
    do{
        printf("$ ");
        line = shell_read_line();//read the command
        args = shell_split_line(line);//split the arguments
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

/* #Another version of the read line function using getline()
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
}*/

#define SH_TOK_BUFFER 64
#define SH_TOK_DELIM " \t\r\n\a"
char** shell_split_line(char* line){
    int buffer_size = SH_TOK_BUFFER, position = 0;
    char **tokens = (char**) malloc(buffer_size * sizeof(char*));
    char* token;

    if(!tokens){
        fprintf(stderr,"sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SH_TOK_DELIM);// tokenize the command into name and arguments
    while(token != NULL){
        tokens[position] = token;
        position++;
        if(position >= buffer_size){
            buffer_size += SH_TOK_BUFFER;
            tokens = (char**) realloc(tokens,buffer_size * sizeof(char*));
            if(!tokens){
                fprintf(stderr,"sh: allocation error \n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;//the list of args
}

int shell_launch(char **args){
    pid_t pid, wpid;
    int status;
    pid = fork();
    if(pid == 0){
        //Child process
        if(execvp(args[0],args) == -1){
            perror("sh");
        }
        exit(EXIT_FAILURE);
    }else if(pid < 0){
        //Error forking to a process
        perror("sh error forking");
    }else{
        //Parent process
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}
/*
    Declaration of built-in functions
*/
int sh_cd(char** args);
int sh_help(char** args);
int sh_exit(char** args);

/*
    List of built-in commands
*/  
char* builtin_str[] = {
    "cd",
    "help",
    "exit"
};

/*
    Pointer to the list of built-in functions that return an int and take a list of strings as an argument
*/
int (*builtin_func[]) (char**) = {
    &sh_cd,
    &sh_help,
    &sh_exit
};

/*
    return the number of built-in commands in list
*/
int sh_num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
}

/*
    Built-in funcs implementation
*/
int sh_cd(char** args){
    if(args[1] == NULL){
        fprintf(stderr,"sh: expected argument to (cd) \n");
    }else{
        if(chdir(args[1]) != 0){
            perror("No such file or repo");
        }
    }
    return 1;
}

int sh_help(char** args){
    int i;
    printf("Simple shell:\n");
    printf("type your command in the form (cmd_name cmd_arg) then hit enter");
    printf("The list of built_in commands");
    for(i=0;i<sh_num_builtins();i++){
        printf("-%d: %s \n",i+1,builtin_str[i]);
    }
    printf("for further commands use the (man) cmd");
}

int sh_exit(char **args){
    return 0;
}

int sh_execute(char** args){
    int i;
    if(args[0] == NULL){
        return 1;
    }
    for(i=0;i<sh_num_builtins();i++){
        if(strcmp(args[0],builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }
    return shell_launch(args);
}