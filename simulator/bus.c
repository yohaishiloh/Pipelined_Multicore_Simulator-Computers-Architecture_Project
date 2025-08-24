#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "header.h"

#define NUM_CORES 4


/*	###########################
*
*
* This file contain Bus handling and execution related functions:
*		Writing to the Bus trace file, Bus transaction - initiating(for read, write or flush), Bus transactions execution, Bus snooping, Bus call handler
*
*	###########################
*/




extern int bus_queue[4];
extern int bus_history[4];
extern coreArgs cores_arr[NUM_CORES];
extern bus_struct bus;
extern int main_memory[1 << 20][5];
extern int max_mem;
extern long clock;


void bus_trace_write(FILE* bus_trace, int clock) {
	char origid[5], shared[5], cmd[5], addr[10], data[12];

	// Record bus transaction parameters
	sprintf(origid, "%01X", bus.bus_origid);
	sprintf(cmd, "%01X", bus.bus_cmd);
	sprintf(addr, "%05X", bus.bus_addr);
	sprintf(data, "%08X", bus.bus_data);
	sprintf(shared, "%01X", bus.bus_shared);

	fprintf(bus_trace, "%ld %s %s %s %s %s\n", clock, origid, cmd, addr, data, shared); //write bustrace
	return;
}

int Bus_transaction(FILE* bustrace, int modified) {
	int i = 0;
	int index = (bus.bus_addr & 0x000000FC) >> 2; // Block index
	int offset = 0;
	int tag = (bus.bus_addr & 0x000FFF00) >> 8; // Block tag

	switch (bus.bus_cmd) {
	case 0:
		return -1; // No transaction - bus is free
	case 1: // BusRd
		bus_trace_write(bustrace, clock);
		for (int i = 0; i < NUM_CORES; i++) {
			if (cores_arr[i].coreNum != bus.bus_origid) {
				snooping(index, tag, cores_arr[i].tsram);
			}
		}
		mesi_update(bus.bus_origid);
		if (modified == -1) { // No modified block - pass control on the bus to the requesting cache
			bus.cycles++;
			offset = -1;
			bus.master = bus.bus_origid;
			bus.bus_cmd = 3;
			bus.cycles = 0;
			bus.bus_origid = 4;
		}
		else { // Modified block - pass control on the bus to the modifieing cache and start flush transaction to MEM
			bus.master = bus.bus_origid;
			bus.bus_origid = modified;
			bus.bus_cmd = 3;
			bus.cycles = 0;
			bus.bus_data = cores_arr[modified].dsram[index << 2];
			offset = 0;
		}	
		break;

	case 2: // BusRdX
		bus_trace_write(bustrace, clock);
		for (int i = 0; i < NUM_CORES; i++) { // Execute snooping
			if (cores_arr[i].coreNum != bus.bus_origid) {
				snooping(index, tag, cores_arr[i].tsram);
			}
		}
		mesi_update(bus.bus_origid);
		if (modified == -1) { // No modified block - pass control on the bus to the requesting cache
			bus.cycles++;
			offset = -1;
			bus.master = bus.bus_origid;
			bus.bus_cmd = 3;
			bus.cycles = 0;
			bus.bus_origid = 4;
		}
		else { // Modified block - pass control on the bus to the modifieing cache and start flush transaction to MEM
			bus.master = bus.bus_origid;
			bus.bus_origid = modified;
			bus.bus_cmd = 3;
			bus.cycles = 0;
			bus.bus_data = 0;
			offset = -1;
		}
		break;

	case 3: // Flush
		if (bus.bus_origid == 4) { // Flush from MEM
			if (bus.cycles < 15) { // Waiting for the MEM
				offset == -1;
				bus.cycles++;
				if (bus.cycles == 15)
					bus.cycles++;
			} 
			else { // Read word from MEM
				offset = bus.cycles % 4;
				bus.bus_addr = (bus.bus_addr & 0xFFFFFFFC) + offset;
				bus.cycles++;
				bus.bus_data = main_memory[bus.bus_addr / 4][offset];
				bus_trace_write(bustrace, clock);
				if (bus.cycles == 20) { // Flush was done
					bus.cycles = 0;
					bus.bus_cmd = 0;
				}
			}
			break;
		}
		else { // Flush from core - Modified block
			if(bus.master == -1) // Flush from the core itself
				mesi_update(bus.bus_origid);
			if (bus.cycles < 4) { // Flush from core in progress
				offset = bus.cycles;
				bus.bus_addr = (bus.bus_addr & 0xFFFFFFFC) + offset;
				bus.bus_data = cores_arr[bus.bus_origid].dsram[bus.bus_addr & 0x000000FF];
				main_memory[bus.bus_addr / 4][offset] = bus.bus_data;
				bus.cycles++;
				bus_trace_write(bustrace, clock);
				if (bus.cycles == 4) {
					main_memory[bus.bus_addr / 4][4] = 1;
					max_mem_update(bus.bus_addr);
					bus.bus_cmd = 0;
					bus.bus_addr = 0;
				}
			}
			else if (bus.cycles >= 4) { // Done cache - flush transaction
				main_memory[bus.bus_addr / 4][4] = 1;
				max_mem_update(bus.bus_addr);
				offset = -1;
				bus.cycles = 0;
				bus.bus_cmd = 0;
				bus.bus_addr = 0;
			}
			break;
		}
	default:
		printf("bus error - case default\n");
		break;
	}
	return offset;
}

int read_miss(FILE* bustrace, int req_core, int index, int tag) {
	int modified = -1;
	int offset = 0;
	int inM = 0; 

	// Check - is anyone in M?
	if (((cores_arr[req_core].tsram[index] & 0x00003000) >> 12) == 3)
		modified = req_core;
	else {
		inM = whoIsInM(index, tag, cores_arr[(req_core + 1) % 4].tsram, cores_arr[(req_core + 2) % 4].tsram, cores_arr[(req_core + 3) % 4].tsram);
		if (inM != 0) {// yes - call bus to flush to MEM and requesting cache
			modified = (req_core + inM) % NUM_CORES;
		}
	}

	offset = Bus_transaction(bustrace, modified);
	if (offset != -1) // Data received on the bus
		cores_arr[req_core].dsram[(index << 2) + offset] = bus.bus_data;
	if (bus.bus_cmd == 0) { // Transaction is over - probably redundant, remains just to be safe
		cores_arr[req_core].transaction.bus_cmd = 0;
		cores_arr[req_core].dataNotReady = 0;
		cores_arr[req_core].transaction.bus_shared = 0; 
		Round_Robin_remove(req_core);
		return req_core;
	}
	return -1; // 
}


int write_miss(FILE* bustrace, int req_core, int index, int tag) { // For BusRdX, build similar to read_miss
	int modified = -1;
	int offset = 0;
	int inM;

	// is anyone in M?
	if ((cores_arr[req_core].tsram[index] & 0x00003000) >> 12 == 3)
		modified = req_core;
	else {
		inM = whoIsInM(index, tag, cores_arr[(req_core + 1) % 4].tsram, cores_arr[(req_core + 2) % 4].tsram, cores_arr[(req_core + 3) % 4].tsram);
		if (inM != 0) {// yes - call bus to flush to MEM and requesting cache
			modified = (req_core + inM) % NUM_CORES;
		}
	}

	offset = Bus_transaction(bustrace, modified);
	if (offset != -1) // Data 
		cores_arr[req_core].dsram[(index << 2) + offset] = bus.bus_data;
	if (bus.bus_cmd == 0) {
		cores_arr[req_core].transaction.bus_cmd = 0;
		cores_arr[req_core].dataNotReady = 0;
		cores_arr[req_core].transaction.bus_shared = 0;
		Round_Robin_remove(req_core);
		bus.bus_data = 0;
		return req_core;
	}
	return -1;
}


int bus_call(FILE* bustrace, int core_num) { // Handling bus transactions requests from the main function
	// Is the bus needed?
	if (cores_arr[core_num].transaction.bus_cmd == 0)
		return;
	
	if (bus.bus_cmd != 0 && (bus.bus_origid != core_num && bus.master != core_num)) // bus occupied and by someone else
		return;

	int tag = (cores_arr[core_num].transaction.bus_addr & 0x000FFF00) >> 8; // Block tag
	int index = (cores_arr[core_num].transaction.bus_addr & 0x000000FC) >> 2; // Block index
	int offset = 0;
	int finished = -1;

	if (bus.bus_cmd == 0) { // bus is free
		if (bus_queue[0] != core_num) // not my turn
			return finished;
		else { // start a new transaction on the bus
			bus.bus_origid = core_num; // occupy bus, probably redundant
			bus.bus_addr = cores_arr[core_num].transaction.bus_addr; // Probably redundant
			bus.bus_cmd = cores_arr[core_num].transaction.bus_cmd;
			if (cores_arr[core_num].transaction.bus_cmd == 1)
				finished = read_miss(bustrace, core_num, index, tag);
			else
				finished = write_miss(bustrace, core_num, index, tag);
		}
	}
	else if (bus.bus_cmd == 3 && (bus.master == core_num || (bus.master == -1 && bus.bus_origid == core_num))) { // Flush for this core
		offset = Bus_transaction(bustrace, -1);
		if (offset != -1) { // If data was received on the bus
			cores_arr[core_num].dsram[(index << 2) + offset] = bus.bus_data; // Write data to the cache
			bus.bus_data = 0;
		}
		if (bus.bus_cmd == 0) { // Flush was over
			if (bus.master != -1) { // The flush was from another cache for a modified block
				finished = bus.master;
				Round_Robin_remove(bus.master);
				Bus_History_Queue_Update(bus.master);
			}
			else { // The flush was from the core itself or from the main memory
				finished = bus.bus_origid;
				Round_Robin_remove(bus.bus_origid);
				Bus_History_Queue_Update(bus.bus_origid);
//				mesi_update(bus.bus_origid);
			}
			// Reset bus parameters
			bus.bus_addr = 0;
			bus.bus_origid = -1;
			bus.bus_shared = 0;
			bus.master = -1;
			bus.cycles = 0;
			cores_arr[core_num].transaction.bus_cmd = 0;
			cores_arr[core_num].dataNotReady = 0;
			cores_arr[core_num].transaction.bus_shared = 0;
		}
		return finished;
	}
	else if (bus.bus_origid == core_num && (bus.bus_cmd == 1 || bus.bus_cmd == 2)) { // Bus is during BusRd/BusRdx
		read_miss(bustrace, core_num, index, tag);
	}
	else
		return finished;

	return -1;
}
