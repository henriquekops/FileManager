/*
    Authors: 
        Carlos S. Castro,
        Gabriel C. Silva,
        Henrique R. Kops
*/

// built-in dependencies
#include <stdint.h>

#ifndef SHELL_H
#define SHELL_H

    /*
    
        SHELL INTERACTION PROTOTYPES
    
    */

    void init(void);

    void load(void);

    void ls(void);

    void mkdir(char *path);

    void create(char *path);

    void unlink(char *path);

    void cd(char *path);

#endif