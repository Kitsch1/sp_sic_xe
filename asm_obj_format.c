#include "20121578.h"

int objFormatTwo(int opcode_num,char *operand_1, char *operand_2) {
	int obj = opcode_num;

	obj <<= 4;
	if (operand_1 != NULL) {
		if(opcode_num == 0xB0){
			obj += strtol(operand_1,NULL,16);
		} // special case : mnemonic 'SVC'

		if (strcmp(operand_1, "A") == 0) {
			obj += 0;
		}
		else if (strcmp(operand_1, "X") == 0) {
			obj += 1;
		}
		else if (strcmp(operand_1, "L") == 0) {
			obj += 2;
		}
		else if (strcmp(operand_1, "PC") == 0) {
			obj += 8;
		}
		else if (strcmp(operand_1, "SW") == 0) {
			obj += 9;
		}
		else if (strcmp(operand_1, "B") == 0) {
			obj += 3;
		}
		else if (strcmp(operand_1, "S") == 0) {
			obj += 4;
		}
		else if (strcmp(operand_1, "T") == 0) {
			obj += 5;
		}
		else if (strcmp(operand_1, "F") == 0) {
			obj += 6;
		}
		// ordinary cases
		else {
			printf("ERROR : Wrong register\n");
			return -1;
		}
		// if there is invalid name of register
	} // find the first register
	else{
		printf("ERROR : Please input at least one register\n");
		return -1;
	}
	// format 2 needs at least one register.

	obj <<= 4;
	if (operand_2 != NULL) {
		if(opcode_num == 0xB4 || opcode_num == 0xB8 || opcode_num == 0xB0){
			printf("ERROR : Only 1 register needed\n");
			return -1;
		} // special case : mnemonic 'CLEAR', 'TIXR', 'SVC'
		if(opcode_num == 0xA4){
			obj += strtol(operand_2,NULL,16);
			return obj;
		} // special case : mnemonic 'SHIFTL'

		if (strcmp(operand_2, "A") == 0) {
			obj += 0;
		}
		else if (strcmp(operand_2, "X") == 0) {
			obj += 1;
		}
		else if (strcmp(operand_2, "L") == 0) {
			obj += 2;
		}
		else if (strcmp(operand_2, "PC") == 0) {
			obj += 8;
		}
		else if (strcmp(operand_2, "SW") == 0) {
			obj += 9;
		}
		else if (strcmp(operand_2, "B") == 0) {
			obj += 3;
		}
		else if (strcmp(operand_2, "S") == 0) {
			obj += 4;
		}
		else if (strcmp(operand_2, "T") == 0) {
			obj += 5;
		}
		else if (strcmp(operand_2, "F") == 0) {
			obj += 6;
		}
		// ordinary cases
		else {
			printf("ERROR : Wrong register\n");
			return -1;
		}
		// if there is invalid name
	} // find the second register
	else if(opcode_num == 0x90 || opcode_num == 0xA0 || opcode_num == 0x9C || opcode_num == 0x98 ||
			opcode_num == 0xAC || opcode_num == 0xA4 || opcode_num == 0x94){
		printf("ERROR : r2 is needed\n");
		return -1;
	} 
	// for the registers that operand_2 is needed
	// 'ADDR', 'COMPR', 'DIVR', 'MULR', 'RMO', 'SHIFTL', 'SUBR'

	return obj;
}

int objFormatThree(int opcode_num, char *operand_1, char *operand_2, int cur_locctr, int base_num, int nobase_flag) {
	int obj = opcode_num;
	int const_flag = 0;
	int sym_locctr, disp = 0;
	int cur_pc = cur_locctr + 3;
	unsigned int i;

	if (operand_1[0] == '#') {
		obj += 1;
		++operand_1;
	} // immediate addressing
	else if (operand_1[0] == '@') {
		obj += 2;
		++operand_1;
	} // indirect addressing
	else {
		obj += 3;
	} // simple addressing
	obj <<= 4;

	for (i = 0; i < strlen(operand_1); i++) {
		if (operand_1[i] < 48 || operand_1[i] > 57) {
			const_flag = 1;
			break;
		} // not number
	}
	if (const_flag == 1) {
		sym_locctr = symbolTableSearch(operand_1);
		// find the symbol in symbol table
		if (sym_locctr == -1) {
			printf("ERROR : Invalid symbol\n");
			return -1;
		}
		// if cannot found, return -1.
	}
	else {
		sym_locctr = strtol(operand_1, NULL, 10);
	}
	// if operand 1 is constant

	if (operand_2 != NULL) {
		if (!strcmp(operand_2, "X")) {
			obj += 8;
		} // indexded mode
		else {
			printf("ERROR : Invalid operand\n");
			return -1;
		}
	}
	if (const_flag == 0) { // constant
		if (sym_locctr > 4095) {
			return objFormatFour(opcode_num, operand_1, operand_2, cur_locctr);
		} // if we cannot use format 3
		else {
			obj <<= 12; 
			obj += sym_locctr;
			return obj;
		}
	}
	else {
		disp = sym_locctr - cur_pc;
		if (disp >= -2048 && disp <= 2047) {
			obj += 2; obj <<= 12;
			if (disp >= 0) {
				obj += disp;
			}
			else {
				disp &= 0x00000FFF;
				obj += disp;
			}
			return obj;
		} // pc relatives
		else if (nobase_flag == 0) {
			disp = sym_locctr - base_num;
			if (disp >= 0 && disp <= 4095) {
				obj += 4; obj <<= 12; obj += disp;
				return obj;
			}
			else {
				return objFormatFour(opcode_num, operand_1, operand_2, cur_locctr);
			} // if we cannot use both PC relative and BASE relative, use format 4.
		} // base relative
		else {
			return objFormatFour(opcode_num, operand_1, operand_2, cur_locctr);
		} // if we cannot use both PC relative and BASE relative, use format 4.
	}
}

int objFormatFour(int opcode_num, char *operand_1, char *operand_2, int cur_locctr) {
	int obj = opcode_num;
	int sym_locctr, const_flag = 0;
	unsigned int i;

	if (operand_1[0] == '#') {
		obj += 1;
		++operand_1;
	} // immediate addressing
	else if (operand_1[0] == '@') {
		obj += 2;
		++operand_1;
	} // indirect addressing
	else {
		obj += 3;
	}
	obj <<= 4;

	for (i = 0; i < strlen(operand_1); i++) {
		if (operand_1[i] < 48 || operand_1[i] > 57) {
			const_flag = 1;
			break;
		} // not number
	}
	if (const_flag == 1) {
		sym_locctr = symbolTableSearch(operand_1);
		if (sym_locctr == -1) {
			printf("ERROR : Invalid symbol\n");
			return -1;
		}
	}
	else {
		sym_locctr = strtol(operand_1, NULL, 10);
	}

	if (operand_2 != NULL) {
		if (!strcmp(operand_2, "X")) {
			obj += 8;
		} // indexded mode
		else {
			printf("ERROR : Invalid operand\n");
			return -1;
		}
	}
	obj += 1; // for the bit 'e'
	obj <<= 20; 
	obj += sym_locctr; // put the address

	return obj;

}
