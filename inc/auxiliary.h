/*
    Authors: 
        Carlos S. Castro,
        Gabriel C. Silva,
        Henrique R. Kops
*/

// built-in dependencies
#include <stdint.h>

#ifndef AUXILIARY_H
#define AUXILIARY_H

    /*
    
        AUXILIARY SHELL PROTOTYPES
    
    */

   int16_t fat_free(void);

   int32_t dir_free(int32_t block, char *filename);

   int dir_is_empty(int16_t block);
   
   int iter_dirs(char *path, char *delimiter);

#endif