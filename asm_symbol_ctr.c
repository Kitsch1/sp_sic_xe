#include "20121578.h"

int opSearch(char *op_str) {
	int i;
	int op_idx = 0;
	int op_len = strlen(op_str);
	opcode_ptr search_op = NULL;

	for (i = 0; i < op_len;i++) {
		op_idx += op_str[i];
	}
	op_idx %= 20;
	search_op = opcode_table[op_idx];
	// calculate the index of opcode hash table and initialize
	while (search_op != NULL) {
		if (!strcmp(op_str,search_op->op_chr)) {
			if (!strcmp(search_op->op_mode,"1")) {
				return 1;
			} // format 1
			else if (!strcmp(search_op->op_mode,"2")) {
				return 2;
			} // format 2
			else {
				return 3;
			} // format 3 or 4
		} // return the mode of mnemonic
		search_op = search_op->link;
	}

	return -1;
	// if there is no mnemonic
}

int opcodeNumberSearch(char *op_str) {
	int i;
	int op_idx = 0;
	int op_len = strlen(op_str);
	opcode_ptr search_op = NULL;

	for (i = 0; i < op_len; i++) {
		op_idx += op_str[i];
	}
	op_idx %= 20;
	search_op = opcode_table[op_idx];
	// calculate the index of opcode hash table
	while (search_op != NULL) {
		if (!strcmp(op_str, search_op->op_chr)) {
			return search_op->op_num;
		} // if opcode is found, return opcode.
		search_op = search_op->link;
	}

	return -1;
	// if there is no mnemonic
}

int symbolTableSearch(char *sym_str) {
	symbol_ptr sy_search;
	int table_idx = sym_str[0] - 65;

	sy_search = last_symbol_table[table_idx];
	while (sy_search != NULL) {
		if (!strcmp(sy_search->symbol_str,sym_str)) {
			return sy_search->locctr;
		} // if symbol is in the table, return the location counter of symbol.
		sy_search = sy_search->link;
	}
	return -1;
	// if there is no symbol, return -1.
}

void symbolTableInsert(int sym_locctr, char* sym_str) {
	symbol_ptr added_symbol = NULL;
	symbol_ptr search_front = NULL, search_rear = NULL;
	int table_idx = sym_str[0] - 65; // initialize the index of table

	added_symbol = (symbol_ptr)malloc(sizeof(symbol_node));
	if (added_symbol == NULL) {
		printf("Memory Allocation Error\n");
		exit(-1);
	}
	strcpy(added_symbol->symbol_str, sym_str);
	added_symbol->locctr = sym_locctr;
	added_symbol->link = NULL;
	// create new node of Symbol Table

	/*
		SYMBOL_TABLE_SIZE == 26
		(The number of Alphabet)
	*/
	search_front = last_symbol_table[table_idx];
	if (last_symbol_table[table_idx] == NULL) {
		last_symbol_table[table_idx] = added_symbol;
	} // if this index is empty
	else if (strcmp(added_symbol->symbol_str, search_front->symbol_str) > 0) {
		added_symbol->link = last_symbol_table[table_idx];
		last_symbol_table[table_idx] = added_symbol;
	} 
	else {
		search_rear = search_front->link;
		while (search_rear != NULL) {
			if (strcmp(added_symbol->symbol_str, search_rear->symbol_str) > 0) {
				search_front->link = added_symbol;
				added_symbol->link = search_rear;
				return;
			}
			search_front = search_front->link;
			search_rear = search_rear->link;
		}
		search_front->link = added_symbol;
	}
	// By descending order, put new symbol to the symbol table. 
}

void freeSymbolTable() {
	int i;
	symbol_ptr temp_node = NULL, deall_node = NULL;

	for (i = 0; i < SYMBOL_TABLE_SIZE;i++) {
		temp_node = last_symbol_table[i];

		while (temp_node != NULL) {
			deall_node = temp_node;
			temp_node = temp_node->link;
			free(deall_node);
		}
		last_symbol_table[i] = NULL;
	}
	// deallocation of symbol table
}

void printSymbolTable() {
	int i;
	symbol_ptr print_node = NULL;

	// By descending order, print the symbol.
	// Before printing, symbol is put by descending order.
	// So, print symbol from 'Z'
	for (i = SYMBOL_TABLE_SIZE-1; i >= 0;i--) {
		print_node = last_symbol_table[i];
		
		while (print_node != NULL) {
			printf("\t%s\t%04X\n",print_node->symbol_str,print_node->locctr);
			print_node = print_node->link;
		}
	}
}
