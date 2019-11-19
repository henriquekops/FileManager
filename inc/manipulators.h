/*
    Authors: 
        Carlos S. Castro,
        Gabriel C. Silva,
        Henrique R. Kops
*/

// built-in dependencies
#include <stdint.h>
#include <definitions.h>

#ifndef MANIPULATORS_H
#define MANIPULATORS_H


    /*
    
        DEFAULT AUXILIARY PROTOTYPES
    
    */


    void read_block(char *file, int32_t block, int8_t *record);

    void write_block(char *file, int32_t block, int8_t *record);

    void read_fat(char *file, int16_t *fat);

    void write_fat(char *file, int16_t *fat);

    void read_dir_entry(int32_t block, int32_t entry, struct dir_entry_s *dir_entry);

    void write_dir_entry(int block, int entry, struct dir_entry_s *dir_entry);

#endif
