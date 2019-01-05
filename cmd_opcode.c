#include "20121578.h"

void opcode_init() {
	FILE *op_txt = NULL;
	int op_num_reader;
	char op_cmd_reader[8];
	char op_mode_reader[8];
	int i, op_cmd_len, op_table_idx;
	opcode_ptr new_node = NULL;

	op_txt = fopen("opcode.txt", "r"); // open opcode.txt file
	if (op_txt == NULL) {
		printf("File open error!\n");
		exit(-1);
	}
	while (fscanf(op_txt,"%x %s %s",&op_num_reader,op_cmd_reader,op_mode_reader) != EOF) {
		op_table_idx = 0;
		op_cmd_len = strlen(op_cmd_reader);
		for (i = 0; i < op_cmd_len;i++) {
			op_table_idx += op_cmd_reader[i];
		}
		op_table_idx %= 20;
		/* 
		   the index of mnemonic is the mod of mnemonic's letters' ASCII code's sum.
		*/
		new_node = (opcode_ptr)malloc(sizeof(opcode_node));
		if (new_node == NULL) {
			printf("Memory Allocation Error!\n");
			exit(-1);
		}
		new_node->op_num = op_num_reader;
		strcpy(new_node->op_chr,op_cmd_reader);
		strcpy(new_node->op_mode,op_mode_reader);
		new_node->link = NULL;
		// create the node in hash table

		if (opcode_table[op_table_idx] == NULL) {
			opcode_table[op_table_idx] = new_node;
		}
		else {
			new_node->link = opcode_table[op_table_idx];
			opcode_table[op_table_idx] = new_node;
		}
		// add the node in hash table
	}

	fclose(op_txt);
}

void opcode_table_print() {
	int i;
	opcode_ptr temp_node;

	for (i = 0; i < HASH_SIZE;i++) {
		printf("%d : ",i);
		if (opcode_table[i] == NULL) {
			printf("\n");
			continue;
		}
		temp_node = opcode_table[i];
		while (temp_node->link != NULL) {
			printf("[%s, %02X] -> ",temp_node->op_chr,temp_node->op_num);
			temp_node = temp_node->link;
		}
		printf("[%s, %02X]\n", temp_node->op_chr, temp_node->op_num);
	}
	// print opcode hash table
}

int opcode_find(char *op_mnec, int mnec_len) {
	int i;
	int op_idx = 0;
	opcode_ptr temp_node;

	for (i = 0; i < mnec_len;i++) {
		op_idx += op_mnec[i];
	}
	op_idx %= 20;
	// calculate the index

	temp_node = opcode_table[op_idx];
	while (temp_node != NULL) {
		if (!strcmp(temp_node->op_chr,op_mnec)) {
			printf("opcode is %02X\n",temp_node->op_num);
			return 0;
		}
		temp_node = temp_node->link;
	}
	// searching entered mnemonic and if found, print the opcode.

	err_msg_bundle(WRONG_MNEMONIC);
	return -1;
	// if cannot found, return -1.
}

void opcode_free() {
	int i;
	opcode_ptr de_node, temp_node;

	for (i = 0; i < HASH_SIZE;i++) {
		temp_node = opcode_table[i];
		de_node = opcode_table[i];
		while (temp_node != NULL) {
			temp_node = temp_node->link;
			free(de_node);
			de_node = temp_node;
		}
	}
	// deallocate mnemonic hash table
}

void history_free() {
	history_ptr de_node, temp_node;

	de_node = his_list_front;
	temp_node = his_list_front;
	while (temp_node != NULL) {
		temp_node = temp_node->link;
		free(de_node);
		de_node = temp_node;
	}
	// deallocate command history list
}
