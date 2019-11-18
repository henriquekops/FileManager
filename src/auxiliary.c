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
#include <default.h>
#include <definitions.h>

/*

	SHELL AUXILIARY METHODS

*/


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
			printf("> there is no space at '%s' to add more stuff\n", actual_dir.dirname);
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


/* search folder through argued path */
int iter_dirs(char *path, char *delimiter)
{
	int32_t entry;
	struct dir_entry_s dir_entry;

	int dir_exists = 1;

	char *token = strtok(path, "/");

	while (token != delimiter)
	{
		if (dir_exists)
		{
			for (entry = 0; entry < DIR_ENTRY_SIZE; entry++) 
			{
				read_dir_entry(actual_dir.block, entry, &dir_entry);

				if (!strcmp((char *)&dir_entry.filename[0], token)) 
				{
					actual_dir.dirname = token;
					actual_dir.block = dir_entry.first_block;
					break;
				}

				else if (dir_entry.attributes == 0) 
				{
					printf("> directory '%s' doesn't exist\n", token);
					dir_exists = 0;
					break;
				}
			}
		}
		else 
		{
			printf("> returning to root\n");
			actual_dir.block = ROOT_BLOCK;
			actual_dir.dirname = "root";
			break;
		}
		token = strtok(NULL, "/");
	}
	return dir_exists;
}