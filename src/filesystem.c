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
#include <shell.h>



/*

	SYSTEM EXECUTION

*/


int main(int argc, char *argv[])
{
	char f_command [10]; // obrigartory first command in runtime (e.g. ${init} )
	char s_command [50]; // optional command in runtime (e.g. $create {filename} )

	int fat_in_memory = 0; // validate initialization
	char *init_error_message = "> please, use $init or $load first\n";

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
			load();
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
				scanf("%s", s_command);
				unlink(s_command);
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
			printf("> command '%s' doesn't exist\n", f_command);
		}
	}

	return 0;
}
