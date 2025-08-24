#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "header.h"


/*	###########################
*
*
* This file contain memory related functions:
*		Reading and writing to the main memory array, handling the memin and memout files and updating the maximal written address
*
*	###########################
*/



extern int max_mem;

void sign_extension(long reg, char dest[], int dest_max_len) {
	char zeros[25] = "000000000000";
	sprintf(dest, "%x", reg);
	strcat(zeros, dest); // Add zeros
	int zeros_added_len = strlen(zeros);
	strcpy(dest, zeros + (zeros_added_len - dest_max_len)); // Take only the wanted size of the string
	return;
}


int dmemin_read(FILE* dmemin, int dmem[][5]) { // Reading the memin file into the main memory array
	char line[9];
	int i = 0, j = 0;
	while (!feof(dmemin) && i < MEM_SIZE) {
		fgets(line, 9, dmemin);
		if (line[0] == '\0')
			return i;
		if (line[0] == '\n')
			continue;
		dmem[i / 4][i % 4] = strtol(line, NULL, 16);
		dmem[i / 4][4] = 1;
		i++;
	};
	return i;
};

void max_mem_update(int new_addr) { // Updating the maximal written address
	if (new_addr > max_mem)
		max_mem = new_addr + 1;
	return;
}


void dmemout_write(FILE* dmemout, int dmem[][5]) { // Functiion to write the memout file from the main memory array
	char mem_line[9];
	for (int i = 0; (i < max_mem) && (i < MEM_SIZE); i++) {
		if (dmem[i / 4][4] == 0)
			fprintf(dmemout, "%s\n", "00000000");
		else {
			sign_extension(dmem[i / 4][i % 4], mem_line, 8);
			fprintf(dmemout, "%s\n", mem_line);
		}
	}
	return;
}