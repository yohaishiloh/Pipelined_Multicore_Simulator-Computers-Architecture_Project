#include "header.h"

void coreFiles(coreArgs* coreArgs) //this fucntion handle writing to files regout dsram tsram and stats
{
	char buffer[50];
	char hexString[10];
	FILE* fp;
	//handle regout file
	sprintf(buffer, "regout%d.txt", coreArgs->coreNum);
	fp = fopen(buffer, "a");
	if (fp == NULL) {
		printf("Error opening file!\n");
		return;
	}
	for (int i = 2; i < 16;i++)
	{
		sprintf(hexString, "%08X", coreArgs->regfile[i]);
		fprintf(fp, "%s\n", hexString);
	}
	fclose(fp);

	//handle dsram file
	sprintf(buffer, "dsram%d.txt", coreArgs->coreNum);
	fp = fopen(buffer, "a");
	if (fp == NULL) {
		printf("Error opening file!\n");
		return;
	}
	for (int i = 0; i < DSRAM_MAX_LINE; i++)
	{
		sprintf(hexString, "%08X", coreArgs->dsram[i]);
		fprintf(fp, "%s\n", hexString);
	}
	fclose(fp);

	//handle tsram file
	sprintf(buffer, "tsram%d.txt", coreArgs->coreNum);
	fp = fopen(buffer, "a");
	if (fp == NULL) {
		printf("Error opening file!\n");
		return;
	}
	for (int i = 0; i < TSRAM_MAX_LINE; i++)
	{
		sprintf(hexString, "%08X", coreArgs->tsram[i]);
		fprintf(fp, "%s\n", hexString);
	}
	fclose(fp);

	//handle stats file
	sprintf(buffer, "stats%d.txt", coreArgs->coreNum);
	fp = fopen(buffer, "a");
	fprintf(fp, "cycles %ld\n", coreArgs->totalClock);
	fprintf(fp, "instructions %ld\n", coreArgs->totalInst);
	fprintf(fp, "read_hit %ld\n", coreArgs->readHit);
	fprintf(fp, "write_hit %ld\n", coreArgs->writeHit);
	fprintf(fp, "read_miss %ld\n", coreArgs->readMiss);
	fprintf(fp, "write_miss %ld\n", coreArgs->writeMiss);
	fprintf(fp, "decode_stall %ld\n", coreArgs->decodeStall);
	fprintf(fp, "mem_stall %ld\n", coreArgs->memStall);



	fclose(fp);
	fclose(coreArgs->coretraceP);
	fclose(coreArgs->imemP);

	return;
}