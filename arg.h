/*
 *Typedef for arg_array struct
 */
typedef struct arg_array_S{// typedef for arg array struct for use in cmd struct
        int size;
        int length;
        char **args;
}arg_array_struct;
/*
 *typedef for pointer to arg array
 */
typedef struct arg_array_S * arg_p;// typedef for arg array pointer
/*
 *Description: creates an arg array pointer
 *Param: None
 *Return: arg_p that points to the created arg array
 */
arg_p create_arg_p();
/*
 *Description: null terminates an arg array for use with exec functions.
 *
 */
void nterm_arg_array(arg_p args);
/*
 *Description: add an arg pointer to an arg array
 *Param:arg_p args: arg array to add arg to, arg: arg to add to array.
 *Return:None
 */
void add_to_arg_array(arg_p args, char * arg);
/*
 *Description: make a deep copy of an arg pointer
 *Param:arg_p args: arg pointer to make a copy of
 *Return:
 */
arg_p copy_arg(arg_p args);
/*
 *Description: frees an arg pointer and all of its necessary members
 *Param: arg_p args: arg pointer to free
 *Return: none
 */
void free_args(arg_p args);
