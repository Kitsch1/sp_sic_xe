#include "20121578.h"

int asmFirstPass(char *asm_file_name) {
	FILE *asm_fp = NULL;
	int cur_locctr = 0, cur_line = 5, cur_fmt;
	char asm_reader[129];
	char *sym, *mnemonic, *operand_1;
	const char *asm_token = " ,\t\n"; // token for assembler
	unsigned int i = 0;
	int byte_len = 0;

	sym = mnemonic = operand_1 = NULL;

	asm_fp = fopen(asm_file_name,"r");
	if (asm_fp == NULL) {
		printf("ERROR : Assembly File Open Error.\n");
		return -1;
	}
	// open file
	if (asm_file_name[strlen(asm_file_name) - 3] != 'a' || asm_file_name[strlen(asm_file_name) - 2] != 's' || 
		asm_file_name[strlen(asm_file_name) - 1] != 'm') {
		printf("ERROR : You should assemble .asm file\n");
		return -2;
	}
	// if entered file isn't .asm file
	while (fgets(asm_reader,128,asm_fp) != NULL) {
		if (asm_reader[0] == '.') {
			cur_line += 5;
			continue;
		} // case : comment
		else {
			asm_reader[strlen(asm_reader) - 1] = '\0';
			trim_left(asm_reader);
			sym = strtok(asm_reader,asm_token);
			mnemonic = strtok(NULL,asm_token);
			if (strcmp(mnemonic,"START")) {
				cur_line -= 5;
				fseek(asm_fp, 0, SEEK_SET);
				break;
			} // if there is no directive 'START', set the first locctr to 0.
			operand_1 = strtok(NULL,asm_token);
			cur_locctr = strtol(operand_1, NULL, 16);
			break;
		}
	} // find the directive 'START' and set the first location counter

	while (fgets(asm_reader,128,asm_fp) != NULL) {
		cur_line += 5;
		sym = mnemonic = operand_1 = NULL;

		if (asm_reader[0] == '.') {
			cur_line += 5;
			continue;
		} // case : comment
		else {
			asm_reader[strlen(asm_reader) - 1] = '\0';
			trim_left(asm_reader);
			sym = strtok(asm_reader, asm_token);
			mnemonic = strtok(NULL, asm_token);
			operand_1 = strtok(NULL, asm_token);
			if (sym[0] == '+') {
				cur_locctr += 4;
				continue;
			} // if format 4
			else {
				for (i = 0; i < strlen(sym); i++) {
					if ((sym[i] >= 65 && sym[i] <= 90) || (sym[i] >= 97 || sym[i] <= 122)) {
						continue;
					}
					else {
						printf("ERROR : Invalid symbol name");
						return cur_line;
					}
				} // check the symbol name
				cur_fmt = opSearch(sym);
				if (cur_fmt != -1) {
					cur_locctr += cur_fmt;
					continue;
				} // if there is mnemonic, add the format number and go on to the next line.
				if (symbolTableSearch(sym) != -1) {
					printf("ERROR : You already put same symbol in symbol list.\n");
					return cur_line;
				}
				else if (!strcmp("BASE", sym) || !strcmp("NOBASE", sym)) {
					continue;
				}
				else if (!strcmp("END", sym)) {
					break;
				}
				else {
					symbolTableInsert(cur_locctr, sym);
				} // put the symbol in symbol table

				if (!strcmp("BYTE", mnemonic)) {
					byte_len = 0;
					if (operand_1[0] == 'C') {
						for (i = 2; i < strlen(operand_1); i++) {
							if (operand_1[i] == 39) {
								continue;
							}
							byte_len++;
						}
						cur_locctr += byte_len;
					} // character
					else if (operand_1[0] == 'X') {
						for (i = 2; i < strlen(operand_1); i++) {
							if (operand_1[i] == 39) {
								continue;
							}
							byte_len++;
						}
						if (byte_len % 2 == 0) {
							cur_locctr += byte_len / 2;
						}
						else {
							cur_locctr += (byte_len / 2) + 1;
						}
					}
					else {
						printf("ERROR : Directive BYTE can save only hexadecimal number or string.\n");
						return cur_line;
					} 
				} // case 'BYTE'
				else if (!strcmp("WORD", mnemonic)) {
					cur_locctr += 3;
				} // case 'WORD' (one integer constant)
				else if (!strcmp("RESW", mnemonic)) {
					cur_locctr += strtol(operand_1, NULL, 10) * 3;
				} // case 'RESW' 
				else if (!strcmp("RESB", mnemonic)) {
					cur_locctr += strtol(operand_1, NULL, 10);
				} // case 'RESB'
				else {
					if (mnemonic[0] == '+') {
						++mnemonic;
						cur_fmt = opSearch(mnemonic);
						if (cur_fmt == -1) {
							printf("ERROR : Invalid mnemonic.\n");
							return cur_line;
						}
						else {
							cur_locctr += 4;
						}
					} // format 4
					else {
						cur_fmt = opSearch(mnemonic);
						if (cur_fmt == -1) {
							printf("ERROR : Invalid mnemonic.\n");
							return cur_line;
						}
						else {
							cur_locctr += cur_fmt;
						}
					} // others
				} // ordinary cases
			}
		}
	}

	fclose(asm_fp);
	return 1;
}

int asmSecondPass(char *asm_file_name,int *start_locctr,int *end_locctr,int *nobase_flag) {
	FILE *asm_fp = NULL, *lst_fp = NULL;
	char *lst_file_name = NULL;
	int cur_line = 5;
	int cur_locctr = 0, base_num = 0, cur_fmt;
	char asm_reader[129], temp_reader[129];
	char *sym, *mnemonic, *operand_1, *operand_2;
	const char *asm_token = " ,\t\n"; // token for assembler
	int byte_len, obj_code, opcode_num;
	unsigned int i;

	sym = mnemonic = operand_1 = operand_2 = NULL;

	asm_fp = fopen(asm_file_name, "r");
	if (asm_fp == NULL) {
		printf("ERROR : Assembly File Open Error.\n");
		return -1;
	}
	lst_file_name = (char*)malloc(sizeof(char) * (strlen(asm_file_name) + 1));
	strcpy(lst_file_name, asm_file_name);
	lst_file_name[strlen(lst_file_name) - 1] = 't';
	lst_file_name[strlen(lst_file_name) - 2] = 's';
	lst_file_name[strlen(lst_file_name) - 3] = 'l';
	lst_fp = fopen(lst_file_name, "w");
	if (lst_fp == NULL) {
		printf("ERROR : List file open error\n");
		return -1;
	}
	// make list file

	while (fgets(asm_reader, 128, asm_fp) != NULL) {
		if (asm_reader[0] == '.') {
			cur_line += 5;
			continue;
		}
		else {
			asm_reader[strlen(asm_reader) - 1] = '\0';
			strcpy(temp_reader, asm_reader);
			sym = strtok(temp_reader, asm_token);
			mnemonic = strtok(NULL, asm_token);
			if (strcmp("START",mnemonic)) {
				cur_line -= 5;
				*start_locctr = -1;
				fseek(asm_fp, 0, SEEK_SET);
				break;
			}
			operand_1 = strtok(NULL, asm_token);
			operand_2 = strtok(NULL, asm_token);
			cur_locctr = strtol(operand_1, NULL, 16);
			*start_locctr = cur_locctr;
			fprintf(lst_fp, "%4d %04X %s\n", cur_line, cur_locctr, asm_reader);
			break;
		}
	} // find the start location. if there is no START, initialize location to 0.

	while (fgets(asm_reader, 128, asm_fp) != NULL) {
		sym = mnemonic = operand_1 = operand_2 = NULL;
		cur_line += 5;
		if (asm_reader[0] == '.') {
			cur_line += 5;
			continue;
		}
		else {
			asm_reader[strlen(asm_reader) - 1] = '\0';
			strcpy(temp_reader, asm_reader);
			sym = strtok(temp_reader, asm_token);
			if (sym[0] == '+' || !strcmp(sym,"BASE") || !strcmp(sym, "END") || !strcmp(sym,"NOBASE") || opSearch(sym) != -1) {
				mnemonic = sym;
				sym = NULL;
			}
			else {
				mnemonic = strtok(NULL, asm_token);
			}
			// read symbol and mnemonic
			operand_1 = strtok(NULL, asm_token);
			operand_2 = strtok(NULL, asm_token);
			if (!strcmp(mnemonic, "BASE") || !strcmp(mnemonic, "NOBASE")) {
				fprintf(lst_fp, "%4d      %-28s ", cur_line, asm_reader);
			}
			// for directive BASE
			else if (strcmp(mnemonic, "END") == 0) {
				fprintf(lst_fp, "%4d      %-28s ", cur_line, asm_reader);
				*end_locctr = cur_locctr;
				fprintf(lst_fp, "\n");
				break;
			}
			// for directive END
			else {
				fprintf(lst_fp, "%4d %04X %-28s ", cur_line, cur_locctr, asm_reader);
			} 
			// other cases
			if (!strcmp(mnemonic, "WORD")) {
				cur_locctr += 3;
				obj_code = strtol(operand_1, NULL, 10);
				fprintf(lst_fp, "%06X\n", obj_code);
				continue;
			} // for directive WORD
			else if (!strcmp(mnemonic,"RESB")) {
				cur_locctr += strtol(operand_1, NULL, 10);
				fprintf(lst_fp, "\n");
				continue;
			} // for directive RESB
			else if (!strcmp(mnemonic,"RESW")) {
				cur_locctr += strtol(operand_1, NULL, 10) * 3;
				fprintf(lst_fp, "\n");
				continue;
			} // for directive RESW
			else if (!strcmp(mnemonic,"BYTE")) {
				byte_len = 0;
				if (operand_1[0] == 'C') {
					for (i = 2; i < strlen(operand_1);i++) {
						if (operand_1[i] == 39) {
							continue;
						}
						fprintf(lst_fp, "%02X", operand_1[i]);
						byte_len++;
					}
					if(byte_len > 0x1E){
						printf("ERROR : BYTE is too long\n");
						remove(lst_file_name);
						return cur_line;
					} // if BYTE is too long (constraint : 0x1E)
					cur_locctr += byte_len;
					fprintf(lst_fp, "\n");
					continue;
				} // BYTE character
				else if (operand_1[0] == 'X') {
					for (i = 2; i < strlen(operand_1); i++) {
						if (operand_1[i] == 39) {
							continue;
						}
						fprintf(lst_fp, "%C", operand_1[i]);
						byte_len++;
					}
					if(byte_len > 60){
						printf("ERROR : BYTE is too long\n");
						remove(lst_file_name);
						return cur_line;
					} // if BYTE is too long (constraint : 0x1E)
					if (byte_len % 2 == 0) {
						cur_locctr += (byte_len / 2);
					}
					else {
						cur_locctr += (byte_len / 2) + 1;
					}
					fprintf(lst_fp, "\n");
					continue;
				} // BYTE hexadecimal
				else {
					printf("ERROR : The directive BYTE only create character or hexadecimal integer\n");
					return cur_line;
				}
			}
			else if (!strcmp(mnemonic,"BASE")) {
				base_num = symbolTableSearch(operand_1);
				if (base_num == -1) {
					printf("ERROR : There is no symbol for BASE\n");
					remove(lst_file_name);
					return cur_line;
				}
				*nobase_flag = 0;
				// find symbol BASE in symbol table
				fprintf(lst_fp, "\n");
				continue;
			}
			else if (!strcmp(mnemonic,"NOBASE")) {
				fprintf(lst_fp, "\n");
				continue;
			}
			else if (!strcmp(mnemonic,"RSUB")) {
				if(operand_1 != NULL){
					printf("mnemonic RSUB doesn't need operand\n");
					remove(lst_file_name);
					return cur_line;
				} // RSUB doesn't need other operand.
				obj_code = 0;
				obj_code += 0x4C; obj_code += 3;
				obj_code <<= 16;
				fprintf(lst_fp, "%06X\n", obj_code);
				cur_locctr += 3;
				continue;
			} // for mnemonic 'RSUB'
			else {
				obj_code = 0;
				if (mnemonic[0] == '+') {
					cur_fmt = 4;
					++mnemonic;
				} // format 4
				else {
					cur_fmt = opSearch(mnemonic);
				} // format 1,2,3
				opcode_num = opcodeNumberSearch(mnemonic);
				if(opcode_num == 0xC0 || opcode_num == 0xC4 || opcode_num == 0xF4 || opcode_num == 0xC8 ||
						opcode_num == 0xF0 || opcode_num == 0xF8){
					if(operand_1 != NULL){
						printf("mnemonic %s doesn't need operand\n",mnemonic);
						remove(lst_file_name);
						return cur_line;
					}
				} 
				// for mnemonics that do not need operand
				// 'FLOAT', 'FIX', 'HIO', 'NORM', 'SIO', 'TIO'
				switch (cur_fmt) {
					case 1: fprintf(lst_fp, "%02X\n", opcode_num); break;
					case 2: obj_code = objFormatTwo(opcode_num, operand_1, operand_2); fprintf(lst_fp, "%04X\n", obj_code); break;
					case 3: obj_code = objFormatThree(opcode_num, operand_1, operand_2, cur_locctr, base_num, *nobase_flag); fprintf(lst_fp, "%06X\n", obj_code); break;
					case 4: obj_code = objFormatFour(opcode_num, operand_1, operand_2, cur_locctr); fprintf(lst_fp, "%08X\n", obj_code); break;
					default: printf("ERROR : Invalid opcode\n"); return cur_line;
				} // By format, execute the function.
				if (obj_code == -1) {
					printf("ERROR : Object Code Error\n");
					remove(lst_file_name);
					return cur_line;
				}
				cur_locctr += cur_fmt; // accumulate location counter
			}
			// ordinary case
		}
	}

	fclose(asm_fp);
	fclose(lst_fp);
	free(lst_file_name);
	return 1;
}

int asmCreate(char *asm_file_name) {
	int start_locctr = 0, end_locctr = 0;
	int res_flag;
	int nobase_flag = 1; // 1 : NOBASE, 0 : BASE

	freeSymbolTable();
	res_flag = asmFirstPass(asm_file_name); // Pass 1
	if (res_flag == 1) { // if Pass 1 executed normally,
		res_flag = asmSecondPass(asm_file_name,&start_locctr,&end_locctr,&nobase_flag);
		// Pass 2
		if (res_flag == 1) {
			res_flag = objFileMaking(asm_file_name, start_locctr, end_locctr);
		} // if Pass 2 is executed normally, create .obj file
	}
	if (res_flag == 1) {
		return 1;
	}
	else if (res_flag == -1) {
		printf("Plase check the existance of file\n");
		return -1;
	} // if there is no file
	else if (res_flag == -2) {
		printf("Please assemble .asm file\n");
		return -1;
	} // if user try to assemble other form of file
	else {
		printf("Please check the assembly source code Line %d\n",res_flag);
		freeSymbolTable(); // deallocate symbol table
		return -1;
	} // information of error line
}
