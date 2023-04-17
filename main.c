#include "simple_shell.h"

int main(int argc,char** argv){
    //Load any config files
    shell_loop();
    //Perform any cleanup (&& ||) shutdown 
    return EXIT_SUCCESS;
}