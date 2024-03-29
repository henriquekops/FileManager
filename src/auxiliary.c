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
#include <stdlib.h>
#include <math.h>

// project dependencies
#include <manipulators.h>
#include <definitions.h>

/*

	SHELL AUXILIARY METHODS

*/


/* clear terminal window */
void cls_screen(void)
{
	int i;
	for(i = 0; i < 1000; i++) printf("\n");
}


/* extend a file when its size is greater than 1024b */
void extend_file(char* content, int32_t block)
{
	int length = strlen(content);
	if (length <= 1024)
	{
		write_block("filesystem.dat", block, (signed char*)content);
		printf("> ok\n");
	}
	else
	{
		int split = floor(length/1024);
		printf("splitting into %d parts...", split);
	}
}


/* check for the first free block in FAT */
int16_t fat_free(void) 
{
	int16_t first_block;
	for (first_block = ROOT_BLOCK + 1; first_block < BLOCKS; first_block++) 
	{
		if (fat[first_block] == 0) 
		{
			break;
		}
	}
	return first_block;
}


/* check for the first free entry in BLOCK + name validation */
int32_t dir_free(int32_t block, char *filename)
{
	int32_t entry;
	struct dir_entry_s dir_entry;

	for (entry = 0; entry < DIR_ENTRY_SIZE; entry++) 
	{
		read_dir_entry(block, entry, &dir_entry);

		if (!strcmp((char *)&dir_entry.filename[0], filename)) 
		{
			printf("> file name '%s' already exists\n", filename);
			entry = -1;
			break;
		}

		else if (dir_entry.attributes == 0) 
		{
			break;
		}

		else if (entry == 31)
		{
			printf("> there is no space at '%s'\n", actual_dir.dirname);
			entry = -1;
			break;
		}
	}
	return entry;
}


/* checks if a directory is empty */
int dir_is_empty(int16_t block)
{	
	int32_t dir_itr;
	struct dir_entry_s dir_entry;

	for(dir_itr = 0; dir_itr < DIR_ENTRY_SIZE; dir_itr++)
	{
		read_dir_entry(block, dir_itr, &dir_entry);
		if (dir_entry.attributes != 0x00) {
			return 0;
		}
	}
	return 1;
}


/* create a DIRECTORY ENTRY */
void create_dir_entry(char *filename, int8_t attributes, int32_t first_block, int32_t entry, int32_t block, struct dir_entry_s dir_entry)
{
	memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
	strcpy((char *)dir_entry.filename, filename);
	dir_entry.attributes = attributes;
	dir_entry.first_block = first_block;
	dir_entry.size = 0;
	write_dir_entry(block, entry, &dir_entry);
}


/* search folder through argued path */
struct dir_entry_s* iter_dirs(char *path, char *delimiter)
{
	int32_t entry;
	struct dir_entry_s *dir_entry = (struct dir_entry_s*)malloc(sizeof(struct dir_entry_s));

	int dir_exists = 1;
	int32_t block = actual_dir.block;

	char *token = strtok(path, "/");

	while (token != delimiter)
	{
		if (dir_exists)
		{
			for (entry = 0; entry < DIR_ENTRY_SIZE; entry++) 
			{
				read_dir_entry(block, entry, dir_entry);

				if (!strcmp((char *)dir_entry->filename, token)) 
				{
					if (dir_entry->attributes == 0x01) 
					{
						printf("> '%s' is a file\n", token);
						dir_exists = 0;
						dir_entry = NULL;
						break;
					}
					else
					{
						block = dir_entry->first_block;
						break;
					}
				}

				else if (dir_entry->attributes == 0) 
				{
					printf("> directory '%s' doesn't exist\n", token);
					dir_exists = 0;
					dir_entry = NULL;
					break;
				}
			}
		}
		else 
		{
			break;
		}
		token = strtok(NULL, "/");
	}
	return dir_entry;
}