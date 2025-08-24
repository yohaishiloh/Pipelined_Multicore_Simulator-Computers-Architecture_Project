#define _CRT_SECURE_NO_WARNINGS

#include "header.h"

// A function to initialze a single core 
void initializator(int coreNum,coreArgs* coreArgsStruct) {
	char buffer[20];// buffer for file names
	
	


	memset(coreArgsStruct, 0, sizeof(coreArgs)); // initialize core args to 0
	strcpy(coreArgsStruct->ifidRegnew.instruction, "12000000");// initialize core instruction to nop
	strcpy(coreArgsStruct->ifidRegold.instruction, "12000000");// initialize core instruction to nop
	coreArgsStruct->idieRegold.opcode = 18;
	coreArgsStruct->ieimRegold.opcode = 18;
	coreArgsStruct->imwbRegold.opcode = 18;
	coreArgsStruct->idieRegnew.opcode = 18;
	coreArgsStruct->ieimRegnew.opcode = 18;
	coreArgsStruct->imwbRegnew.opcode = 18;
	coreArgsStruct->coreNum = coreNum; // set number to core

	FILE* fp; //fp to open file

	//initialize regout file
	sprintf(buffer, "regout%d.txt", coreArgsStruct->coreNum);
	fp = fopen(buffer, "w");
	if (fp == NULL) {
		printf("Error opening file!\n");
		return 1;
	}
	fclose(fp);
	//initialize coretrace file
	sprintf(buffer, "core%dtrace.txt", coreArgsStruct->coreNum);
	fp = fopen(buffer, "w");
	if (fp == NULL) {
		printf("Error opening file!\n");
		return 1;
	}
	fclose(fp);

	coreArgsStruct->coretraceP = fopen(buffer, "a");
	if (coreArgsStruct->coretraceP == NULL) {
		printf("Error opening file!\n");
		return 1;
	}

	//initialize dsram file
	sprintf(buffer, "dsram%d.txt", coreArgsStruct->coreNum);
	fp = fopen(buffer, "w");
	if (fp == NULL) {
		printf("Error opening file!\n");
		return 1;
	}
	fclose(fp);

	//initialize tsram file
	sprintf(buffer, "tsram%d.txt", coreArgsStruct->coreNum);
	fp = fopen(buffer, "w");
	if (fp == NULL) {
		printf("Error opening file!\n");
		return 1;
	}
	fclose(fp);

	//initialize stats file
	sprintf(buffer, "stats%d.txt", coreArgsStruct->coreNum);
	fp = fopen(buffer, "w");
	if (fp == NULL) {
		printf("Error opening file!\n");
		return 1;
	}
	fclose(fp);



}

