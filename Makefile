CC = gcc
CFLAGS = -Wall
TARGET = 20121578.out

$(TARGET) : 20121578.o cmd_ctr.o cmd_execution.o cmd_mem_ctr.o cmd_opcode.o asm_lst_create.o asm_obj_create.o asm_obj_format.o asm_symbol_ctr.o linkload_load_bp.o linkload_run.o
	    $(CC) $(CFLAGS) -o $(TARGET) 20121578.o cmd_ctr.o cmd_execution.o cmd_mem_ctr.o cmd_opcode.o asm_lst_create.o asm_obj_create.o asm_obj_format.o asm_symbol_ctr.o linkload_load_bp.o linkload_run.o

20121578.o : 20121578.c
	$(CC) $(CFLAGS) -c -o 20121578.o 20121578.c

cmd_ctr.o : cmd_ctr.c
	$(CC) $(CFLAGS) -c -o cmd_ctr.o cmd_ctr.c

cmd_execution.o : cmd_execution.c
	$(CC) $(CFLAGS) -c -o cmd_execution.o cmd_execution.c

cmd_mem_ctr.o : cmd_mem_ctr.c
	$(CC) $(CFLAGS) -c -o cmd_mem_ctr.o cmd_mem_ctr.c

cmd_opcode.o : cmd_opcode.c
	$(CC) $(CFLAGS) -c -o cmd_opcode.o cmd_opcode.c

asm_lst_create.o : asm_lst_create.c
	$(CC) $(CFLAGS) -c -o asm_lst_create.o asm_lst_create.c

asm_obj_create.o : asm_obj_create.c
	$(CC) $(CFLAGS) -c -o asm_obj_create.o asm_obj_create.c

asm_obj_format.o : asm_obj_format.c
	$(CC) $(CFLAGS) -c -o asm_obj_format.o asm_obj_format.c

asm_symbol_ctr.o : asm_symbol_ctr.c
	$(CC) $(CFLAGS) -c -o asm_symbol_ctr.o asm_symbol_ctr.c

linkload_load_bp.o : linkload_load_bp.c
	$(CC) $(CFLAGS) -c -o linkload_load_bp.o linkload_load_bp.c

linkload_run.o : linkload_run.c
	$(CC) $(CFLAGS) -c -o linkload_run.o linkload_run.c

clean :
	rm *.o 20121578.out
