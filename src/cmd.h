#include "arg.h"
typedef struct cmd_S{// typedef for cmd struct
        char * cmd;
        arg_p args;
        int cmd_num;
}cmd_struct;
typedef struct cmd_S * cmd_p;//typedef for cmd pointer
/*
 *function that creates cmd structure and returns.
 * with deep copies of everything that is contained in the struct.
 */
cmd_p create_cmd(char *cmd, arg_p args, int num);
/*
 *Description: frees a cmd struct
 *Param: cmd_p cmd: the cmd struct pointer to free
 *Return: none
 */
void free_cmd(cmd_p cmd);
/*
 *Description: prints a cmd struct properly
 *Param: cmd_p cmd: pointer to the cmd struct to print
 *Return: none
 */
void print_cmd(cmd_p cmd);
