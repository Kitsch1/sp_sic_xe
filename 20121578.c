#include "20121578.h"

int main() {
	opcode_init(); // initialize opcode hash table
	bp_cnt = 0; bp_idx = -1; // initialize breakpoint count
	sic_menu(); // main menu : sicsim>
	opcode_free(); // deallocate opcode hash table
	history_free(); // deallocate hash table list
	freeSymbolTable(); // deallocate symbol table list

	return 0;
}

void err_msg_bundle(int err_state) {
	/*
		This function contains various types of error messages in error cases.
	*/
	if (err_state == FEW_INPUTS) {
		printf("ERROR : Not enough inputs\n");
		return;
	}
	if (err_state == MANY_INPUTS) {
		printf("ERROR : Too many inputs\n");
		return;
	}
	if (err_state == WRONG_INPUTS) {
		printf("ERROR : Wrong input. Please input correctly.\n");
		return;
	}
	if (err_state == WRONG_MNEMONIC) {
		printf("ERROR : Wrong mnemonic. Please input valid mnemonic.\n");
		return;
	}
	if (err_state == WRONG_BOUNDARY) {
		printf("ERROR : Wrong Boundary. Please input valid boundary.\n");
		return;
	}
	if (err_state == NO_FILE_EXIST) {
		printf("ERROR : File doesn't exist.\n");
		return;
	}
}
