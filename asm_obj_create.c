#include "20121578.h"

int objFileMaking(char *lst_file_name,int locctr_start,int locctr_end) {
	FILE *lst_fp = NULL, *obj_fp = NULL; // file pointer
	char *obj_fname = NULL, *lst_fname = NULL;
	char lst_reader[129], temp_reader[129];
	char obj_line[62];
	char *cur_line, *cur_locctr, *sym, *mnemonic, *op_1, *op_2, *objcode; // token
	const char *obj_token = " ,\t\n"; // cutter
	int first_locctr = locctr_start;
	int h_byte_num = 0; // half byte number
	int cur_loc = locctr_start; // current location counter
	int loc_plus; // next loc
	int symbol_flag; // format 4 and symbol flag
	int *mod_record = NULL; // modification record
	int mod_num = 0; // the number of modification record 
	unsigned int i;
	int byte_len, j;
	
	obj_fname = (char*)malloc(sizeof(char) * (strlen(lst_file_name) + 1));
	lst_fname = (char*)malloc(sizeof(char) * (strlen(lst_file_name) + 1));
	strcpy(obj_fname, lst_file_name); strcpy(lst_fname, lst_file_name);
	obj_fname[strlen(obj_fname) - 3] = 'o'; obj_fname[strlen(obj_fname) - 2] = 'b'; obj_fname[strlen(obj_fname) - 1] = 'j';
	lst_fname[strlen(obj_fname) - 3] = 'l'; lst_fname[strlen(obj_fname) - 2] = 's'; lst_fname[strlen(obj_fname) - 1] = 't';
	lst_fp = fopen(lst_fname, "r");
	obj_fp = fopen(obj_fname, "w");
	if (lst_fp == NULL) {
		printf("ERROR : List file open error\n");
		return -1;
	} // open the list file
	if (obj_fp == NULL) {
		printf("ERROR : Object file creation error\n");
		return -1;
	} // make the object file

	if (locctr_start != -1) {
		fgets(lst_reader, 128, lst_fp);
		lst_reader[strlen(lst_reader) - 1] = '\0';
		cur_line = strtok(lst_reader, obj_token);
		cur_locctr = strtok(NULL, obj_token);
		sym = strtok(NULL, obj_token);
		fprintf(obj_fp, "H%-6s%06X%06X\n", sym, locctr_start, locctr_end - locctr_start);
		// write the Header
	}
	else {
		fprintf(obj_fp, "HTEMPSP%06X%06X\n", 0, locctr_end);
		locctr_start++;
		first_locctr = cur_loc = 0;
	}

	memset(obj_line, '\0', 62);
	while (fgets(lst_reader, 128, lst_fp) != NULL) {
		cur_line = cur_locctr = sym = mnemonic = op_1 = op_2 = objcode = NULL;
		symbol_flag = 0;
		loc_plus = 0;
		lst_reader[strlen(lst_reader) - 1] = '\0';
		strcpy(temp_reader, lst_reader);
		cur_line = strtok(temp_reader, obj_token);
		cur_locctr = strtok(NULL, obj_token);

		if (strcmp(cur_locctr, "BASE") == 0 || strcmp(cur_locctr, ".") == 0 || strcmp(cur_locctr, "NOBASE") == 0) {
			continue;
		} // directive BASE and comment
		else if (strcmp(cur_locctr, "END") == 0) {
			fprintf(obj_fp, "T%06X%02X%s\n", first_locctr, h_byte_num, obj_line);
			for (j = 0; j < mod_num; j++) {
				fprintf(obj_fp, "M%06X05\n", mod_record[j]);
			}
			fprintf(obj_fp, "E%06X\n", locctr_start);
			break;
		} // directive END
		else {
			sym = strtok(NULL, obj_token);
			if (sym[0] == '+' || symbolTableSearch(sym) == -1) {
				mnemonic = sym;
				sym = NULL;
			}
			else {
				mnemonic = strtok(NULL, obj_token);
			}
			// find label and mnemonic
			if (mnemonic[0] == '+') {
				symbol_flag = 0;
				loc_plus = 4;
				op_1 = strtok(NULL, obj_token);
				op_2 = strtok(NULL, obj_token);
				if (strlen(op_2) < 8) {
					objcode = strtok(NULL, obj_token);
				}
				else {
					objcode = op_2;
					op_2 = NULL;
				}
				if (op_1[0] == '#' || op_1[0] == '@') {
					op_1++;
				}
				for (i = 0; i < strlen(op_1); i++) {
					if (op_1[i] < 48 || op_1[i] > 57) {
						symbol_flag = 1;
					}
				}
				if (symbol_flag == 1) {
					if (mod_record == NULL) {
						mod_record = (int*)malloc(sizeof(int));
					}
					else {
						mod_record = (int*)realloc(mod_record, sizeof(int) * (mod_num + 1));
					}
					mod_record[mod_num++] = cur_loc + 1;
				}
				// symbol
			} // format 4. If the modification is needed, save the location counter.
			else if (strcmp(mnemonic, "BYTE") == 0) {
				byte_len = 0;
				op_1 = strtok(NULL, obj_token);
				for (i = 2; i < strlen(op_1) - 1; i++) {
					byte_len++;
				}
				if (op_1[0] == 'C') {
					loc_plus = byte_len;
				} // for characeter
				else {
					if (byte_len % 2 == 0) {
						loc_plus = (byte_len / 2);
					}
					else {
						loc_plus = (byte_len / 2) + 1;
					}
				} // for integer
				objcode = strtok(NULL, obj_token);
			} // directive byte
			else if (strcmp(mnemonic, "RESW") == 0) {
				op_1 = strtok(NULL, obj_token);
				loc_plus = 3 * strtol(op_1, NULL, 10);
				cur_loc += loc_plus;
				continue;
			} // directive RESW
			else if (strcmp(mnemonic, "RESB") == 0) {
				op_1 = strtok(NULL, obj_token);
				loc_plus = strtol(op_1, NULL, 10);
				fprintf(obj_fp, "T%06X%02X%s\n", first_locctr, h_byte_num, obj_line);
				first_locctr = cur_loc + loc_plus;
				cur_loc += loc_plus;
				h_byte_num = 0;
				memset(obj_line, '\0', 62);
				continue;
				// go to the next line
			} // directive RESB
			else if (strcmp(mnemonic, "WORD") == 0) {
				op_1 = strtok(NULL, obj_token);
				objcode = strtok(NULL, obj_token);
				loc_plus = 3;
			} // directive WORD
			else if (strcmp(mnemonic, "RSUB") == 0) {
				objcode = strtok(NULL, obj_token);
				loc_plus = 3;
			} // mnemonic RSUB
			else if (!strcmp(mnemonic, "SHIFTL") || !strcmp(mnemonic, "SHIFTR")) {
				op_1 = strtok(NULL, obj_token);
				op_2 = strtok(NULL, obj_token);
				objcode = strtok(NULL, obj_token);
				loc_plus = opSearch(mnemonic);
			}
			else {
				op_1 = strtok(NULL, obj_token);
				op_2 = strtok(NULL, obj_token);
				if (!strcmp(op_2, "X") || !strcmp(op_2, "A") || !strcmp(op_2, "L") || !strcmp(op_2, "PC") || !strcmp(op_2, "SW") || !strcmp(op_2, "B") || !strcmp(op_2, "S") || !strcmp(op_2, "T") || !strcmp(op_2, "F")) {
					objcode = strtok(NULL, obj_token);
				}
				else {
					objcode = op_2;
					op_2 = NULL;
				}
				loc_plus = opSearch(mnemonic);
			} // mnemonic

			if (h_byte_num + loc_plus > 0x1E) {
				fprintf(obj_fp, "T%06X%02X%s\n", first_locctr, h_byte_num, obj_line);
				first_locctr = cur_loc;
				cur_loc += loc_plus;
				h_byte_num = loc_plus;
				memset(obj_line, '\0', 62);
				strcpy(obj_line, objcode);
			} // write the tail
			else {
				strcat(obj_line, objcode);
				h_byte_num += loc_plus;
				cur_loc += loc_plus;
			}
		}
	}

	printf("   output file : [%s], [%s]\n", lst_fname, obj_fname); // print the result
	printf("   last line : %s\n", cur_line); // for erase the warning
	free(mod_record);
	free(lst_fname);
	free(obj_fname);
	fclose(lst_fp);
	fclose(obj_fp);
	return 1;
}
