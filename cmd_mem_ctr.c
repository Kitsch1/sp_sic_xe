#include "20121578.h"

void mem_reset() {
	int i, j;

	for (i = 0; i < MEM_ROW_SIZE; i++) {
		for (j = 0; j < MEM_COL_SIZE; j++) {
			mem_arr[i][j] = 0;
		}
	}
} // reset the memory

int mem_hexa_convert(char *num_str) {
	int i,j;
	int num_len = strlen(num_str);
	int cur_idx = 0;
	int num_res = 0;
	int hexa_start = 1;

	for (i = num_len - 1; i >= 0;i--) {
		if(cur_idx < 5){
			if (num_str[i] >= 48 && num_str[i] <= 57) {
				num_res += hexa_start * (num_str[i] - 48);
			} // char 0~9
			else if (num_str[i] >= 65 && num_str[i] <= 70) {
				num_res += hexa_start * (num_str[i] - 55);
			} // char A~F
			else if (num_str[i] >= 97 && num_str[i] <= 102) {
				num_res += hexa_start * (num_str[i] - 87);
			} // char a~f
			else {
				err_msg_bundle(WRONG_INPUTS);
				return -1;
			} // the others
			hexa_start *= 16;
			cur_idx++;
		}
		else{
			for(j=0; j < num_len - cur_idx ; j++){
				if(num_str[j] != '0'){
					err_msg_bundle(WRONG_BOUNDARY);
					return -1;
				}
			}
			// for the case like 0000000000000000000fff
		}
	}

	return num_res; // return converted address or ASCII code
}

void mem_dump_only(int *cur_mem_ctr) {
	int du_start, du_end;
	du_start = *cur_mem_ctr;
	
	/*
	if (du_start == 0xfffff) {
		du_start = 0;
	}
	*/
	if (du_start + 159 > 0xfffff) {
		du_end = 0xfffff;
	} // dump ends at FFFFF if du_start + 159 over FFFFF
	else {
		du_end = du_start + 159;
	}
	mem_dump_first_last(du_start, du_end, cur_mem_ctr);
}

void mem_dump_first_only(int du_start, int *cur_mem_ctr) {
	int du_end;

	if (du_start + 159 > 0xfffff) {
		du_end = 0xfffff;
	} // dump ends at FFFFF if du_start + 159 over FFFFF
	else {
		du_end = du_start + 159;
	}
	mem_dump_first_last(du_start,du_end,cur_mem_ctr);
}

void mem_dump_first_last(int du_start, int du_end, int *cur_mem_ctr) {
	int i,j;
	int start_line, end_line, cur_line;
	int line_bound, line_bound_end;

	start_line = (du_start / 16); // start line for the print
	end_line = (du_end / 16); // end line for the print
	cur_line = (du_start / 16) * 16; // current line for the memory's start addr

	if(start_line == end_line){
		printf("%05X ",cur_line);
		line_bound = du_start % 16;
		line_bound_end = du_end % 16;

		for(j=0;j<line_bound;j++){
			printf("   ");
		} // blank
		for(j=line_bound;j<=line_bound_end;j++){
			printf("%02X ",mem_arr[start_line][j]);
		} // char
		for(j=line_bound_end+1;j<16;j++){
			printf("   ");
		} // blank
		printf(" ; ");
		for(j=0;j<16;j++){
			if(j<line_bound || j >line_bound_end){
				printf(".");
			}
			else if(mem_arr[start_line][j] >= 0x20 && mem_arr[start_line][j] <= 0x7E){
				printf("%c",mem_arr[start_line][j]);
			}
			else{
				printf(".");
			}
		}
		printf("\n");
		if(du_end == 0xfffff){
			*cur_mem_ctr = 0;
		}
		else{
			*cur_mem_ctr = du_end + 1;
		}
		return;
	}	

	for (i = start_line; i <= end_line; i++) {
		printf("%05X ",cur_line);
		if (i == start_line) {
			line_bound = (du_start % 16);
			for (j = 0; j < line_bound;j++) {
				printf("   ");
			} // blank
			for (j = line_bound; j < 16;j++) {
				printf("%02X ",mem_arr[i][j]);
			}
			// mem
			printf(" ; ");
			for (j = 0; j < 16;j++) {
				if (j < line_bound) {
					printf(".");
				}
				else if (mem_arr[i][j] >= 0x20 && mem_arr[i][j] <= 0x7E) {
					printf("%c",mem_arr[i][j]);
				}
				else {
					printf(".");
				}
			}
			// character
			// only between character 20 ~ 7E can be printed
		}
		else if (i == end_line) {
			line_bound = (du_end % 16);
			// mem 
			for (j = 0; j <= line_bound;j++) {
				printf("%02X ", mem_arr[i][j]);
			}
			// blank
			for (j = line_bound + 1; j < 16; j++) {
				printf("   ");
			}
			printf(" ; ");
			// character
			// only between character 20 ~ 7E can be printed
			for (j = 0; j < 16;j++) {
				if (j >= line_bound) {
					printf(".");
				}
				else if (mem_arr[i][j] >= 0x20 && mem_arr[i][j] <= 0x7E) {
					printf("%c", mem_arr[i][j]);
				}
				else {
					printf(".");
				}
			}
			
		}
		else {
			for (j = 0; j < 16;j++) {
				printf("%02X ",mem_arr[i][j]);
			}
			printf(" ; ");
			// mem
			for (j = 0; j < 16;j++) {
				if (mem_arr[i][j] >= 0x20 && mem_arr[i][j] <= 0x7E) {
					printf("%c", mem_arr[i][j]);
				}
				else {
					printf(".");
				}
			}
			// character
		}
		cur_line += 16;
		printf("\n");
	}
	if (du_end == 0xfffff) {
		*cur_mem_ctr = 0;
	}
	else {
		*cur_mem_ctr = du_end + 1;
	}
}

void mem_edit(int edit_mem, int edit_val) {
	int edit_row, edit_col;
	
	edit_row = edit_mem / 16;
	edit_col = edit_mem % 16;

	mem_arr[edit_row][edit_col] = edit_val;
	// edit the value of entered address
}

void mem_fill(int fill_start, int fill_end, int fill_val) {
	int i,j;
	int start_line, end_line, line_bound;

	start_line = fill_start / 16;
	end_line = fill_end / 16;

	for (i = start_line; i <= end_line;i++) {
		if (i == start_line) {
			line_bound = fill_start % 16;
			for (j = line_bound; j < 16;j++) {
				mem_arr[i][j] = fill_val;
			}
		} // if now is first line, edit from the start address.
		else if (i == end_line) {
			line_bound = fill_end % 16;
			for (j = 0; j <= line_bound;j++) {
				mem_arr[i][j] = fill_val;
			}
		} // if now is last line, edit before the end address.
		else {
			line_bound = 16;
			for (j = 0; j < line_bound;j++) {
				mem_arr[i][j] = fill_val;
			}
		} // edit all value of current line.
	}
}
