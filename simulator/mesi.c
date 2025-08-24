#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "header.h"

/*	###########################
* 
*
* This file contain MESI protocol related functions:
*		the snooping functoin, one for checking if other cores in Modified and one for updating the MESI state for all four cores, if needed
*
*	########################### 
*/


extern coreArgs cores_arr[NUM_CORES]; // Array of the cores
extern bus_struct bus; // The Bus 


int whoIsInM(int index, int tag, int tsram1[], int tsram2[], int tsram3[]) { // Check if another cache is in Modified for the same tag and retern its relatiive distance
	if ((tsram1[index] & 0x00000FFF) == tag && (tsram1[index] & 0x00003000) >> 12 == 3)  // Block has been modified by the next core
		return 1;
	else if ((tsram2[index] & 0x00000FFF) == tag && (tsram2[index] & 0x00003000) >> 12 == 3) // Modified by the core two places away
		return 2;
	else if ((tsram3[index] & 0x00000FFF) == tag && (tsram3[index] & 0x00003000) >> 12 == 3) // Modified by the core three places away
		return 3;
	return 0; // No other cache has modified this block
}


void snooping(int index, int tag, int tsram[]) { // Returns 0 if Block doesn't exist in the cache, 1 if it does and 3 if it has been modified
	if (bus.bus_shared == 1) // we already know the block snooper is 'shared'
		return;
	else if (((tsram[index] & 0x00000FFF) != tag) || ((tsram[index] & 0x00003000) >> 12) == 0) { // Block not in cache
		return;
	}
	else {
		bus.bus_shared = 1;
		return;
	}
}


void mesi_update(int core_updated) { 
	int i = 0;
	int tag = (cores_arr[core_updated].transaction.bus_addr & 0x000FFF00) >> 8; // Block tag
	int index = (cores_arr[core_updated].transaction.bus_addr & 0x000000FC) >> 2; // Block index 

	if (cores_arr[core_updated].transaction.bus_cmd == 2) { // BUsRdX
		cores_arr[core_updated].tsram[index] = tag + (0x00000003 << 12); // Set MESI to 'Modified'
		for (i = 0; i < NUM_CORES; i++) {
			if (i != core_updated) {
				if (((cores_arr[i].tsram[index] & 0x00000FFF) == tag) && (((cores_arr[i].tsram[index] & 0x00003000) >> 12) != 0)) {
					cores_arr[i].tsram[index] = (cores_arr[i].tsram[index] & 0x00000FFF); // Set MESI to 'Invalid'
				}
			}
		}
	}
	else { // BusRd or flash by the core itself
		if (bus.bus_shared == 1) { // Block is shared
			cores_arr[core_updated].tsram[index] = tag + (0x00000001 << 12); // Set MESI to 'Shared'
			for (i = 0; i < NUM_CORES; i++) { // Update other cores MESI state
				if (i != core_updated) {
					if (((cores_arr[i].tsram[index] & 0x00000FFF) == tag) && (((cores_arr[i].tsram[index] & 0x00003000) >> 12) != 0)) {
						cores_arr[i].tsram[index] = (cores_arr[i].tsram[index] & 0x00000FFF) + (0x00000001 << 12); // Set the core MESI to 'Shared'
					}
				}
			}
		}
		else {
			cores_arr[core_updated].tsram[index] = tag + (0x00000002 << 12); // Set MESI to 'Exclusive'
		}
	}
	return;
}
