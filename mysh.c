/*
 *Author: Zach Showalter
 */
#define _DEFAULT_SOURCE
#include "trimit.h"
#include "cmd.h"
//#include "arg.h"
//#include "util.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

typedef struct history_s{
	cmd_p * cmdlst;
	int size;//size of the cmd list
	int length;//amount of cmd's in the cmd list
	int cur_pos;
	int beginning;
	int end;
}history_struct;
typedef struct history_s * history_p; // typedef for history struct pointer
/*
 *Description: creates history object
 *Param:n, size of history
 *Return: pointer to history struct
 */
history_p create_history(int n){
	history_p temp = malloc(sizeof(history_struct));
	cmd_p* cmdlst;
	if(n == 0){
		cmdlst = malloc(10*sizeof(cmd_p)); //allocate space for the list
		//		for(int i = 0; i <= 10; i++){
		//			cmdlst[i] = malloc(sizeof(cmd_p)); 	// allocate space in the list for
		//each cmd pointer.
		//		}
		temp->size = 10;
		temp->length = 0;//finish building history struct pointer
		temp->cmdlst = cmdlst;
		temp->cur_pos = 0;
		temp->beginning = 0;
		temp->end = temp->size;
	}
	else{
		cmdlst = malloc(n*sizeof(cmd_p));
		for(int i = 0; i <= n; i++){
			cmdlst[i] = malloc(sizeof(cmd_p));
		}
		temp->size = n;
		temp->length = 0;
		temp->cmdlst = cmdlst;
		temp->cur_pos =0;
		temp->beginning = 0;
		temp->end = temp->size;
	}
	return temp;

}
static history_p history;
/*
 *Description: add cmd object to history
 *Param:history to add cmd object to, cmd object to add
 *Return:none
 */
void add_to_history(history_p history, cmd_p cmd){
	cmd_p newcmd = create_cmd(cmd->cmd, cmd->args, cmd->cmd_num);
	if(history->length == history->size){
		free_cmd(history->cmdlst[(cmd->cmd_num-1)%(history->size)]); //changed way of adding to
		//history, may not need len
		//attribute.
		history->cmdlst[(cmd->cmd_num-1)%(history->size)] = newcmd;
		history->cur_pos = (cmd->cmd_num-1)%(history->size);
		history->end++;
		history->beginning++;
	}
	else{
		history->cmdlst[(cmd->cmd_num-1)%(history->size)] = newcmd;
		history->cur_pos = (cmd->cmd_num-1)%(history->size);
		history->length++;
	}
}
/*
 *Description:frees history objecy
 *Param:history object to free
 *Return:none
 */
void free_history(history_p history){
	for(int i = 0; i< history->length; i++){
		if(history->cmdlst[i] != NULL){
			free_cmd(history->cmdlst[i]);
		}
		//	free(history->cmdlst[i]);
	}
	free(history->cmdlst);
	free(history);
}/*
 *Description: prints the program history
 *Param: none used
 *Return: int to indicate correct execution
 */
static int mysh_history(int argc, char *argv[]){
	(void)argc;
	(void)argv;
	for(int i = history->cur_pos+1; i < history->length; i++){
		printf("\t%i: ", history->cmdlst[i]->cmd_num);
		print_cmd(history->cmdlst[i]);
	}	
	for(int i = 0; i <= history->cur_pos; i++){
		printf("\t%i: ", history->cmdlst[i]->cmd_num);
		print_cmd(history->cmdlst[i]);
	}
	return 0;	
}
/*
 *Description:quits the program and cleans mem
 *Param:none used
 *Return:int if executed correctly
 */
static int mysh_quit(int argc, char * argv[]){
	(void)argc;
	(void)argv;
	free_history(history);
	return 0;
}
/*
 *Description: finds a command in the history list based off of its number in the history
 *Param: int n position of cmd in history list
 *Return:cmd pointer to the command
 */
static cmd_p find_cmd(int n){
	if((n < history->beginning) || (n> history->end)){
		fprintf(stderr, "not a valid command number\n");
		return NULL;
	}
	else{
		return history->cmdlst[(n%history->size)-1];
	}
}

int verbose = 0;
/*
 *Description:toggles verbose mode on or off based on input.
 *Param: argv used to decide to turn verbose on or off
 *Return:int to indicate if verbose is on or off
 */
static int mysh_verbose(int argc, char *argv[]){
	(void)argc;
	if(strcmp(argv[0], "on") ==0){
		verbose = 1;
		return 1;
	}
	else if(strcmp(argv[0], "off") ==0){
		verbose = 0;
		return 0;
	}
	return 0;
}
/*
*Description: handles printing of a command when verbose mode is on
*Param:cmd command to priint in verbose mode
*Return:none
*/
void verbose_print(cmd_p cmd){
	int cmdtk = 0;
	printf("\tinput command tokens:\n");
	printf("\t%i: %s\n", cmdtk, cmd->cmd);
	cmdtk++;
	for(int i = 0; i<cmd->args->length; i++){
		printf("\t%i: %s\n", cmdtk, cmd->args->args[i]);
		cmdtk++;
	}
}
/*
 *Description: prints the prompt for the user to type with command number
 *Param:cmd_num, number of command this input will be
 *Return: none
 */
void print_prompt(int cmd_num){
	fprintf(stdout, "mysh[%i]> ", cmd_num);
}
static int mysh_bang(int argc, char* argv[]);
/*
 *Description: prints a useful help message
 *Param: int argc, not used, and char* argvp[] not used
 *Return: 0 to indicate proper function
 */
static int mysh_help(int argc, char* argv[]){
	(void)argc;
	(void)argv;
	printf("\t!N: Re-execute the Nth command in the history list\n");
	printf("\thelp: Print a list of internal commands.\n");
	printf("\thistory: print a list of the commands executed so far, including their arguments\n");
	printf("\tquit: Clean up memory, and exit the shell\n");
	printf("\tverbose on/off: turn the shell verbose on/off based on argument. When on");
	printf(" verbose mode will print the last command entered and its components.\n");
	return 0;
}
/*
 *Description:Executes an internal command
 *Param:cmd_p command to execute
 *Return: an int based on which command was executed, or -1 if the command given was not internal
 */
static int execute_internal(cmd_p cmd){
	int retval = 0;
	if(strcmp(cmd->cmd, "history")==0){
		mysh_history(1,NULL);
		retval = 1;
	}
	else if(strcmp(cmd->cmd, "quit")==0){
		mysh_quit(1,NULL);
		retval = 2;
	}
	else if(strcmp(cmd->cmd, "!") == 0){
		retval = mysh_bang(1, cmd->args->args);
		retval = 1;
	}
	else if(strcmp(cmd->cmd, "verbose") == 0){
		retval = mysh_verbose(1, cmd->args->args);
		retval = 3;
	}
	else if(strcmp(cmd->cmd, "help") == 0){
		mysh_help(1, NULL);
		retval = 4;
	}
	else{
		retval = -1;
	}
	return retval;
}
/*
 *Description: repeats the nth command in internal history
 *Param: argc and argv, argc not used, argv used to determine which command to execute
 *Return: returns num of command executed.
 */
static int mysh_bang(int argc, char* argv[]){
	(void)argc;
	long num = (int)strtol((argv[0])+1, &argv[0], 10);
	cmd_p bang = find_cmd((int)num);
	if(bang ==NULL){
		return 0;
	}
	execute_internal(bang);
	return num;
}
/*
 *Description: main driver of the shell that handles all input and printing/executing
 *Param: argc and argv the command line inputs and num of cmd line inputs
 *Return:int representing exit status
 */
int main(int argc, char * argv[]){
	pid_t pid;
	(void)pid;
	int status;
	int opt;
	char *input = NULL;
	size_t bytes = 0;
	int cmd_num = 1;
	register char *token;
	char * cmd;
	char * bangcmd = NULL;
	int histnum = 0;
	arg_p args;
	arg_p bangargs;
	cmd_p temp;
	cmd_p bang;
	int retval;
	print_prompt(cmd_num);
	while((opt = getopt(argc, argv, "vh:")) != -1){
		switch (opt){
			case 'v':
				verbose = 1;
				break;
			case 'h':
				histnum = (int)strtol(optarg, NULL, 10);	

		}
	}
	history = create_history(histnum);
	while(getline(&input, &bytes, stdin)!= -1){
		args = create_arg_p();
		bangargs = create_arg_p();
		token = strtok(input, " ");
		cmd = trim(token);
		add_to_arg_array(args, cmd);
		while((token = strtok(NULL," ")) != NULL){
			char * temp = strdup(trim(token));
			add_to_arg_array(args, temp);
		}
		if(strncmp(cmd, "!", 1) == 0){
			//	big = strtol(cmd+1, NULL, 10);
			//	sprintf( number, "%ld", big);
			add_to_arg_array(args, cmd);
			bangcmd = cmd;
			cmd = "!"; 	

		}
		temp = create_cmd(cmd, args, cmd_num);
		//print_cmd(temp);	
		if(strcmp(cmd, "!") ==0){
			bang = create_cmd(bangcmd, bangargs, cmd_num);
			if(verbose){
				fprintf(stdout, "command: ");	
				print_cmd(bang);
				verbose_print(bang);
			}
			add_to_history(history, bang);
			free_cmd(bang);
		}
		else{
			if(verbose){
				fprintf(stdout, "command: ");
				print_cmd(temp);
				verbose_print(temp);
			}
			add_to_history(history, temp);
		}
		retval = execute_internal(temp);
		if(retval == 2){
			free_args(args);
			free_cmd(temp);
			printf("\n");
			break;
		}
		if(retval == -1){
			nterm_arg_array(temp->args);
			int result=0;
			pid = fork();
			switch(pid){
				case -1:
					perror( "fork" );
					exit(EXIT_FAILURE);
				case 0:
					if(verbose){
						verbose_print(temp);
					}
					result = execvp(temp->cmd, temp->args->args);
					if(result == -1){
						printf(" %s: %s\n", temp->cmd, strerror(errno));
					}
					break;
				default:
					pid = wait(&status);
					if(WIFEXITED(status) && verbose){
						printf("Child %d terminated, status%d\n",
								pid, WEXITSTATUS(status));
					}
			}

		}
		cmd_num++;
		print_prompt(cmd_num);
		//free_args(args);
		if(args != NULL){
			free_cmd(temp);
		}
	}
	//	free(input);
	printf("\n");
	return 0;
}
