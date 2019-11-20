/*
    Authors: 
        Carlos S. Castro,
        Gabriel C. Silva,
        Henrique R. Kops
*/

// built-in dependencies
#include <stdint.h>

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

    /*

        SYSTEM DEFAULT DEFINITIONS

    */

    #define BLOCK_SIZE		1024 							// size of a data block
    #define BLOCKS			2048 							// quantity of blocks
    #define FAT_SIZE		(BLOCKS * 2) 					// size of fat (4096 = 2048 * 16 bits - HEX)
    #define FAT_BLOCKS		(FAT_SIZE / BLOCK_SIZE) 		// quantity of fat blocks (4)
    #define ROOT_BLOCK		FAT_BLOCKS 						// position of root block (5)
    #define DIR_ENTRY_SIZE	32 								// size of dir entries
    #define DIR_ENTRIES		(BLOCK_SIZE / DIR_ENTRY_SIZE) 	// quantity of dir entries
    #define COMMAND_ENTRY   10                              // bytes for principal shell command entry
    #define PATH_ENTRY      500                             // bytes for path entry
    #define CONTENT_ENTRY   10000                           // bytes for file content entry


    /*

        MEMORY DATA

    */


    int8_t data_block[BLOCK_SIZE];	// DATA BLOCKS
    int16_t fat[BLOCKS]; 			// FAT
    struct directory actual_dir;	// ACTUAL DIRECTORY


    /*

        SYSTEM DEFAULT STRUCTS

    */


    struct directory
    {
        int32_t block;          // current directory block
        char *dirname;          // current directory name
    };


    struct dir_entry_s 
    {
        int8_t filename[25];	// name
        int8_t attributes;		// 0x00=free, 0x01=file, 0x02=directory
        int16_t first_block;	// first block in DATA_BLOCKS
        int32_t size;			// file/directory size
    };

#endif