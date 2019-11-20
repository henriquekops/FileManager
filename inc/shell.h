/*
    Authors: 
        Carlos S. Castro,
        Gabriel C. Silva,
        Henrique R. Kops
*/

#ifndef SHELL_H
#define SHELL_H

    /*
    
        SHELL INTERACTION PROTOTYPES
    
    */

    void minit(void);

    int mload(void);

    void mls(void);

    void mmkdir(char *path);

    void mcreate(char *path);

    void munlink(char *path);

    void mread(char *path);

    void mwrite(char *path, char *content);

    void mcd(char *path);

#endif