/*
    Authors: 
        Carlos S. Castro,
        Gabriel C. Silva,
        Henrique R. Kops
*/

// built-in dependencies
#include <stdio.h>
#include <string.h>

// project dependencies
#include <manipulators.h>
#include <definitions.h>
#include <auxiliary.h>
#include <shell.h>

#define HELP_STRING "Commands: \n"\
	"\tExample:\n"\
	"\t\t$ {command}: {explanation} {accepts path}\n\n"\
	"\tDefinitions: \n"\
	"\t\t$ init\t\t:\tReset system\t\t\t\t0\n"\
	"\t\t$ load\t\t:\tLoad system data from filesystem.dat\t0\n"\
	"\t\t$ ls\t\t:\tList current directory entries\t\t0\n"\
	"\t\t$ mkdir\t\t:\tCreate a directory entry\t\t1\n"\
	"\t\t$ create\t:\tCreate a file in a directory\t\t1\n"\
	"\t\t$ unlink\t:\tDelete a file or directory\t\t1\n"\
	"\t\t$ read\t\t:\tRead a file's content\t\t\t1\n"\
	"\t\t$ write\t\t:\tWrite content to a file\t\t\t1\n"\
	"\t\t$ append\t:\tAppend content to a file\t\t0\n\n"\
	"\tSystem calls: \n"\
	"\t\t$ init\n"\
	"\t\t$ load\n"\
	"\t\t$ ls\n"\
	"\t\t$ mkdir {path/to/directory/new_dir}\n"\
	"\t\t$ create {path/to/directory/new_file}\n"\
	"\t\t$ unlink {path/to/directory_or_file}\n"\
	"\t\t$ read {path/to/file}\n"\
	"\t\t$ write {path/to/file}\n"\
	"\t\t$ append not coded yet...\n"


/*

	SYSTEM UI

*/


/* system execution */
int main(int argc, char *argv[])
{
	char f_command [COMMAND_ENTRY];		// obrigartory command in runtime	(e.g.	${init} )
	char s_command [PATH_ENTRY];		// optional path field				(e.g.	$create {path} )
	char t_command [CONTENT_ENTRY];		// optional file content			( e.g.	$write path {content} )

	/* validate initialization */
	int fat_in_memory = 0;
	char *init_error_message = "> please, use $init or $load first\n";

	cls_screen();

	printf("WELCOME TO YOUR FILESYSTEM!\n");
	printf("If you need any help, type 'help'\n");

	while (1) 
	{
		printf("\n$ ");
		scanf("%s", f_command);

		cls_screen();

		// INIT
		if (strstr(f_command, "init")) 
		{
			minit();
			fat_in_memory = 1;
		}

		//LOAD
		else if (strstr(f_command , "load")) 
		{
			int loaded = mload();
			if (loaded)
			{
				fat_in_memory = 1;
			}
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
				mls();
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
				mmkdir(s_command);
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
				mcreate(s_command);
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
				scanf("%s", s_command);
				munlink(s_command);
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
				scanf("%s", s_command);
				mread(s_command);
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
				scanf("%s", s_command);
				fgets(t_command, CONTENT_ENTRY, stdin);
				mwrite(s_command, t_command);
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
				mcd(s_command);
			}
		}

		// HELP
		else if (strstr(f_command, "help"))
		{
			printf(HELP_STRING);
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
			printf("> '%s' is not a command\n", f_command);
		}
	}

	return 0;
}
