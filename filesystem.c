#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE		1024 							// size of a data block
#define BLOCKS			2048 							// quantity of blocks
#define FAT_SIZE		(BLOCKS * 2) 					// size of fat (4096 = 2048 * 16 bits - HEX)
#define FAT_BLOCKS		(FAT_SIZE / BLOCK_SIZE) 		// quantity of fat blocks (4)
#define ROOT_BLOCK		FAT_BLOCKS 						// position of root block (5)
#define DIR_ENTRY_SIZE		32 							// size of dir entries
#define DIR_ENTRIES		(BLOCK_SIZE / DIR_ENTRY_SIZE) 	// quantity of dir entries

int16_t fat[BLOCKS];
int8_t data_block[BLOCK_SIZE];


/*
	STRUCTS
*/


struct dir_entry_s {
	int8_t filename[25];
	int8_t attributes;
	int16_t first_block;
	int32_t size;
};


/*
	DEFAULT AUXILIARY METHODS
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
	/* reads a directory entry from a directory */
	read_block("filesystem.dat", block, data_block);
	memcpy(dir_entry, &data_block[entry * sizeof(struct dir_entry_s)], sizeof(struct dir_entry_s));
}


void write_dir_entry(int block, int entry, struct dir_entry_s *dir_entry)
{
	/* writes a directory entry in a directory */
	read_block("filesystem.dat", block, data_block);
	memcpy(&data_block[entry * sizeof(struct dir_entry_s)], dir_entry, sizeof(struct dir_entry_s));
	write_block("filesystem.dat", block, data_block);
}


/*
	SHELL AUXILIARY METHODS
*/


int16_t fat_free(int16_t* fat) {
	int16_t first_block;
	for (first_block = ROOT_BLOCK + 1; first_block < BLOCKS; first_block++) {
		if (fat[first_block] == 0) {
			break;
		}
	}
	return first_block;
}


int32_t dir_free(char* filename, struct dir_entry_s dir_entry) {
	int32_t entry;
	for (entry = 0; entry < DIR_ENTRY_SIZE; entry++) {
		read_dir_entry(ROOT_BLOCK, entry, &dir_entry);
		if (!strcmp((char *)&dir_entry.filename[0], filename)) {
			entry = -1;
			break;
		}
		if (dir_entry.attributes == 0) {
			break;
		}
	}
	return entry;
}


/*
	SHELL INTERACTION METHODS
*/


int16_t* init(void)
{

	/* reinitialize disk */
	remove("filesystem.dat");
	
	FILE *f;
	
	f = fopen("filesystem.dat", "a");
	fclose(f);

	/* initialize fat */
	int32_t fat_itr;

	for (fat_itr = 0; fat_itr < FAT_BLOCKS; fat_itr++) {
		fat[fat_itr] = 0x7ffe;
	}

	fat[ROOT_BLOCK] = 0x7fff;

	for (fat_itr = ROOT_BLOCK + 1; fat_itr < BLOCKS; fat_itr++) {
		fat[fat_itr] = 0;
	}

	write_fat("filesystem.dat", fat);

	/* initialize data blocks */
	int32_t block_itr;

	for (block_itr = 0; block_itr < BLOCK_SIZE; block_itr++)
	{
		data_block[block_itr] = 0;
	}

	write_block("filesystem.dat", ROOT_BLOCK, data_block);

	for (block_itr = ROOT_BLOCK + 1; block_itr < BLOCKS; block_itr++)
	{
		write_block("filesystem.dat", block_itr, data_block);
	}

	return fat;
}


void ls(struct dir_entry_s dir_entry)
{
	/* list entries from the root directory */
	int32_t i;

	printf("ENTRY FILE ATTR FIRST SIZE\n");
	for (i = 0; i < DIR_ENTRIES; i++) {
		read_dir_entry(ROOT_BLOCK, i, &dir_entry);
		printf("%d %s %d %d %d\n", 
			i, dir_entry.filename, dir_entry.attributes, dir_entry.first_block, dir_entry.size);
	}
}


void create(char* filename, struct dir_entry_s dir_entry, int16_t* fat)
{
	int16_t first_block = fat_free(fat);
	int32_t entry = dir_free(filename, dir_entry);

	if (entry >= 0) {
		memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
		strcpy((char *)dir_entry.filename, filename);
		dir_entry.attributes = 0x01;
		dir_entry.first_block = first_block;
		dir_entry.size = 0;
		write_dir_entry(ROOT_BLOCK, entry, &dir_entry);
	}
	else {
		printf("You cannot enter existing filename!");
	}
}


/*
	EXECUTION
*/


int main(int argc, char *argv[])
{
	int running = 1;
	char* f_command;
	char* s_command;
	int16_t* fat;

	struct dir_entry_s dir_entry;

	while (running) {
		printf("\n$ ");
		scanf("%s", f_command);

		if (strstr(f_command, "init")) {
			fat = init();
		}
		else if (strstr(f_command , "load")) {
			printf("LOAD");
		}
		else if (strstr(f_command, "ls")) {
			ls(dir_entry);
		}
		else if (strstr(f_command, "mkdir")) {
			printf("MKDIR");
		}
		else if (strstr(f_command, "create")) {
			scanf("%s", s_command);
			create(s_command, dir_entry, fat);
		}
		else if (strstr(f_command, "unlink")) {
			printf("UNLINK");
		}
		else if (strstr(f_command, "write")) {
			printf("WRITE"); // s_command
		}
		else if (strstr(f_command, "append")) {
			printf("APPEND"); // s_command
		}
		else if (strstr(f_command, "read")) {
			printf("READ");
		}
		else if (strstr(f_command, "exit")) {
			break;
		}
		else {
			printf("NOT FOUND!");
		}
	}
	printf("Bye!\n");
	return 0;
}
