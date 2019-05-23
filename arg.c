#define _DEFAULT_SOURCE
#include "arg.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
arg_p create_arg_p(){
	arg_p temp = malloc(sizeof(arg_array_struct));
	//        char ** lst = malloc(2*sizeof(char*));
	temp->args = malloc(2*sizeof(char*));
	temp->length = 0;
	temp->size = 2;
	return temp;
}
void nterm_arg_array(arg_p args){
	add_to_arg_array(args, NULL);
}
void add_to_arg_array(arg_p args, char * arg){
	if(args->length == args->size){
		args->size = args->size*2;
		args->args = realloc(args->args, args->size*sizeof(char*));
		args->args[args->length] = arg;
		args->length++;
	}
	else{
		args->args[args->length] = arg;
		args->length++;
	}
}
arg_p copy_arg(arg_p args){
	arg_p temp = create_arg_p();
	char* temps;
	for(int i = 0; i< args->length; i++){
		temps = strdup(args->args[i]);
		add_to_arg_array(temp, temps);
	}
	//free_args(args);
	//	if(temps != NULL){
	//		free(temps);
	//	}
	return temp;

}
void free_args(arg_p args){
	if(args != NULL){
		for(int i = 0; i<args->length-1; i++){
			//if(args->args[i] != NULL){
				if(strcmp(args->args[i], "") != 0 ){
					free(args->args[i]);
				}
			//}
		}
		free(args->args);
		free(args);
	}
}
