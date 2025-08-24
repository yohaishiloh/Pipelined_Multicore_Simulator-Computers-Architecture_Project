#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "header.h"

#define NUM_CORES 4

/*	###########################
*
*
* This file contain the arbitrator functions:
*		Adding core to thr queue, removing core from the queue and updating the bus history queue
*
*	###########################
*/


extern int bus_queue[4];
extern int bus_history[4];


void Round_Robin_update(int req_core) { // Update the arbitrator queue by the Round Robin policy
	for (int i = 0; i < NUM_CORES; i++) {
		if (bus_queue[i] == req_core) { // core already in queue
			break;
		}
		if (bus_queue[i] == -1) { // next place in queue is empty
			bus_queue[i] = req_core;
			break;
		}
	}
	return; 
}

void Round_Robin_remove(int finished) { // Remove core from the arbitrator queue
	int temp = 0;
	if (bus_queue[0] != finished) {
		printf("Error RR queue update\n");
		return;
	}
	temp = bus_queue[0];
	for (int i = 0; i < 3; i++) {
		bus_queue[i] = bus_queue[i + 1];
	}
	bus_queue[3] = -1;
	return;
}

void Bus_History_Queue_Update(int core_num) { 
	int i = 0;
	for (i = 0; i < NUM_CORES - 1; i++) {
		if (bus_history[i] == core_num) {
			bus_history[i] = bus_history[i + 1];
			bus_history[i + 1] = core_num;
		}
	}
	return;
}