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
#include <default.h>
#include <auxiliary.h>

/*

	SHELL INTERACTION METHODS
	
*/


/* reset system data structures*/
void init(void)
{
	FILE *f;
	int32_t fat_itr;
	int32_t block_itr;

	/* reinitialize disk */

	remove("filesystem.dat");
	f = fopen("filesystem.dat", "a");
	fclose(f);

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
void load(void)
{
	read_fat("filesystem.dat", fat);
	actual_dir.block = ROOT_BLOCK;
	actual_dir.dirname = "root";
}


/* list entries starting from the root DIRECTORY ENTRY */
void ls(void)
{
	int32_t entry_itr;
	struct dir_entry_s dir_entry;

	printf("\nDIRECTORY: %s\n", actual_dir.dirname);
	printf("ENTRY \t| \tATTR \t| \tFIRST \t| \tSIZE \t| \tFILE\n");
	
	for (entry_itr = 0; entry_itr < DIR_ENTRIES; entry_itr++) 
	{
		read_dir_entry(actual_dir.block, entry_itr, &dir_entry);
		printf("%d \t| \t%d \t| \t%d \t| \t%d \t| \t%s\n", 
			entry_itr, dir_entry.attributes, dir_entry.first_block, dir_entry.size, dir_entry.filename);
	}
}


/* create a new DIRECTORY ENTRY over an existing one  */
void mkdir(char *path) 
{
	printf("dir: %s", actual_dir.dirname);
	int block_exists = 1;

	if (strstr(path, "/"))
	{
		char *delimiter = strrchr(path, '/')+1;
		block_exists = iter_dirs(path, delimiter);
		path = delimiter;
	}

	if (block_exists)
	{
		struct dir_entry_s dir_entry;

		int16_t first_block = fat_free();
		printf("fat free: %d\n", first_block);
		int32_t entry = dir_free(actual_dir.block, actual_dir.dirname);
		
		if (entry >= 0) 
		{
			memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
			strcpy((char *)dir_entry.filename, path);
			dir_entry.attributes = 0x02;
			dir_entry.first_block = first_block;
			dir_entry.size = 0;
			write_dir_entry(actual_dir.block, entry, &dir_entry);

			fat[first_block] = 0x7ffd;
			write_fat("filesystem.dat", fat);

			printf("> ok\n");
		}
	}
}


/* create a new file in a DIRECTORY ENTRY */
void create(char *path)
{
	int block_exists = 1;

	if (strstr(path, "/")) // file/
	{
		char *delimiter = strrchr(path, '/')+1;
		block_exists = iter_dirs(path, delimiter);
		path = delimiter;
	}

	if (block_exists) 
	{
		struct dir_entry_s dir_entry;

		int16_t first_block = fat_free();
		int32_t entry = dir_free(actual_dir.block, actual_dir.dirname);

		if (entry >= 0) 
		{
			memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
			strcpy((char *)dir_entry.filename, path);
			dir_entry.attributes = 0x01;
			dir_entry.first_block = first_block;
			dir_entry.size = 0;
			write_dir_entry(actual_dir.block, entry, &dir_entry);

			fat[first_block] = 0x7fff;
			write_fat("filesystem.dat", fat);

			printf("> ok\n");
		}
	}
}


/* unlink a file or diretory from DIRECTORY ENTRY */
void unlink(char *filename)
{
	int32_t entry;
	int32_t block = actual_dir.block;
	struct dir_entry_s dir_entry;

	for(entry = 0; entry < DIR_ENTRY_SIZE; entry++)
	{
		read_dir_entry(block, entry, &dir_entry);

		if (!strcmp((char *)&dir_entry.filename[0], filename))
		{
			if (dir_entry.attributes == 0x01) 
			{
				fat[dir_entry.first_block] = 0;

				memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
				strcpy((char *)dir_entry.filename, "");
				dir_entry.attributes = 0x00;
				dir_entry.first_block = 0;
				dir_entry.size = 0;
				write_dir_entry(block, entry, &dir_entry);
				printf("> ok\n");
			}
			else if (dir_entry.attributes == 0x02)
			{
				if(dir_is_empty(dir_entry.first_block))
				{
					fat[dir_entry.first_block] = 0;

					memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
					strcpy((char *)dir_entry.filename, "");
					dir_entry.attributes = 0x00;
					dir_entry.first_block = 0;
					dir_entry.size = 0;
					write_dir_entry(block, entry, &dir_entry);
					printf("> ok\n");
				}
				else
				{
					printf("> you need to empty directory '%s' first\n", dir_entry.filename);
				}
			}
		}
	}
}


/* enters a directory sequence */
void cd(char *path)
{
	if (strstr(path, "~"))
	{
		actual_dir.dirname = "root";
		actual_dir.block = ROOT_BLOCK;
		printf("> now on '%s'\n", actual_dir.dirname);
	}
	else
	{
		int dir_exists = iter_dirs(path, NULL);
		if (dir_exists == 1) {
			printf("> now on '%s'\n", actual_dir.dirname);
		}
	}
}