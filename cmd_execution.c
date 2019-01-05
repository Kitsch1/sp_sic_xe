#include "20121578.h"

void sic_help() {
	printf("\nh[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start, end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n\n");
} // h[elp]

void sic_history_print() {
	int his_num = 1;
	history_ptr temp_his = his_list_front;

	while (temp_his != NULL) {
		printf("%d %s\n",his_num,temp_his->his_mem);
		his_num++;
		temp_his = temp_his->link;
	} // print command history list
}

void history_add(char *history_input) {
	history_ptr new_history = NULL;

	new_history = (history_ptr)malloc(sizeof(history_node));
	if (new_history == NULL) {
		printf("Memory Allocation Error!\n");
		exit(-1);
	}
	strcpy(new_history->his_mem,history_input);
	new_history->link = NULL;
	// make new history node

	if (his_list_front == NULL) {
		his_list_front = his_list_rear = new_history;
	} // first history
	else {
		his_list_rear->link = new_history;
		his_list_rear = new_history;
	} // the others
}

int sicType(char *type_fname) {
	FILE *type_fp = NULL;
	char res;

	type_fp = fopen(type_fname,"r");
	if (type_fp == NULL) {
		err_msg_bundle(NO_FILE_EXIST);
		return -1;
	}
	while (1) {
		res = fgetc(type_fp);
		if (res == EOF) {
			break;
		}
		printf("%c",res);
	}

	return 1;
}

int sic_dir() {
	DIR *cur_dir = NULL; // read the directory's name
	struct dirent *f_point = NULL;
	struct stat s_buf; // save the directory's status

	cur_dir = opendir("."); // open current directory
	if(!cur_dir){
		printf("ERROR : Cannot Open Current Directory\n");
		return -1;
	}
	while((f_point = readdir(cur_dir)) != NULL){ // read the directory
		lstat(f_point->d_name,&s_buf); // read the status of directory
		if(S_ISDIR(s_buf.st_mode)){
			printf("%s/\t",f_point->d_name);
		} // S_ISDIR : the function that check if status is directory
		else if((S_IEXEC & s_buf.st_mode) != 0){
			printf("%s*\t",f_point->d_name);
		} // S_ISEXEC : constant that check if status is execute file
		else{
			printf("%s\t",f_point->d_name);
		} // others
	}
	printf("\n");
	closedir(cur_dir); // close the current directory

	return 1;
}

