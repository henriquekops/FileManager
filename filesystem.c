#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE		1024
#define BLOCKS			2048
#define FAT_SIZE		(BLOCKS * 2)
#define FAT_BLOCKS		(FAT_SIZE / BLOCK_SIZE)
#define ROOT_BLOCK		FAT_BLOCKS
#define DIR_ENTRY_SIZE		32
#define DIR_ENTRIES		(BLOCK_SIZE / DIR_ENTRY_SIZE)

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
	AUXILIARY METHODS
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
	SHELL INTERACTION METHODS
*/

int init_disk(void)
{
	/*initialize disk*/
	remove("filesystem.dat");
	FILE *f;
	f = fopen("filesystem.dat", "a");
	fclose(f);
}


void init_fat(void)
{
	/* initialize fat */
	int32_t i;

	for (i = 0; i < FAT_BLOCKS; i++) {
		fat[i] = 0x7ffe;
	}

	fat[ROOT_BLOCK] = 0x7fff;

	for (i = ROOT_BLOCK + 1; i < BLOCKS; i++) {
		fat[i] = 0;
	}

	write_fat("filesystem.dat", fat);
}


void init_data_blocks(void)
{
	/* initialize data blocks */
	int32_t i;

	for (i = 0; i < BLOCK_SIZE; i++)
	{
		data_block[i] = 0;
	}

	write_block("filesystem.dat", ROOT_BLOCK, data_block);

	for (i = ROOT_BLOCK + 1; i < BLOCKS; i++)
	{
		write_block("filesystem.dat", i, data_block);
	}
}


void ls(struct dir_entry_s dir_entry)
{
	/* list entries from the root directory */
	int32_t i;

	for (i = 0; i < DIR_ENTRIES; i++) {
		read_dir_entry(ROOT_BLOCK, i, &dir_entry);
		printf("Entry %d, file: %s attr: %d first: %d size: %d\n", i, dir_entry.filename, dir_entry.attributes, dir_entry.first_block, dir_entry.size);
	}
}


void create(char* filename, struct dir_entry_s dir_entry)
{
	// create logic reading fat to gather below info
	// int16_t first_block, int32_t size, int block, int entry
	// int16_t* fat;
	// read_fat("filesystem.dat", fat);
	memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
	strcpy((char *)dir_entry.filename, filename);
	dir_entry.attributes = 0x01;
	dir_entry.first_block = 111; //change to first_block
	dir_entry.size = 444; //change to size
	write_dir_entry(ROOT_BLOCK/*block*/, 1/*entry*/, &dir_entry);
}

/*
memset((char *)dir_entry.filename, 0, sizeof(struct dir_entry_s));
strcpy((char *)dir_entry.filename, "file1");
dir_entry.attributes = 0x01;
dir_entry.first_block = 3333;
dir_entry.size = 444;
write_dir_entry(ROOT_BLOCK, 2, &dir_entry);
*/


/*
	EXECUTION
*/

int main(int argc, char *argv[])
{

	int running = 1;
	char* command;

	struct dir_entry_s dir_entry;

	while (running) {
		printf("\nCommand: ");
		gets(command);

		if (strstr(command, "init")) {
			init_disk();
			init_fat();
			init_data_blocks();
		}
		else if (strstr(command , "load")) {
			printf("LOAD");
		}
		else if (strstr(command, "ls")) {
			ls(dir_entry);
		}
		else if (strstr(command, "mkdir")) {
			printf("MKDIR");
		}
		else if (strstr(command, "create")) {
			printf("CREATE");
			// create(filename, dir_entry);
		}
		else if (strstr(command, "unlink")) {
			printf("UNLINK");
		}
		else if (strstr(command, "write")) {
			printf("WRITE"); // command +1
		}
		else if (strstr(command, "append")) {
			printf("APPEND"); // command + 1
		}
		else if (strstr(command, "read")) {
			printf("READ");
		}
		else if (strstr(command, "exit")) {
			running = 0;
			printf("BYE");
		}
		else {
			printf("NOT FOUND!");
		}
	}
	return 0;
}
