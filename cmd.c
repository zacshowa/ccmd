#define _DEFAULT_SOURCE
#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
cmd_p create_cmd(char *cmd, arg_p args, int num){//constructor for cmd pointer
        cmd_p temp = malloc(sizeof(cmd_struct));
	temp->cmd = malloc(strlen(cmd)+1);
        temp->cmd = strcpy(temp->cmd, cmd);
	//1free(cmd);
        temp->args = copy_arg(args);
	temp->cmd_num = num;
        return temp;
}
void free_cmd(cmd_p cmd){
        free_args(cmd->args);
       // free(cmd->args);
	free(cmd->cmd);
	free(cmd);
}
void print_cmd(cmd_p cmd){
        printf("%s", cmd->cmd);
        for(int i=1; i<=cmd->args->length-1; i++){
                printf(" %s", cmd->args->args[i]);
        }
        printf("\n");
}
