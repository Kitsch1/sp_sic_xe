#include "20121578.h"

void printBreakPoint() {
	bp_ptr iter_bp = bp_front;

	printf("breakpoint\n----------\n");
	while (iter_bp != NULL) {
		printf("%X\n",iter_bp->bp_loc);
		iter_bp = iter_bp->link;
	}
}

int insertBreakPoint(char *bp_new) {
	int bp_new_num = 0;
	bp_ptr first, second, search;
	bp_ptr new_node;
	first = second = new_node = search = NULL;

	bp_new_num = mem_hexa_convert(bp_new);
	if (bp_new_num == -1) {
		return -1;
	} // convert

	search = bp_front;
	while (search != NULL) {
		if (search->bp_loc == bp_new_num) {
			printf("ERROR : That breakpoint is already in the list.\n");
			return -1;
		}
		search = search->link;
	} // redundancy check

	new_node = (bp_ptr)malloc(sizeof(bp_node));
	new_node->bp_loc = bp_new_num;
	new_node->link = NULL;

	if (bp_front == NULL) {
		bp_front = new_node;
		bp_rear = new_node;
	}
	else if (new_node->bp_loc < bp_front->bp_loc) {
		new_node->link = bp_front;
		bp_front = new_node;
	}
	else {
		first = bp_front;
		second = bp_front->link;

		while (second != NULL) {
			if (new_node->bp_loc < second->bp_loc) {
				new_node->link = second;
				first->link = new_node;
			}
			first = first->link;
			second = second->link;
		}
		first->link = new_node;
	}
	if (bp_cnt == 0) {
		bp_cnt++;
		bp_idx = 0;
	}
	else {
		bp_cnt++;
	}
	printf("[ok] create breakpoint %X\n",bp_new_num);
	return 1;
}

void clearBreakPoint() {
	bp_ptr deall_bp = bp_front;

	while (deall_bp != NULL) {
		bp_front = bp_front->link;
		free(deall_bp);
		deall_bp = bp_front;
	}
	bp_front = bp_rear = NULL;
	bp_cnt = 0;
	bp_idx = -1;
	printf("[ok] clear all breakpoints\n");
}

int searchBreakPoint() {
	bp_ptr search_bp = bp_front;
	int i = 0;

	for (i = 0; i < bp_idx;i++) {
		search_bp = search_bp->link;
	}
	
	return search_bp->bp_loc;
}

void deallESTAB() {
	int i;
	es_ptr deall_node, temp_node;

	for (i = 0; i < HASH_SIZE;i++) {
		if (external_table[i] != NULL) {
			temp_node = external_table[i];

			while (temp_node != NULL) {
				deall_node = temp_node;
				temp_node = temp_node->link;
				free(deall_node);
			}
			external_table[i] = NULL;
		}
	}
}

int searchESTAB(char *search_sym) {
	int hash_idx = 0;
	int i;
	es_ptr search_node;

	for (i = (int)strlen(search_sym) - 1; i >= 0; i--) {
		hash_idx += search_sym[i];
	}
	hash_idx %= 20;
	
	search_node = external_table[hash_idx];
	while (search_node != NULL) {
		if (!strcmp(search_node->sym_name,search_sym)) {
			return search_node->sym_addr;
		}
		search_node = search_node->link;
	}

	return -1;
}

void insertESTAB(char *sym_name,int sym_addr,int length) {
	int hash_idx = 0;
	int i;
	es_ptr new_node = NULL;

	for (i = (int)strlen(sym_name) - 1; i >= 0; i--) {
		hash_idx += sym_name[i];
	}
	hash_idx %= 20;

	new_node = (es_ptr)malloc(sizeof(es_node));
	strcpy(new_node->sym_name, sym_name);
	new_node->sym_addr = sym_addr;
	new_node->cs_length = length;
	new_node->link = NULL;

	new_node->link = external_table[hash_idx];
	external_table[hash_idx] = new_node;
}

int loaderPass1(int prog_addr, char *file_one, char *file_two, char *file_three) {
	FILE *obj_fp[3] = { NULL, };
	int cs_addr = prog_addr;
	int cs_len = 0;
	int read_addr = 0;
	int i;
	int j;
	int def_len = 0;
	char obj_reader[81];
	char symbol_name[8];
	char *symbol_str;
	char addr_reader[8];

	if (file_one != NULL) {
		obj_fp[0] = fopen(file_one, "r");
		if (obj_fp[0] == NULL) {
			printf("ERROR : .obj file doesn't exist.\n");
			return -1;
		}
	}
	if (file_two != NULL) {
		obj_fp[1] = fopen(file_two, "r");
		if (obj_fp[1] == NULL) {
			printf("ERROR : .obj file doesn't exist.\n");
			return -1;
		}
	}
	if (file_three != NULL) {
		obj_fp[2] = fopen(file_three, "r");
		if (obj_fp[2] == NULL) {
			printf("ERROR : .obj file doesn't exist.\n");
			return -1;
		}
	}

	for (i = 0; i < 3;i++) {
		if (obj_fp[i] != NULL) {
			while (fgets(obj_reader,80,obj_fp[i]) != NULL) {
				if (obj_reader[0] == '.') {
					continue;
				} // comment
				if (obj_reader[0] == 'H') {
					memset(symbol_name, '\0', 8);
					strncpy(symbol_name, obj_reader + 1, 6);
					symbol_name[6] = '\0';
					symbol_str = strtok(symbol_name, " \n\t");
					// control section name

					memset(addr_reader, '\0', 8);
					strncpy(addr_reader, obj_reader + 7, 6);
					addr_reader[6] = '\0';
					cs_addr += strtol(addr_reader, NULL, 16);
					// start address

					memset(addr_reader, '\0', 8);
					strncpy(addr_reader, obj_reader + 13, 6);
					addr_reader[6] = '\0';
					cs_len = strtol(addr_reader, NULL, 16);
					// length

					if (searchESTAB(symbol_str) != -1) {
						printf("ERROR : symbol %s is already in External Symbol Table.\n",symbol_str);
						return -1;
					} // aldready
					else {
						insertESTAB(symbol_str, cs_addr, cs_len);
					} // insert
					// find the symbol
				} // Header. Read the control section's name, start address and length.
				else if (obj_reader[0] == 'D') {
					def_len = (int)(strlen(obj_reader) - 1) / 12;
					
					for (j = 0; j < def_len;j++) {
						memset(symbol_name, '\0', 8);
						strncpy(symbol_name, obj_reader + 1 + (j * 12), 6);
						symbol_name[6] = '\0';
						symbol_str = strtok(symbol_name, " \n\t");
						// symbol name

						memset(addr_reader, '\0', 8);
						strncpy(addr_reader, obj_reader + 7 + (j * 12), 6);
						addr_reader[6] = '\0';
						read_addr = strtol(addr_reader, NULL, 16);
						// address

						if (searchESTAB(symbol_str) != -1) {
							printf("ERROR : symbol %s is already in External Symbol Table.\n", symbol_str);
							return -1;
						} // already in
						else {
							insertESTAB(symbol_str, cs_addr + read_addr, 0);
						} // insert
					}
				} // Definition
				else if (obj_reader[0] == 'E') {
					cs_addr += cs_len;
					break;
				} // End of .obj file
				else if (obj_reader[0] == 'M' || obj_reader[0] == 'R' || obj_reader[0] == 'T') {
					continue;
				}
				else {
					printf("ERROR : Wrong type record\n");
					return -1;
				}
			} // read .obj file
		} // if there is .obj file
	}

	for (i = 0; i < 3; i++) {
		if (obj_fp[i] != NULL) {
			fclose(obj_fp[i]);
		}
	}

	return 1;
}

void loaderMod(int mod_addr,int mod_hbyte,char mod_type,int symbol_addr) {
	int mem_row = mod_addr / 16;
	int mem_col = mod_addr % 16;
	int mod_val = 0;
	int hbyte = mod_hbyte;
	unsigned char mem_adder;

	while (hbyte > 0) {
		if (hbyte % 2 == 1) {
			mod_val <<= 4;
			mod_val += (int)mem_arr[mem_row][mem_col] % 16;
			--hbyte;
		}
		else {
			mod_val <<= 8;
			mod_val += (int)mem_arr[mem_row][mem_col];
			hbyte -= 2;
		}

		++mem_col;
		if (mem_col == 16) {
			++mem_row;
			mem_col = 0;
		}
	}
	// modify value
	if (mod_type == '+') {
		mod_val += symbol_addr;
	} // add addr
	else if (mod_type == '-') {
		mod_val -= symbol_addr;
	} // sub addr

	hbyte = mod_hbyte;
	while (hbyte > 0) {
		mem_adder = 0;

		--mem_col;
		if (mem_col == -1) {
			--mem_row;
			mem_col = 15;
		}
		if (hbyte == 1) {
			mem_arr[mem_row][mem_col] ^= 0xFF00;
			mem_arr[mem_row][mem_col] += (unsigned char)(mod_val % 16);
			--hbyte;
		} // for hbyte 5
		else {
			mem_adder = (unsigned char)(mod_val % 256);
			mem_arr[mem_row][mem_col] = mem_adder;
			mod_val >>= 8;
			hbyte -= 2;
		} // for hbyte 6
	}
}

void insertRefList(int ref_num, char *ref_name) {
	ref_ptr new_ref = NULL;

	new_ref = (ref_ptr)malloc(sizeof(ref_node));
	new_ref->ref_num = ref_num;
	strcpy(new_ref->ref_sym,ref_name);
	new_ref->link = NULL;

	if (ref_front == NULL) {
		ref_front = new_ref;
		ref_rear = new_ref;
	}
	else {
		ref_rear->link = new_ref;
		ref_rear = new_ref;
	}
}

void deallRefList() {
	ref_ptr deall_ptr, temp_ptr;

	temp_ptr = ref_front;
	while (temp_ptr != NULL) {
		deall_ptr = temp_ptr;
		temp_ptr = temp_ptr->link;
		free(deall_ptr);
	}

	ref_front = ref_rear = NULL;
}

ref_ptr searchRefList(int ref_num) {
	ref_ptr search_ptr = ref_front;

	while (search_ptr != NULL) {
		if (search_ptr->ref_num == ref_num) {
			return search_ptr;
		}
		search_ptr = search_ptr->link;
	}

	return NULL;
}

int loaderPass2(int prog_addr, char *file_one, char *file_two, char *file_three, int *exec_addr,int *end_addr) {
	FILE *obj_fp[3] = { NULL, };
	int cs_addr = prog_addr;
	int cs_len = 0;
	int start_addr = 0, mod_addr = 0;
	int byte_num = 0, byte_val = 0, mod_hbyte = 0, symbol_addr = 0;
	char obj_reader[81];
	char symbol_name[8];
	char *symbol_str;
	char ref_num_str[4],ref_name[8];
	char *ref_name_str;
	int ref_len, ref_num;
	char mod_num_str[4];
	int mod_sym_num;
	ref_ptr mod_ref;
	char target_reader[8];
	char byte_reader[4];
	char mod_type;
	int i,j;

	if (file_one != NULL) {
		obj_fp[0] = fopen(file_one, "r");
		if (obj_fp[0] == NULL) {
			printf("ERROR : .obj file doesn't exist.\n");
			return -1;
		}
	}
	if (file_two != NULL) {
		obj_fp[1] = fopen(file_two, "r");
		if (obj_fp[1] == NULL) {
			printf("ERROR : .obj file doesn't exist.\n");
			return -1;
		}
	}
	if (file_three != NULL) {
		obj_fp[2] = fopen(file_three, "r");
		if (obj_fp[2] == NULL) {
			printf("ERROR : .obj file doesn't exist.\n");
			return -1;
		}
	}

	*exec_addr = prog_addr;
	for (i = 0; i < 3;i++) {
		if (obj_fp[i] != NULL) {
			deallRefList();
			while (fgets(obj_reader,80,obj_fp[i]) != NULL) {
				if (obj_reader[0] == '.') {
					continue;
				} // comment
				else if (obj_reader[0] == 'H') {
					memset(symbol_name, '\0', 8);
					strncpy(symbol_name, obj_reader + 1, 6);
					symbol_name[6] = '\0';
					symbol_str = strtok(symbol_name, " \n\t");
					insertRefList(1,symbol_str);
					// control section name

					strncpy(target_reader, obj_reader + 13, 6);
					target_reader[6] = '\0';
					cs_len = strtol(target_reader, NULL, 16);
					// control section length
				}
				else if (obj_reader[0] == 'T') {
					strncpy(target_reader, obj_reader + 1, 6);
					target_reader[6] = '\0';
					start_addr = strtol(target_reader,NULL,16);
					// start address
					
					memset(byte_reader,'\0',4);
					strncpy(byte_reader, obj_reader + 7, 2);
					byte_num = strtol(byte_reader, NULL, 16);
					// byte length

					for (j = 0; j < byte_num;j++) {
						memset(byte_reader, '\0', 4);
						strncpy(byte_reader,obj_reader + 9 + (j*2),2);
						byte_val = strtol(byte_reader, NULL, 16);
						mem_edit(cs_addr+start_addr, byte_val);
						++start_addr;
					}
					// edit
				}
				else if (obj_reader[0] == 'M') {
					strncpy(target_reader, obj_reader + 1, 6);
					target_reader[6] = '\0';
					mod_addr = strtol(target_reader, NULL, 16) + cs_addr;
					// modification address

					memset(byte_reader, '\0', 4);
					strncpy(byte_reader, obj_reader + 7, 2);
					mod_hbyte = strtol(byte_reader, NULL, 16);
					// half byte len

					mod_type = obj_reader[9]; // modification calculation option (plus or minus)

					memset(mod_num_str, '\0', 4);
					strcpy(mod_num_str,obj_reader+10);
					mod_sym_num = strtol(mod_num_str, NULL, 16);
					mod_ref = searchRefList(mod_sym_num);
					symbol_addr = searchESTAB(mod_ref->ref_sym);
					if (symbol_addr == -1) {
						printf("ERROR : Symbol %s is not in External Symbol Table.\n",symbol_name);
						return -1;
					} // cannot find the symbol
					else {
						loaderMod(mod_addr, mod_hbyte, mod_type, symbol_addr);
					}
				}
				else if (obj_reader[0] == 'E') {
					if (strlen(obj_reader) > 1) {
						strncpy(target_reader, obj_reader + 1, 6);
						target_reader[6] = '\0';
						*exec_addr = strtol(target_reader, NULL, 16) + cs_addr;
					} // if there is specified execute address
					cs_addr += cs_len;
					break;
				}
				else if (obj_reader[0] == 'R') {
					memset(ref_num_str,'\0',4);
					memset(ref_name, '\0', 8);
					ref_len = (int)(strlen(obj_reader) - 1);

					for (j = 0; j < ref_len;j += 8) {
						strncpy(ref_num_str, obj_reader + j + 1, 2);
						ref_num = strtol(ref_num_str, NULL, 16);
						strncpy(ref_name, obj_reader + j + 3, 6);
						ref_name_str = strtok(ref_name, " \n\t");
						insertRefList(ref_num, ref_name_str);
					}
				}
				else if (obj_reader[0] == 'D') {
					continue;
				}
				else {
					printf("ERROR : Wrong type record\n");
					return -1;
				}
			}
		}
	}

	*end_addr = cs_addr;
	for (i = 0; i < 3;i++) {
		if (obj_fp[i] != NULL) {
			fclose(obj_fp[i]);
		}
	}
	
	return 1;
}

void printESTAB() {
	es_ptr print_list = NULL;
	es_ptr search_list;
	es_ptr temp_node = NULL;
	es_ptr first, second;
	int i;
	int total_len = 0;

	for (i = 0; i < HASH_SIZE;i++) {
		if (external_table[i] != NULL) {
			search_list = external_table[i];

			while (search_list != NULL) {
				temp_node = (es_ptr)malloc(sizeof(es_node));
				strcpy(temp_node->sym_name, search_list->sym_name);
				temp_node->sym_addr = search_list->sym_addr;
				temp_node->cs_length = search_list->cs_length;
				temp_node->link = NULL;

				if (print_list == NULL) {
					print_list = temp_node;
				}
				else if (print_list->sym_addr > temp_node->sym_addr) {
					temp_node->link = print_list;
					print_list = temp_node;
				}
				else {
					first = print_list;
					second = print_list->link;

					while (second != NULL) {
						if (second->sym_addr > temp_node->sym_addr) {
							temp_node->link = second;
							first->link = temp_node;
							break;
						}
						first = first->link;
						second = second->link;
					}
					if (second == NULL) {
						first->link = temp_node;
					}
				}
				search_list = search_list->link;
			}
		}
	} // make the list to print

	printf("control\t\tsymbol\t\taddress\t\tlength\n");
	printf("section\t\tname\n");
	printf("---------------------------------------------------------------------\n");
	search_list = print_list;
	while (search_list != NULL) {
		if (search_list->cs_length != 0) {
			printf("%s\t\t\t\t",search_list->sym_name);
			printf("%04X\t\t%04X\n",search_list->sym_addr,search_list->cs_length);
			total_len += search_list->cs_length;
		}
		else {
			printf("\t\t%s\t\t",search_list->sym_name);
			printf("%04X\n",search_list->sym_addr);
		}
		search_list = search_list->link;
	}
	printf("---------------------------------------------------------------------\n");
	printf("                                total length    %04X\n",total_len);

	while (print_list != NULL) {
		temp_node = print_list;
		print_list = print_list->link;
		free(temp_node);
	}
}

int loaderBundle(int prog_addr, char *file_one, char *file_two, char *file_three, int *exec_addr,int *end_addr) {
	int err_tag;

	deallESTAB();

	err_tag = loaderPass1(prog_addr, file_one, file_two, file_three);
	if (err_tag == 1) {
		err_tag = loaderPass2(prog_addr, file_one, file_two, file_three,exec_addr,end_addr);
		
		if (err_tag == 1) {
			printESTAB();
		}
	}

	return err_tag;
}
