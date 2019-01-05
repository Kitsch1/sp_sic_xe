#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

#define MANY_INPUTS -1
#define FEW_INPUTS -2
#define WRONG_INPUTS -3
#define WRONG_MNEMONIC -4
#define WRONG_BOUNDARY -5
#define NO_FILE_EXIST -6
#define HASH_SIZE 20
#define SYMBOL_TABLE_SIZE 26
#define MEM_ROW_SIZE 65536
#define MEM_COL_SIZE 16

// history linked list's node
typedef struct history_node {
	char his_mem[256];
	struct history_node *link;
}history_node;

// opcode hash table's node
typedef struct opcode_node {
	int op_num;
	char op_chr[8];
	char op_mode[8];
	struct opcode_node *link;
}opcode_node;

// symbol hash table
typedef struct symbol_node {
	int locctr;
	char symbol_str[16];
	struct symbol_node *link;
}symbol_node;

typedef struct bp_node {
	int bp_loc;
	struct bp_node *link;
}bp_node;

typedef struct es_node {
	char sym_name[8];
	int sym_addr;
	int cs_length;
	struct es_node *link;
}es_node;

typedef struct ref_node {
	int ref_num;
	char ref_sym[8];
	struct ref_node *link;
}ref_node;

long long int sic_reg[10];
long long int sic_reg_history[10];

typedef struct history_node* history_ptr; // history node pointer
typedef struct opcode_node* opcode_ptr; // opcode node pointer
typedef struct symbol_node* symbol_ptr; // symbol node pointer
typedef struct bp_node *bp_ptr;
typedef struct es_node *es_ptr;
typedef struct ref_node *ref_ptr;

// global variables (history, opcode hash table)
history_ptr his_list_front;
history_ptr his_list_rear;
opcode_ptr opcode_table[HASH_SIZE];

// global variables (memory)
unsigned char mem_arr[MEM_ROW_SIZE][MEM_COL_SIZE];

// global variables (symbol table)
symbol_ptr last_symbol_table[SYMBOL_TABLE_SIZE];

// global variables (breakpoint list)
bp_ptr bp_front;
bp_ptr bp_rear;
int bp_cnt;
int bp_idx;

// global variables (External Symbol Table)
es_ptr external_table[HASH_SIZE];

// global variables (Reference List)
ref_ptr ref_front, ref_rear;

// This functions are used for the manipulation of list, table and memory.
void opcode_init();
void opcode_table_print();
int opcode_find(char*,int);
void opcode_free();
void history_add(char*);
void history_free();
void mem_reset();
void mem_dump_only(int*);
void mem_dump_first_only(int,int*);
void mem_dump_first_last(int,int,int*);
void mem_edit(int,int);
void mem_fill(int,int,int);
int mem_hexa_convert(char*);

// This functions are used for the main menu of SIC.
void sic_menu();
int sic_menu_classify(char**,char*,int*,int*,int*,int*,int*);
char *trim_left(char*);
char *trim_right(char*);

// This functions are used for the execution of command.
void sic_help();
void sic_history_print();
int sic_dir();

// This functions are used for project 2, creating .lst file and .obj file.
int sicType(char*);
int asmCreate(char*);
int asmFirstPass(char*);
int asmSecondPass(char*,int*,int*,int*);
int opSearch(char*);

// for creating objcode for .lst file
int objFormatTwo(int,char*,char*);
int objFormatThree(int,char*,char*,int,int,int);
int objFormatFour(int,char*,char*,int);
int opcodeNumberSearch(char*);
int objFileMaking(char*,int,int);

// This functions are used for project 2, used for symbol.
void freeSymbolTable(); 
void printSymbolTable();
int symbolTableSearch(char*);
void symbolTableInsert(int, char*);

// This functions are used for project 3, used for breakpoint.
void printBreakPoint();
int insertBreakPoint(char*);
void clearBreakPoint();
int searchBreakPoint();

// This functions are used for project 3, used for loader.
int loaderBundle(int,char*,char*,char*,int*,int*);
int loaderPass1(int,char*,char*,char*);
int loaderPass2(int,char*,char*,char*,int*,int*);
void loaderMod(int,int,char,int);

// This functions are used for project 3, used for run.
int runProgram(int,int,int*);
void saveRegHistory(int);
int opcodeFormat(int);
int formatTwoCheck(int,int);
long long int readMemory(long long int,long long int,int,int);
void storeMemoryFormat3(long long int,int,int,int);

// This functios are used for project 3, used for external symbol table.
int searchESTAB(char*);
void insertESTAB(char*,int,int);
void deallESTAB();
void printESTAB();

// This functions are used for project 3, used for reference list.
void insertRefList(int,char*);
ref_ptr searchRefList(int);
void deallRefList();

// This function is used for the notice of error.
void err_msg_bundle(int);
