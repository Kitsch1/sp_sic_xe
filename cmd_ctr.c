#include "20121578.h"

char* trim_left(char *tok_num) {
	while (isspace(*tok_num)) {
		tok_num++;
	}

	return tok_num;
}
// erase the left-sided space that include spacebar and tap.

char* trim_right(char *tok_num) {
	char *end;

	end = tok_num + strlen(tok_num) - 1;
	while (end > tok_num && isspace(*end)) {
		end--;
	}
	*(end + 1) = 0;

	return tok_num;
} // erase the right-sided space that include spacebar and tap.

void sic_menu() {
	char input_str[256], history_str[256], copy_str[256];
	char *check_str;
	char *input_arr[5], *check_arr[5]; // input : used for menu. check : used for checking input error
	const char *cmd_tok = " \t\n"; // tokens for the first tokenize
	const char *cmd_comma_tok = " ,\t\n"; // tokens for du[mp], e[dit], f[ill]
	const char *comma_only_tok = ","; // tokens for the comma check
	int i,cmd_num,exec_res,input_check_num;
	int cur_mem_ctr = 0;
	int prog_start_addr = 0;
	int prog_exec_addr = 0;
	int prog_end_addr = 0;
	int prog_run_flag = 0;
	/* exec_res : used for distinguishing the state of result 
	   cmd_num : the number of tokens 
	   input_check_run : integer that used for checking the error of input */

	while (1) {
		for (i = 0; i < 5;i++) {
			input_arr[i] = check_arr[i] = NULL;
		}
		__fpurge(stdin); // clean the buffer
		check_str = NULL;
		cmd_num = 0;
		/* initialization */

		printf("sicsim> ");
		fgets(input_str,256,stdin); // input
		input_str[strlen(input_str) - 1] = '\0';
		strcpy(copy_str, input_str);
		strcpy(history_str,input_str);
		input_arr[cmd_num++] = strtok(input_str, cmd_tok);
		//input_arr[cmd_num++] = strtok(NULL, cmd_tok);
		while (cmd_num < 5) {
			input_arr[cmd_num] = strtok(NULL, cmd_comma_tok);
			if (input_arr[cmd_num] == NULL) {
				break;
			}
			cmd_num++;
		}
		// tokenize the command

		// input error checking part (start)
		if (input_arr[0] == NULL) {
			continue;
		} // no input
		else if (input_arr[4] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			continue;
		} // too many inputs
		else {
			check_str = copy_str;
			check_str = trim_left(check_str);
			check_str = trim_right(check_str);
			input_check_num = strlen(check_str);
			if (check_str[input_check_num - 1] == ',') {
					err_msg_bundle(WRONG_INPUTS);
					input_check_num = -1;
			} 
			// wrong token (last letter is comma)
			if (input_check_num == -1) {
				continue;
			}
			// recieve input again
			if (!strcmp(input_arr[0], "du") || !strcmp(input_arr[0], "dump") || !strcmp(input_arr[0], "e") || !strcmp(input_arr[0], "edit") || !strcmp(input_arr[0], "f") || !strcmp(input_arr[0], "fill") 
				|| !strcmp(input_arr[0],"opcode") || !strcmp(input_arr[0],"assemble") || !strcmp(input_arr[0],"type") || !strcmp(input_arr[0],"bp") || !strcmp(input_arr[0],"progaddr")) {
				if (input_arr[1] != NULL) {
					cmd_num = 0;
					check_arr[cmd_num++] = strtok(check_str, cmd_tok);
					while (cmd_num < 5) {
						check_arr[cmd_num] = strtok(NULL, comma_only_tok);
						if (check_arr[cmd_num] == NULL) {
							break;
						}
						check_arr[cmd_num] = trim_left(check_arr[cmd_num]);
						check_arr[cmd_num] = trim_right(check_arr[cmd_num]);
						cmd_num++;
					} // tokenize by comma
					for(i=input_check_num-1;i>=0;i--){
						if(check_str[i] == ','){
							err_msg_bundle(WRONG_INPUTS);
							input_check_num = -1;
							break;
						}
					}
					if(input_check_num == -1){
						continue;
					} // if there are many commas between commands
					for (i = 0; i < 5; i++) {
						if (input_arr[i] != NULL && check_arr[i] != NULL) {
							if (strcmp(input_arr[i], check_arr[i])) {
								err_msg_bundle(WRONG_INPUTS);
								input_check_num = -1;
								break;
							}
						}
						else if (input_arr[i] == NULL && check_arr[i] == NULL) {
							break;
						}
						else {
							err_msg_bundle(WRONG_INPUTS);
							input_check_num = -1;
							break;
						}
					} // checking if there is comma between the memory number
					if (input_check_num == -1) {
						continue;
					} // if there is no comma between the memory number
				}
			} // du[mp], e[dit], f[ill] : comma checking
		} // checking
		// input error checking part (end)

		exec_res = sic_menu_classify(input_arr,history_str,&cur_mem_ctr,&prog_start_addr,&prog_exec_addr,&prog_end_addr,&prog_run_flag); // execution of command
		if (exec_res == -1 || exec_res == 2) {
			continue;
		} // error or hi[story] command
		else if (exec_res == 0) {
			break;
		} // quit
		else {
			history_add(history_str);
		} // save the history (need to add)

		/*
		  exec_res == -1 : error
		  exec_res == 0 : quit
		  exec_res == 1 : executed normally
		  exec_res == 2 : hi[story] executed normally
		*/
	}
}

int sic_menu_classify(char** input_arr, char* history_str, int *cur_mem_ctr, int *prog_start_addr, int *prog_exec_addr, int *prog_end_addr, int *prog_run_flag) {
	/*
		return -1 : error
		return 0 : only quit
		return 1 : executed normally
		return 2 : hi[story] executed normally
	*/
	int du_start, du_end; // dump start addr, dump end addr
	int edit_addr, edit_val; // address that edited and value
	int fill_start, fill_end, fill_val;  // fill start addr, fill end addr, value
	int prog_addr = 0;
	du_start = du_end = edit_addr = edit_val = 0;
	fill_start = fill_end = fill_val = 0;
	if (!strcmp(input_arr[0], "h") || !strcmp(input_arr[0], "help")) {
		if (input_arr[1] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		sic_help();
		return 1; // executed normally
	}
	else if (!strcmp(input_arr[0], "d") || !strcmp(input_arr[0], "dir")) {
		if (input_arr[1] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		return sic_dir(); // execute the function
	}
	else if (!strcmp(input_arr[0], "q") || !strcmp(input_arr[0], "quit")) {
		if (input_arr[1] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		return 0; // quit : return 0
	}
	else if (!strcmp(input_arr[0], "hi") || !strcmp(input_arr[0], "history")) {
		if (input_arr[1] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		history_add(history_str);
		// hi[story] is also command so it must be added in history list.
		sic_history_print();
		// print history list
		return 2;
	}
	else if (!strcmp(input_arr[0], "du") || !strcmp(input_arr[0], "dump")) {
		if (input_arr[3] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		if (input_arr[1] == NULL) {
			mem_dump_only(cur_mem_ctr);
		} // dump (only)
		else {
			du_start = mem_hexa_convert(input_arr[1]);
			// convert dump start addr string to integer
			if (du_start == -1) {
				return -1;
			} // if there is wrong addr
			else if (du_start > 0xfffff || du_start < 0) {
				err_msg_bundle(WRONG_BOUNDARY);
				return -1;
			}
			// boundary checking
			if (input_arr[2] == NULL) {
				mem_dump_first_only(du_start,cur_mem_ctr);
			} // dump (start only)
			else {
				du_end = mem_hexa_convert(input_arr[2]);
				// convert dump end addr string to integer
				if (du_end == -1) {
					return -1;
				} // if there is wrong addr
				if (du_start > du_end || du_end > 0xfffff || du_end < 0 || du_start > 0xfffff || du_start < 0) {
					err_msg_bundle(WRONG_BOUNDARY);
					return -1;
				} // if there is wrong boundary
				mem_dump_first_last(du_start, du_end, cur_mem_ctr);
			} // dump (start, end)
		} // dump
		return 1;
		// need to add
	}
	else if (!strcmp(input_arr[0],"e") || !strcmp(input_arr[0],"edit")) {
		if (input_arr[3] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		else if (input_arr[2] == NULL) {
			err_msg_bundle(FEW_INPUTS);
			return -1;
		}

		edit_addr = mem_hexa_convert(input_arr[1]);
		// convert edit addr string to integer
		if (edit_addr == -1) {
			return -1;
		} // if there is wrong addr
		else if (edit_addr > 0xfffff || edit_addr < 0) {
			err_msg_bundle(WRONG_BOUNDARY);
			return -1;
		} // if there is wrong boundary

		edit_val = mem_hexa_convert(input_arr[2]);
		// convert edit val string to integer (ASCII code number)
		if (edit_val == -1) {
			return -1;
		}
		// if there is wrong input
		else if (edit_val > 0xff || edit_val < 0) {
			err_msg_bundle(WRONG_BOUNDARY);
			return -1;
		}
		// if there is wrong ASCII code

		mem_edit(edit_addr, edit_val);
		return 1;
		// edit the memory
	}
	else if (!strcmp(input_arr[0],"f") || !strcmp(input_arr[0],"fill")) {
		if (input_arr[3] == NULL) {
			err_msg_bundle(FEW_INPUTS);
			return -1;
		}

		/*
			convert fill start addr and input, boundary error checking
		*/
		fill_start = mem_hexa_convert(input_arr[1]);
		if (fill_start == -1) {
			return -1;
		}
		else if (fill_start > 0xfffff) {
			err_msg_bundle(WRONG_BOUNDARY);
			return -1;
		}

		/*
			convert fill end addr and input, boundary error checking
		*/
		fill_end = mem_hexa_convert(input_arr[2]);
		if (fill_end == -1) {
			return -1;
		}
		else if (fill_end > 0xfffff || fill_end < 0) {
			err_msg_bundle(WRONG_BOUNDARY);
			return -1;
		}

		/*
			boundary checking if end addr is lower than start addr
		*/
		if (fill_start > fill_end) {
			err_msg_bundle(WRONG_BOUNDARY);
			return -1;
		}

		/*
			convert fill value and input error checking
		*/
		fill_val = mem_hexa_convert(input_arr[3]);
		if (fill_val == -1) {
			return -1;
		}
		else if (fill_val > 0xff || fill_val < 0) {
			err_msg_bundle(WRONG_BOUNDARY);
			return -1;
		}

		mem_fill(fill_start, fill_end, fill_val);
		// f[ill]
		return 1;
	}
	else if (!strcmp(input_arr[0],"reset")) {
		if (input_arr[1] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		mem_reset(); // reset the memory
		return 1;
	}
	else if (!strcmp(input_arr[0],"opcode")) {
		if (input_arr[1] == NULL) {
			err_msg_bundle(FEW_INPUTS);
			return -1;
		}
		else if (input_arr[2] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}

		if (opcode_find(input_arr[1],strlen(input_arr[1])) == 0) {
			return 1;
		} // if opcode is in hash table, opcode_find returns 0.
		else {
			return -1;
		} // else
	}
	else if (!strcmp(input_arr[0],"opcodelist")) {
		if (input_arr[1] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		opcode_table_print(); // print opcode hash table
		return 1;
	}
	else if (!strcmp(input_arr[0],"assemble")) {
		if (input_arr[1] == NULL) {
			err_msg_bundle(FEW_INPUTS);
			return -1;
		}
		else if (input_arr[2] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		return asmCreate(input_arr[1]);
		// execute asmCreate function
	} // command 'assemble'
	else if (!strcmp(input_arr[0],"type")) {
		if (input_arr[1] == NULL) {
			err_msg_bundle(FEW_INPUTS);
			return -1;
		}
		else if (input_arr[2] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		
		return sicType(input_arr[1]);
	} // command 'type'
	else if (!strcmp(input_arr[0],"symbol")) {
		if (input_arr[1] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		printSymbolTable(); // print symbol table.
		return 1;
	} // command 'symbol'
	else if (!strcmp(input_arr[0],"progaddr")) {
		if (input_arr[1] == NULL) {
			err_msg_bundle(FEW_INPUTS);
			return -1;
		}
		else if (input_arr[2] != NULL) {
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}
		else {
			prog_addr = mem_hexa_convert(input_arr[1]);
			if (prog_addr == -1) {
				return -1;
			}
			*prog_start_addr = prog_addr;
			return 1;
		}
	}
	else if (!strcmp(input_arr[0],"loader")) {
		if (input_arr[1] == NULL) {
			err_msg_bundle(FEW_INPUTS);
			return -1;
		}
		
		return loaderBundle(*prog_start_addr, input_arr[1], input_arr[2], input_arr[3], prog_exec_addr,prog_end_addr);
	}
	else if (!strcmp(input_arr[0],"bp")) {
		if (input_arr[1] == NULL) {
			printBreakPoint();
			return 1;
		} // only bp : print all breakpoints
		else if (input_arr[2] != NULL){
			err_msg_bundle(MANY_INPUTS);
			return -1;
		} // too many
		else {
			if (!strcmp(input_arr[1],"clear")) {
				clearBreakPoint();
				return 1;
			}// clear all breakpoints
			else {
				return insertBreakPoint(input_arr[1]);
			}
		} // add breakpoints
	}
	else if (!strcmp(input_arr[0],"run")) {
		if(input_arr[1] != NULL){
			err_msg_bundle(MANY_INPUTS);
			return -1;
		}

		return runProgram(*prog_exec_addr, *prog_end_addr, prog_run_flag);
	}
	else {
		err_msg_bundle(WRONG_INPUTS);
		return -1;
	} // the others
}
