#include "20121578.h"

void saveRegHistory(int flag) {
	int i;

	if (flag == 0) {
		for (i = 0; i < 10; i++) {
			sic_reg_history[i] = sic_reg[i];
		}
	}
	else {
		for (i = 0; i < 10; i++) {
			sic_reg[i] = sic_reg_history[i];
		}
	}
}

int formatTwoCheck(int reg_1,int reg_2) {
	if (reg_1 > 9 || reg_2 > 9 || reg_1 == 7 || reg_2 == 7) {
		return -1;
	} // 7 is not valid
	else {
		return 1;
	}
}

int opcodeFormat(int opcode_num) {
	int i;
	opcode_ptr search_list;
	
	// get the opcode number and search.
	for (i = 0; i < HASH_SIZE;i++) {
		if (opcode_table[i] != NULL) {
			search_list = opcode_table[i];

			while (search_list != NULL) {
				if (search_list->op_num == opcode_num) {
					if (!strcmp(search_list->op_mode,"1")) {
						return 1;
					}
					else if (!strcmp(search_list->op_mode,"2")) {
						return 2;
					}
					else {
						return 3;
					}
				}// if find, return the number of format.
				search_list = search_list->link;
			}
		}
	}

	return -1; // if cannot find
}

void storeMemoryFormat3(long long int mem_start, int reg_num,int addr_mode,int extend_flag) {
	unsigned char st_val[3] = {'\0',};
	int i;
	long long int mem_row, mem_col;
	long long int indirect_mem;

	st_val[0] = (unsigned char)((sic_reg[reg_num] & 0x000000FF0000) >> 16);
	// first byte of register
	st_val[1] = (unsigned char)((sic_reg[reg_num] & 0x00000000FF00) >> 8);
	// second byte of register
	st_val[2] = (unsigned char)((sic_reg[reg_num] & 0x0000000000FF));
	// third byte of register

	if (addr_mode == 3) {
		mem_row = mem_start / 16;
		mem_col = mem_start % 16;

		for (i = 0; i < 3;i++) {
			mem_arr[mem_row][mem_col++] = st_val[i];
			if (mem_col == 16) {
				mem_col = 0;
				mem_row++;
			}
		}
	} // simple addressing
	else if (addr_mode == 2) {
		indirect_mem = readMemory(mem_start, mem_start + 2, addr_mode, extend_flag);

		mem_row = indirect_mem / 16;
		mem_col = indirect_mem % 16;
		
		for (i = 0; i < 3; i++) {
			mem_arr[mem_row][mem_col++] = st_val[i];
			if (mem_col == 16) {
				mem_col = 0;
				mem_row++;
			}
		}
	} // indirect addressing
	else if (addr_mode == 1) {
		return;
	} // immediate mode
	else {
		if (extend_flag == 1) {
			return;
		}
		else {
			return;
		}
	}
}

long long int readMemory(long long int mem_start,long long int mem_end,int addr_mode,int extend_flag) {
	long long int mem_row = mem_start / 16;
	long long int mem_col = mem_start % 16;
	long long int mem_val = 0;
	int i;
	int loop_num = (int)(mem_start - mem_end);

	for (i = 0; i < loop_num;i++) {
		if (mem_col + i >= 16) {
			mem_val <<= 8;
			mem_val += (long long int)mem_arr[mem_row + 1][mem_col + i - 16];
		}
		else {
			mem_val <<= 8;
			mem_val += (long long int)mem_arr[mem_row][mem_col];
		}
	} // read memory's value

	if (addr_mode == 3) {
		return mem_val;
	} // simple addressing
	else if (addr_mode == 2) {
		mem_row = mem_val / 16;
		mem_col = mem_val % 16;
		mem_val = 0;
		
		for (i = 0; i < loop_num; i++) {
			if (mem_col + i >= 16) {
				mem_val <<= 8;
				mem_val += (long long int)mem_arr[mem_row + 1][mem_col + i - 16];
			}
			else {
				mem_val <<= 8;
				mem_val += (long long int)mem_arr[mem_row][mem_col];
			}
		}
		
		return mem_val;
	} // indirect addressing
	else if (addr_mode == 1) {
		return mem_start;
	} // immediate
	else {
		if (extend_flag == 1) {
			return mem_val;
		}
		else {
			return -1;
		}
	} // SIC Mode
}

int runProgram(int very_start_addr,int exec_end_addr,int *prog_run_flag) {
	long long int cur_start_addr;
	long long int cur_end_addr;
	long long int mem_row, mem_col;
	long long int target_val;
	unsigned char op_chr[4];
	unsigned char right_most;
	int i;
	int opcode_num = 0;
	int cur_fmt = 0; // format
	int addr_mode = 0; // address mode
	int reg_1 = 0, reg_2 = 0; // for format 2
	int xbpe = 0;
	int idx_flag = 0, b_flag = 0, pc_flag = 0, extend_flag = 0; // for format 3 or 4
	long long int disp = 0;
	long long int target_addr = 0;
	int err_flag = 1;

	if (*prog_run_flag == 0) {
		cur_start_addr = (long long int)very_start_addr;
		if (bp_cnt == 0) {
			cur_end_addr = (long long int)exec_end_addr;
		} // if there is no breakpoint
		else {
			cur_end_addr = (long long int)searchBreakPoint();
		} // if there is any breakpoint
	} // very first run
	else {
		cur_start_addr = sic_reg[8];
		if (bp_idx + 1 == bp_cnt) {
			cur_end_addr = (long long int)exec_end_addr;
		} // if there is last breakpoint
		else {
			bp_idx++;
			cur_end_addr = (long long int)searchBreakPoint();
		}
	} // on the run

	if (*prog_run_flag == 0) {
		for (i = 0; i < 10;i++) {
			if (i == 8) {
				sic_reg[i] = cur_start_addr;
			}
			else {
				sic_reg[i] = 0;
			}
		}
		sic_reg[2] = sic_reg[8];
	} // initialize the registers
	saveRegHistory(0); // for error case

	while (sic_reg[8] < cur_end_addr) {
		mem_row = sic_reg[8] / 16;
		mem_col = sic_reg[8] % 16;

		op_chr[0] = mem_arr[mem_row][mem_col];
		addr_mode = (int)(op_chr[0] & 0x03);
		opcode_num = (int)(op_chr[0]) - addr_mode;

		cur_fmt = opcodeFormat(opcode_num);
		
		if (cur_fmt == 1) {
			sic_reg[8]++;
		} // format 1
		else if (cur_fmt == 2) {
			sic_reg[8] += 2; // fetch instruction and increase program counter
			
			if (mem_col + 1 == 16) {
				mem_row++;
				mem_col = 0;
			}
			else {
				mem_col++;
			}
			op_chr[1] = mem_arr[mem_row][mem_col]; // register number
			reg_1 = (int)((op_chr[1] & 0xF0) >> 4);
			reg_2 = (int)(op_chr[1] & 0x0F);

			if (formatTwoCheck(reg_1, reg_2) == -1) {
				printf("ERROR : Invalid register number at %04llX\n", sic_reg[8] - 1);
				err_flag = -1;
				break;
			} // invalid register number

			if (opcode_num == 0x90) {
				sic_reg[reg_2] += sic_reg[reg_1];
			} // ADDR
			else if (opcode_num == 0xB4) {
				sic_reg[reg_1] = 0;
			} // CLEAR
			else if (opcode_num == 0xA0) {
				if (sic_reg[reg_1] > sic_reg[reg_2]) {
					sic_reg[9] = 1;
				}
				else if (sic_reg[reg_1] == sic_reg[reg_2]) {
					sic_reg[9] = 0;
				}
				else {
					sic_reg[9] = -1;
				}
			} // COMPR
			else if (opcode_num == 0x9C) {
				sic_reg[reg_2] /= sic_reg[reg_1];
			} // DIVR
			else if (opcode_num == 0x98) {
				sic_reg[reg_2] *= sic_reg[reg_1];
			} // MULR
			else if (opcode_num == 0xAC) {
				sic_reg[reg_2] = sic_reg[reg_1];
			} // RMO
			else if (opcode_num == 0xA4) {
				sic_reg[reg_1] <<= reg_2;
			} // SHIFTL
			else if (opcode_num == 0x94) {
				sic_reg[reg_2] -= sic_reg[reg_1];
			} // SUBR
			else if (opcode_num == 0xB0) {
				continue;
			} // SVC
			else if (opcode_num == 0xB8) {
				sic_reg[1]++;
				if (sic_reg[reg_1] > sic_reg[sic_reg[1]]) {
					sic_reg[9] = 1;
				}
				else if (sic_reg[reg_1] == sic_reg[sic_reg[1]]) {
					sic_reg[9] = 0;
				}
				else {
					sic_reg[9] = -1;
				}
			} // TIXR
		}
		else if (cur_fmt == 3) { // format 3
			disp = 0;
			target_addr = 0;
			if (mem_col + 1 == 16) {
				mem_row++;
				mem_col = 0;
			}
			else {
				mem_col++;
			}
			op_chr[1] = mem_arr[mem_row][mem_col]; // get the second byte
			
			if (mem_col + 1 == 16) {
				mem_row++;
				mem_col = 0;
			}
			else {
				mem_col++;
			}
			op_chr[2] = mem_arr[mem_row][mem_col];

			xbpe = (int)((op_chr[1] & 0xF0) >> 4);
			if (xbpe >= 8) {
				idx_flag = 1;
			} // index
			b_flag = xbpe & 0x00000004;
			if (b_flag > 0) {
				b_flag = 1;
			} // base
			pc_flag = xbpe & 0x00000002;
			if (pc_flag > 0) {
				pc_flag = 1;
			} // pc
			extend_flag = xbpe & 0x00000001;


			if (extend_flag == 1) {
				if (mem_col + 1 == 16) {
					mem_row++;
					mem_col = 0;
				}
				else {
					mem_col++;
				}
				op_chr[3] = mem_arr[mem_row][mem_col];
				sic_reg[8] += 4; // add pc

				target_addr += (long long int)(op_chr[1] & 0x0F);
				target_addr <<= 8;
				target_addr += (long long int)(op_chr[2]);
				target_addr <<= 8;
				target_addr += (long long int)(op_chr[3]);

				if (idx_flag == 1) {
					disp += sic_reg[1]; // add idx
				}
				if (b_flag == 1) {
					disp += sic_reg[3]; // add base
				}
				if (pc_flag == 1) {
					disp += sic_reg[8]; // add pc
				}
			}
			else {
				sic_reg[8] += 3; // add pc

				disp += (long long int)(op_chr[1] & 0x0F);
				disp <<= 8;
				disp += (long long int)(op_chr[2]);

				if (idx_flag == 1) {
					disp += sic_reg[1]; // add idx
				}
				if (b_flag == 1) {
					disp += sic_reg[3];
				}
				if (pc_flag == 1) {
					disp += sic_reg[8];
				}
				target_addr = disp; // make target address
			}

			if (opcode_num == 0x18) {
				sic_reg[0] += readMemory(target_addr, target_addr + 2, addr_mode, extend_flag);
			} // ADD
			else if (opcode_num == 0x58) {
				continue;
			} // ADDF
			else if (opcode_num == 0x40) {
				sic_reg[0] &= readMemory(target_addr, target_addr + 2, addr_mode, extend_flag);
			} // AND
			else if (opcode_num == 0x28) {
				target_val = readMemory(target_addr, target_addr + 2, addr_mode, extend_flag);
				if (sic_reg[0] > target_val) {
					sic_reg[9] = 1;
				}
				else if (sic_reg[0] == target_val) {
					sic_reg[9] = 0;
				}
			} // COMP
			else if (opcode_num == 0x88) {
				continue;
			} // COMPF
			else if (opcode_num == 0x24) {
				sic_reg[0] /= readMemory(target_addr, target_addr + 2, addr_mode, extend_flag);
			} // DIV
			else if (opcode_num == 0x64) {
				continue;
			} // DIVF
			else if (opcode_num == 0x3C) {
				if (extend_flag == 1) {
					sic_reg[8] = target_addr + very_start_addr;
				}
				else {
					sic_reg[8] = target_addr;
				}
			} // J
			else if (opcode_num == 0x30) {
				if (sic_reg[9] == 0) {
					if (extend_flag == 1) {
						sic_reg[8] = target_addr + very_start_addr;
					}
					else {
						sic_reg[8] = target_addr;
					}
				}
			} // JEQ
			else if (opcode_num == 0x34) {
				if (sic_reg[9] == 1) {
					if (extend_flag == 1) {
						sic_reg[8] = target_addr + very_start_addr;
					}
					else {
						sic_reg[8] = target_addr;
					}
				}
			} // JGT
			else if (opcode_num == 0x38) {
				if (sic_reg[9] == -1) {
					if (extend_flag == 1) {
						sic_reg[8] = target_addr + very_start_addr;
					}
					else {
						sic_reg[8] = target_addr;
					}
				}
			} // JLT
			else if (opcode_num == 0x48) {
				sic_reg[2] = sic_reg[8];
				if (extend_flag == 1) {
					sic_reg[8] = target_addr + very_start_addr;
				}
				else {
					sic_reg[8] = target_addr;
				}
			} // JSUB
			else if (opcode_num == 0x00) {
				sic_reg[0] = readMemory(target_addr,target_addr+2,addr_mode,extend_flag);
			} // LDA
			else if (opcode_num == 0x68) {
				sic_reg[3] = readMemory(target_addr,target_addr+2,addr_mode,extend_flag);
			} // LDB
			else if (opcode_num == 0x50) {
				target_val = readMemory(target_addr,target_addr,addr_mode,extend_flag);
				sic_reg[0] &= 0xFFFFFFFFFF00;
				sic_reg[0] += target_val;
			} // LDCH
			else if (opcode_num == 0x70) {
				continue;
			} // LDF
			else if (opcode_num == 0x08) {
				sic_reg[2] = readMemory(target_addr,target_addr+2,addr_mode,extend_flag);
			} // LDL
			else if (opcode_num == 0x6C) {
				sic_reg[4] = readMemory(target_addr,target_addr+2,addr_mode,extend_flag);
			} // LDS
			else if (opcode_num == 0x74) {
				sic_reg[5] = readMemory(target_addr,target_addr+2,addr_mode,extend_flag);
			} // LDT
			else if (opcode_num == 0x04) {
				sic_reg[1] = readMemory(target_addr,target_addr+2,addr_mode,extend_flag);
			} // LDX
			else if (opcode_num == 0xD0) {
				continue;
			} // LPS
			else if (opcode_num == 0x20) {
				sic_reg[0] *= readMemory(target_addr,target_addr+2,addr_mode,extend_flag);
			} // MUL
			else if (opcode_num == 0x60) {
				continue;
			} // MULF
			else if (opcode_num == 0x44) {
				sic_reg[0] |= readMemory(target_addr,target_addr+2,addr_mode,extend_flag);
			} // OR
			else if (opcode_num == 0xD8) {
				target_val = readMemory(target_addr, target_addr, addr_mode, extend_flag);
				sic_reg[0] &= 0xFFFFFFFFFF00;
				sic_reg[0] += target_val;
			} // RD
			else if (opcode_num == 0x4C) {
				sic_reg[8] = sic_reg[2];
			} // RSUB
			else if (opcode_num == 0xEC) {
				continue;
			} // SSK
			else if (opcode_num == 0x0C) {
				storeMemoryFormat3(target_addr, 0, addr_mode, extend_flag);
			} // STA
			else if (opcode_num == 0x78) {
				storeMemoryFormat3(target_addr, 3, addr_mode, extend_flag);
			} // STB
			else if (opcode_num == 0x54) {
				right_most = (unsigned char)(sic_reg[0] & 0x0000000000FF);
				mem_arr[target_addr / 16][target_addr % 16] = right_most;
			} // STCH
			else if (opcode_num == 0x80) {
				continue;
			} // STF
			else if (opcode_num == 0xD4) {
				continue;
			} // STI
			else if (opcode_num == 0x14) {
				storeMemoryFormat3(target_addr, 2, addr_mode, extend_flag);
			} // STL
			else if (opcode_num == 0x7C) {
				storeMemoryFormat3(target_addr, 4, addr_mode, extend_flag);
			} // STS
			else if (opcode_num == 0xE8) {
				storeMemoryFormat3(target_addr, 9, addr_mode, extend_flag);
			} // STSW
			else if (opcode_num == 0x84) {
				storeMemoryFormat3(target_addr, 5, addr_mode, extend_flag);
			} // STT
			else if (opcode_num == 0x10) {
				storeMemoryFormat3(target_addr, 1, addr_mode, extend_flag);
			} // STX
			else if (opcode_num == 0x1C) {
				sic_reg[0] -= readMemory(target_addr,target_addr+2,addr_mode,extend_flag);
			} // SUB
			else if (opcode_num == 0x5C) {
				continue;
			} // SUBF
			else if (opcode_num == 0xE0) {
				continue;
			} // TD
			else if (opcode_num == 0x2C) {
				sic_reg[1]++;
				target_val = readMemory(target_addr, target_addr + 2, addr_mode, extend_flag);
				if (sic_reg[1] > target_val) {
					sic_reg[9] = 1;
				}
				else if (sic_reg[1] == target_val) {
					sic_reg[9] = 0;
				}
				else {
					sic_reg[9] = -1;
				}
			} // TIX
			else if (opcode_num == 0xDC) {
				right_most = (unsigned char)(sic_reg[0] & 0x0000000000FF);
				mem_arr[target_addr / 16][target_addr % 16] = right_most;
			} // WD
			// need to add
		}
		else {
			err_flag = -1;
		} // set error flag
	}

	printf("A : %012llX  X : %012llX\n",sic_reg[0],sic_reg[1]);
	printf("L : %012llX  PC: %012llX\n",sic_reg[2],sic_reg[8]);
	printf("B : %012llX  S : %012llX\n",sic_reg[3],sic_reg[4]);
	printf("T : %012llX\n",sic_reg[5]);
	// print the value of registers

	if (err_flag == -1) {
		saveRegHistory(-1);
		bp_idx--;
	}
	else {
		if (sic_reg[8] < exec_end_addr) {
			*prog_run_flag = 1;
		}
		else {
			*prog_run_flag = 0;
			bp_idx = 0;
		}
	}
	if (*prog_run_flag == 1) {
		printf("Stop at checkpoint[%04X]\n",searchBreakPoint());
	}
	else {
		printf("End Program\n");
	}

	return err_flag;
}
