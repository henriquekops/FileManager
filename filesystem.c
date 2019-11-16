// Authors: Carlos S. Castro, Gabriel C. Silva & Henrique R. Kops
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE		1024 							// size of a data block
#define BLOCKS			2048 							// quantity of blocks
#define FAT_SIZE		(BLOCKS * 2) 					// size of fat (4096 = 2048 * 16 bits - HEX)
#define FAT_BLOCKS		(FAT_SIZE / BLOCK_SIZE) 		// quantity of fat blocks (4)
#define ROOT_BLOCK		FAT_BLOCKS 						// position of root block (5)
#define DIR_ENTRY_SIZE	32 								// size of dir entries
#define DIR_ENTRIES		(BLOCK_SIZE / DIR_ENTRY_SIZE) 	// quantity of dir entries


/* Memory data */

int8_t data_block[BLOCK_SIZE];	// DATA BLOCKS
int16_t fat[BLOCKS]; 			// FAT
struct directory actual_dir;	// ACTUAL DIRECTORY


/*
	STRUCTS
	=======
*/

struct directory
{
	int32_t block;				// current directory block
	char *dirname;				// current directory name
	char *filename_to_save;		// file in current directory
};


struct dir_entry_s 
{
	int8_t filename[25];	// name
	int8_t attributes;		// 0=free, 1=directory, 2=file
	int16_t first_block;	// first block in DATA_BLOCKS
	int32_t size;			// file/directory size
};


/*
	DEFAULT AUXILIARY METHODS
	=========================
*/


void read_block(char *file, int32_t block, int8_t *record)
{
	/* reads a data block from disk */

	FILE *f;

	f = fopen(file, "r+");
	fseek(f, block * BLOCK_SIZE, SEEK_SET);
	fread(record, 1, BLOCK_SIZE, f);
	fclose(f);
}


void write_block(char *file, int32_t block, int8_t *record)
{
	/* writes a data block to disk */

	FILE *f;

	f = fopen(file, "r+");
	fseek(f, block * BLOCK_SIZE, SEEK_SET);
	fwrite(record, 1, BLOCK_SIZE, f);
	fclose(f);
}


void read_fat(char *file, int16_t *fat)
{
	/* reads the FAT from disk */

	FILE *f;

	f = fopen(file, "r+");
	fseek(f, 0, SEEK_SET);
	fread(fat, 2, BLOCKS, f);
	fclose(f);
}


void write_fat(char *file, int16_t *fat)
{
	/* writes the FAT to disk */

	FILE *f;

	f = fopen(file, "r+");
	fseek(f, 0, SEEK_SET);
	fwrite(fat, 2, BLOCKS, f);
	fclose(f);
}


void read_dir_entry(int32_t block, int32_t entry, struct dir_entry_s *dir_entry)
{
	/* reads a DIRECTORY ENTRY from a directory */

	read_block("filesystem.dat", block, data_block);
	memcpy(dir_entry, &data_block[entry * sizeof(struct dir_entry_s)], sizeof(struct dir_entry_s));
}


void write_dir_entry(int block, int entry, struct dir_entry_s *dir_entry)
{
	/* writes a DIRECTORY ENTRY in a directory */

	read_block("filesystem.dat", block, data_block);
	memcpy(&data_block[entry * sizeof(struct dir_entry_s)], dir_entry, sizeof(struct dir_entry_s));
	write_block("filesystem.dat", block, data_block);
}


/*
	SHELL AUXILIARY METHODS
	=======================
*/


int16_t fat_free() 
{
	/* check for the first free block in FAT */

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


int32_t dir_free(int32_t block, char *filename) // TODO: alter logic for blocking 31th dir_entry
{
	/* check for the first free entry in BLOCK + name validation */

	int32_t entry;
	struct dir_entry_s dir_entry;

	for (entry = 0; entry < DIR_ENTRY_SIZE; entry++) 
	{
		read_dir_entry(block, entry, &dir_entry);

		if (!strcmp((char *)&dir_entry.filename[0], filename)) 
		{
			entry = -1;
			break;
		}

		if (dir_entry.attributes == 0) 
		{
			break;
		}
	}
	return entry;
}



int iter_dirs(char *path)
{
	/* search folder through argued path */

	int32_t entry;
	struct dir_entry_s dir_entry;

	int dir_exists = 1;

	char *last = strrchr(path, '/')+1;
	char *token = strtok(path, "/");

	actual_dir.filename_to_save = last;

	while (token != last)
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
			actual_dir.filename_to_save = "";
			break;
		}
		token = strtok(NULL, "/");
	}
	return dir_exists;
}


/*
	SHELL INTERACTION METHODS
	=========================
*/


void init()
{
	/* reset system */

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
}


void ls(void)
{
	/* list entries starting from the root directory */

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


void mkdir(char *dirname) 
{
	/* create a new DIR ENTRY over an existing one  */

	int block_exists = 1;

	if (strstr(dirname, "/") != NULL)
	{
		block_exists = iter_dirs(dirname);
		dirname = actual_dir.filename_to_save;
	}

	if (block_exists)
	{
		struct dir_entry_s dir_entry;

		int16_t first_block = fat_free();
		int32_t entry = dir_free(actual_dir.block, actual_dir.dirname);
		
		if (entry >= 0) 
		{
			memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
			strcpy((char *)dir_entry.filename, dirname);
			dir_entry.attributes = 0x02;
			dir_entry.first_block = first_block;
			dir_entry.size = 0;
			write_dir_entry(actual_dir.block, entry, &dir_entry);

			actual_dir.filename_to_save = "";

			fat[first_block] = 0x7ffd;
			write_fat("filesystem.dat", fat);
		}
		else 
		{
			printf("> you cannot enter existing directory name\n");
		}	
	}
}


void create(char *filename)
{
	/* create a new file in a DIR ENTRY */

	int block_exists = 1;

	if (strstr(filename, "/") != NULL)
	{
		block_exists = iter_dirs(filename);
		filename = actual_dir.filename_to_save;
	}

	if (block_exists) 
	{
		struct dir_entry_s dir_entry;

		int16_t first_block = fat_free();
		int32_t entry = dir_free(actual_dir.block, actual_dir.dirname);

		if (entry >= 0) 
		{
			memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
			strcpy((char *)dir_entry.filename, filename);
			dir_entry.attributes = 0x01;
			dir_entry.first_block = first_block;
			dir_entry.size = 0;
			write_dir_entry(actual_dir.block, entry, &dir_entry);

			fat[first_block] = 0x7fff;
			write_fat("filesystem.dat", fat);
		}
		else 
		{
			printf("> you cannot enter existing filename\n");
		}
	}
}

void cd(char *path)
{
	if (strstr(path, "/") != NULL)
	{
		strcat(path, ".");
		int dir_exists = iter_dirs(path);
		if (dir_exists == 1) {
			printf("> now on '%s'\n", actual_dir.dirname);
		}
	}
	else if (strstr(path, "..") != NULL)
	{
		actual_dir.dirname = "root";
		actual_dir.block = ROOT_BLOCK;
		printf("> now on '%s'\n", actual_dir.dirname);
	}
	else
	{
		strcat(path, "/.");
		int dir_exists = iter_dirs(path);
		if (dir_exists == 1) {
			printf("> now on '%s'\n", actual_dir.dirname);
		}
	}
}


/*
	EXECUTION
	=========
*/


int main(int argc, char *argv[])
{
	char f_command [10]; // obrigartory first command in runtime (e.g. ${init} )
	char s_command [50]; // optional command in runtime (e.g. $create {filename} )

	int fat_in_memory = 0; // validate initialization
	char *init_error_message = "> please, use $init or $load first\n";

	struct dir_entry_s dir_entry;

	while (1) 
	{
		printf("\n$ ");
		scanf("%s", f_command);

		// INIT
		if (strstr(f_command, "init")) 
		{
			init();
			fat_in_memory = 1;
		}

		//LOAD
		else if (strstr(f_command , "load")) 
		{
			read_fat("filesystem.dat", fat);
			actual_dir.block = ROOT_BLOCK;
			actual_dir.dirname = "root";
			fat_in_memory = 1;
		}

		// LS
		else if (strstr(f_command, "ls")) 
		{
			if (!fat_in_memory) 
			{
				printf("%s", init_error_message);
			}
			else 
			{
				ls();
			}
		}

		// MKDIR
		else if (strstr(f_command, "mkdir")) 
		{
			if (!fat_in_memory) 
			{
				printf("%s", init_error_message);
			}
			else 
			{
				scanf("%s", s_command);
				mkdir(s_command);
			}
		}

		// CREATE
		else if (strstr(f_command, "create")) 
		{
			if (!fat_in_memory) 
			{
				printf("%s", init_error_message);
			} 
			else 
			{
				scanf("%s", s_command);
				create(s_command);
			}
		}

		// UNLINK
		else if (strstr(f_command, "unlink")) 
		{
			if (!fat_in_memory) 
			{
				printf("%s", init_error_message);
			}
			else 
			{
				printf("UNLINK");
			}
		}

		// WRITE
		else if (strstr(f_command, "write")) 
		{
			if (!fat_in_memory) 
			{
				printf("%s", init_error_message);
			}
			else 
			{
				printf("WRITE"); // s_command
			}
		}

		// APPEND
		else if (strstr(f_command, "append")) 
		{
			if (!fat_in_memory) 
			{
				printf("%s", init_error_message);
			}
			else 
			{
				printf("APPEND"); // s_command
			}
		}


		// READ
		else if (strstr(f_command, "read")) 
		{
			if (!fat_in_memory) 
			{
				printf("%s", init_error_message);
			}
			else 
			{
				printf("READ");
			}
		}

		// CD
		else if (strstr(f_command, "cd")) 
		{
			if (!fat_in_memory) 
			{
				printf("%s", init_error_message);
			}
			else 
			{
				scanf("%s", s_command);
				cd(s_command);
			}
		}
		
		// EXIT
		else if (strstr(f_command, "exit")) 
		{
			printf("Bye!\n");
			break;
		}

		// DEFAULT
		else 
		{
			printf("> command '%s' doent exist\n", f_command);
		}
	}

	return 0;
}
