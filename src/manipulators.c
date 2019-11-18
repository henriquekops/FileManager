/*
    Authors: 
        Carlos S. Castro,
        Gabriel C. Silva,
        Henrique R. Kops
*/

// built-in dependencies
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// project dependencies
#include <definitions.h>

/*

	DATA MANIPULATION AUXILIARY METHODS

*/


/* reads a data block from disk */
void read_block(char *file, int32_t block, int8_t *record)
{
	FILE *f;

	f = fopen(file, "r+");
	fseek(f, block * BLOCK_SIZE, SEEK_SET);
	fread(record, 1, BLOCK_SIZE, f);
	fclose(f);
}


/* writes a data block to disk */
void write_block(char *file, int32_t block, int8_t *record)
{
	FILE *f;

	f = fopen(file, "r+");
	fseek(f, block * BLOCK_SIZE, SEEK_SET);
	fwrite(record, 1, BLOCK_SIZE, f);
	fclose(f);
}


/* reads the FAT from disk */
void read_fat(char *file, int16_t *fat)
{
	FILE *f;

	f = fopen(file, "r+");
	fseek(f, 0, SEEK_SET);
	fread(fat, 2, BLOCKS, f);
	fclose(f);
}


/* writes the FAT to disk */
void write_fat(char *file, int16_t *fat)
{
	FILE *f;

	f = fopen(file, "r+");
	fseek(f, 0, SEEK_SET);
	fwrite(fat, 2, BLOCKS, f);
	fclose(f);
}


/* reads a DIRECTORY ENTRY from a directory */
void read_dir_entry(int32_t block, int32_t entry, struct dir_entry_s *dir_entry)
{
	read_block("filesystem.dat", block, data_block);
	memcpy(dir_entry, &data_block[entry * sizeof(struct dir_entry_s)], sizeof(struct dir_entry_s));
}


/* writes a DIRECTORY ENTRY in a directory */
void write_dir_entry(int block, int entry, struct dir_entry_s *dir_entry)
{
	read_block("filesystem.dat", block, data_block);
	memcpy(&data_block[entry * sizeof(struct dir_entry_s)], dir_entry, sizeof(struct dir_entry_s));
	write_block("filesystem.dat", block, data_block);
}