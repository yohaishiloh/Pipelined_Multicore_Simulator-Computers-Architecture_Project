#ifndef header_H
#define header_H
#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


#define IMEM_LINE_SIZE 20
#define IMEM_MAX_LINE 1024
#define CORE_REG_NUM 16
#define DSRAM_MAX_LINE 256
#define TSRAM_MAX_LINE 64
#define NUM_CORES 4
#define MEM_SIZE 1048576


///pipeline registers structure

// IF/ID register
typedef struct {

	char instruction[IMEM_LINE_SIZE];
	int pc;
	int stateHalt;
}ifidReg;

//ID/IE register

typedef struct {
		
	int opcode;
	int rd;
	int rs;
	int rt;
	int imm;
	int rdVal;
	int rtVal;
	int rsVal;
	int pc;
	int stateHalt;


}idieReg;
//IE/IM register
typedef struct {

	int opcode;
	int rd;
	int rs;
	int rt;
	int imm;
	int rdVal;
	int rtVal;
	int rsVal;
	int addrResolution;
	int rdResolution;
	int pc;
	int stateHalt;
}ieimReg;
//IM/WB register
typedef struct {

	int opcode;
	int rd;
	int rs;
	int rt;
	int imm;
	int rdVal;
	int rtVal;
	int rsVal;
	int addrResolution;
	int rdResolution;
	int pc;
	int stateHalt;

}imwbReg;
//transaction structure
typedef struct {
	int bus_origid;
	int bus_cmd;
	int bus_addr;
	int bus_data;
	int bus_shared;

}transaction;
// core relavent arguments structure
typedef struct {

	int coreNum;
	int corehalt;
	int pc;
	int regfile[CORE_REG_NUM];
	int dsram[DSRAM_MAX_LINE];
	int tsram[TSRAM_MAX_LINE];
	int dataNotReady;
	ifidReg ifidRegold, ifidRegnew;
	idieReg idieRegold, idieRegnew;
	ieimReg ieimRegold, ieimRegnew;
	imwbReg imwbRegold, imwbRegnew;
	transaction transaction;

	int miss;
	long readHit;
	long writeHit;
	long readMiss;
	long writeMiss;
	long decodeStall;
	long memStall;
	long totalClock;
	long totalInst;

	FILE* coretraceP;
	FILE* imemP;

}coreArgs;

typedef struct {
	int bus_origid;
	int bus_cmd;
	int bus_addr;
	int bus_data;
	int bus_shared;
	int cycles; // cycles since MEM started transaction
	int master; // Bus original caller if differ then bus_origid
} bus_struct;




// Function declaring 
void core(long clock, coreArgs* coreArgsStruct, bus_struct bus);
void initializator(int coreNum,coreArgs* coreArgsStruct);
void coreFiles(coreArgs* coreArgs);
void Round_Robin_update(int req_core);
void Round_Robin_remove(int finished);
void Bus_History_Queue_Update(int core_num);

int whoIsInM(int index, int tag, int tsram1[], int tsram2[], int tsram3[]);
void snooping(int index, int tag, int tsram[]);
void mesi_update(int core_updated);

void bus_trace_write(FILE* bus_trace, int clock);
int Bus_transaction(FILE* bus_trace, int modified);
int read_miss(FILE* bus_trace, int req_core, int index, int tag);
int bus_call(FILE* bus_trace, int core_num);

int dmemin_read(FILE* dmemin, int dmem[][5]);
void dmemout_write(FILE* dmemout, int dmem[][5]);
void max_mem_update(int new_addr);

#endif