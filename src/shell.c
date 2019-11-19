/*
    Authors: 
        Carlos S. Castro,
        Gabriel C. Silva,
        Henrique R. Kops
*/

// built-in dependencies
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// project dependencies
#include <definitions.h>
#include <manipulators.h>
#include <auxiliary.h>

/*

	SHELL INTERACTION METHODS
	
*/


/* reset system data structures*/
void minit(void)
{
	FILE *f;
	int32_t fat_itr;
	int32_t block_itr;

	/* initialize fat */
	
	for (fat_itr = 0; fat_itr < FAT_BLOCKS; fat_itr++) 
	{
		fat[fat_itr] = 0x7ffe;
	}

	fat[ROOT_BLOCK] = 0x7fff;

	for (fat_itr = ROOT_BLOCK + 1; fat_itr < BLOCKS; fat_itr++) 
	{
		fat[fat_itr] = 0;
	}

	write_fat("filesystem.dat", fat);

	/* initialize data blocks */
	
	for (block_itr = 0; block_itr < BLOCK_SIZE; block_itr++)
	{
		data_block[block_itr] = 0;
	}

	write_block("filesystem.dat", ROOT_BLOCK, data_block);

	for (block_itr = ROOT_BLOCK + 1; block_itr < BLOCKS; block_itr++)
	{
		write_block("filesystem.dat", block_itr, data_block);
	}

	/* set actual directory */

	actual_dir.dirname = "root";
	actual_dir.block = ROOT_BLOCK;

	printf("> ok\n");
}


/* load FAT from disk */
void mload(void)
{
	if (access( "filesystem.dat", F_OK ) != -1)
	{
		read_fat("filesystem.dat", fat);
		actual_dir.block = ROOT_BLOCK;
		actual_dir.dirname = "root";
		printf("> ok\n");
	}
	else
	{
		printf("> filesystem.dat does not exist\n");
	}
}


/* list entries starting from the root DIRECTORY ENTRY */
void mls(void)
{	
	int32_t entry_itr;
	struct dir_entry_s dir_entry;

	printf("\nDIRECTORY: %s\t\tBLOCK: %d\n\n", actual_dir.dirname, actual_dir.block);
	printf("ENTRY\t|\tATTR\t|\tFIRST\t|\tSIZE \t|\tFILE\n");
	
	for (entry_itr = 0; entry_itr < DIR_ENTRIES; entry_itr++) 
	{
		read_dir_entry(actual_dir.block, entry_itr, &dir_entry);
		printf("%d\t|\t%d\t|\t%d\t|\t%d\t|\t%s\n", 
			entry_itr, dir_entry.attributes, dir_entry.first_block, dir_entry.size, dir_entry.filename);
	}
}


/* create a new DIRECTORY ENTRY over an existing one  */
void mmkdir(char *path) 
{
	int32_t block;
	int is_path = 0, create = 1;
	struct dir_entry_s *navigate_dir = NULL;

	if (strstr(path, "/"))
	{
		char *delimiter = strrchr(path, '/')+1;
		navigate_dir = iter_dirs(path, delimiter, 0);
		path = delimiter;
		is_path = 1;
	}
	if (navigate_dir) 
	{
		block = navigate_dir->first_block;
	}
	else
	{
		if (is_path)
		{
			create = 0;
		}
		else
		{
			block = actual_dir.block;
		}
	}

	if (create)
	{
		struct dir_entry_s dir_entry;
		int16_t first_block = fat_free();
		int32_t entry = dir_free(block, path);

		if (entry >= 0)
		{
			create_dir_entry(path, 0x02, first_block, entry, block, dir_entry);
			fat[first_block] = 0x7ffd;
			write_fat("filesystem.dat", fat);

			printf("> ok\n");
		}
	}
}


/* create a new file in a DIRECTORY ENTRY */
void mcreate(char *path)
{
	int32_t block;
	int is_path = 0, create = 1;
	struct dir_entry_s *navigate_dir = NULL;

	if (strstr(path, "/"))
	{
		char *delimiter = strrchr(path, '/')+1;
		navigate_dir = iter_dirs(path, delimiter, 0);
		path = delimiter;
		is_path = 1;
	}

	if (navigate_dir) 
	{
		block = navigate_dir->first_block;
	}
	else
	{
		if (is_path)
		{
			create = 0;
		}
		block = actual_dir.block;
	}

	if (create)
	{
		struct dir_entry_s dir_entry;
		int16_t first_block = fat_free();
		int32_t entry = dir_free(block, path);

		if (entry >= 0) 
		{
			create_dir_entry(path, 0x01, first_block, entry, block, dir_entry);

			fat[first_block] = 0x7fff;
			write_fat("filesystem.dat", fat);

			printf("> ok\n");
		}
	}
}


/* unlink a file or diretory from DIRECTORY ENTRY */
void munlink(char *path)
{
	int32_t block;
	int is_path = 0, unlink = 1;
	struct dir_entry_s *navigate_dir = NULL;

	if (strstr(path, "/"))
	{
		char *delimiter = strrchr(path, '/')+1;
		navigate_dir = iter_dirs(path, delimiter, 0);
		path = delimiter;
		is_path= 1;
	}

	if (navigate_dir) 
	{
		block = navigate_dir->first_block;
	}
	else
	{
		if (is_path)
		{
			unlink = 0;
		}
		block = actual_dir.block;
	}

	if (unlink)
	{
		int found = 0;

		int32_t entry;
		struct dir_entry_s dir_entry;

		for(entry = 0; entry < DIR_ENTRY_SIZE; entry++)
		{
			read_dir_entry(block, entry, &dir_entry);

			if (!strcmp((char *)&dir_entry.filename[0], path))
			{
				found = 1;

				if (dir_entry.attributes == 0x01) 
				{
					fat[dir_entry.first_block] = 0;
					create_dir_entry("", 0x00, 0, entry, block, dir_entry);
					printf("> ok\n");
				}
				else if (dir_entry.attributes == 0x02)
				{
					if(dir_is_empty(dir_entry.first_block))
					{
						fat[dir_entry.first_block] = 0;
						create_dir_entry("", 0x00, 0, entry, block, dir_entry);
						printf("> ok\n");
					}
					else
					{
						printf("> you need to empty directory '%s' first\n", dir_entry.filename);
					}
				}
			}
			break;
		}
		if (!found) 
		{
			printf("> '%s' doesn't exists", path);
		}	
	}
}


void mread(char *path)
{
	int32_t block;
	int is_path = 0, read = 1;
	struct dir_entry_s *navigate_dir = NULL;

	if (strstr(path, "/"))
	{
		char *delimiter = strrchr(path, '/')+1;
		navigate_dir = iter_dirs(path, delimiter, 0);
		path = delimiter;
		is_path = 1;
	}

	if (navigate_dir) 
	{
		block = navigate_dir->first_block;
	}
	else
	{
		if (is_path)
		{
			read = 0;
		}
		block = actual_dir.block;
	}
	if (read) 
	{
		int found = 0;
		int32_t dir_itr;
		struct dir_entry_s *dir_entry;

		for(dir_itr = 0; dir_itr < DIR_ENTRY_SIZE; dir_itr++)
		{
			read_dir_entry(block, dir_itr, dir_entry);
			if (!strcmp((char *)dir_entry->filename, path))
			{
				block = dir_entry->first_block;
				found = 1;
				break;
			}
		}
		if (found)
		{
			signed char *content = (signed char*)malloc(BLOCK_SIZE*sizeof(signed char));
			read_block("filesystem.dat", block, content);
			printf("\n%s\n", content);
		}
		else
		{
			printf("> '%s' doesn't exist\n", path);
		}
	}
}


void mwrite(char *path, char *content)
{
	int32_t block;
	int is_path = 0, write = 1;
	struct dir_entry_s *navigate_dir = NULL;

	if (strstr(path, "/"))
	{
		char *delimiter = strrchr(path, '/')+1;
		navigate_dir = iter_dirs(path, delimiter, 0);
		path = delimiter;
		is_path = 1;
	}

	if (navigate_dir) 
	{
		block = navigate_dir->first_block;
	}
	else
	{
		if (is_path)
		{
			write = 0;
		}
		block = actual_dir.block;
	}
	if (write) 
	{
		int found = 0;
		int32_t dir_itr;
		struct dir_entry_s *dir_entry = (struct dir_entry_s *)malloc(sizeof(struct dir_entry_s));

		for(dir_itr = 0; dir_itr < DIR_ENTRY_SIZE; dir_itr++)
		{
			read_dir_entry(block, dir_itr, dir_entry);
			if (!strcmp((char *)dir_entry->filename, path))
			{
				block = dir_entry->first_block;
				found = 1;
				break;
			}
		}

		if (found)
		{
			write_block("filesystem.dat", block, (signed char*)content);
			printf("> ok\n");
		}
		else
		{
			printf("> '%s' doesn't exist\n", path);
		}
	}
}


/* enters a directory sequence */
void mcd(char *path)
{
	if (strstr(path, "~"))
	{
		actual_dir.dirname = "root";
		actual_dir.block = ROOT_BLOCK;
		printf("> now on '%s'\n", actual_dir.dirname);
	}
	else
	{
		struct dir_entry_s *dir_exists = iter_dirs(path, NULL, 1);
		if (dir_exists) {
			printf("> now on '%s'\n", actual_dir.dirname);
		}
	}
}