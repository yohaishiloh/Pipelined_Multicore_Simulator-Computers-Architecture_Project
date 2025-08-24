#define _CRT_SECURE_NO_WARNINGS

#include "header.h"


// global variables
long clock = 0; // entire program clock
int bus_queue[4] = { -1, -1, -1, -1 };
int bus_history[4] = { 0, 1, 2, 3 };
coreArgs cores_arr[NUM_CORES];
bus_struct bus;
int main_memory[1 << 20][5];
int max_mem;
int last_M = -1;
char default_files_names[28][18] = {"imem0.txt", "imem1.txt", "imem2.txt", "imem3.txt", "memin.txt", "memout.txt", "regout0.txt", "regout1.txt", "regout2.txt", "regout3.txt", 
										"core0trace.txt", "core1trace.txt", "core2trace.txt", "core3trace.txt", "bustrace.txt", "dsram0.txt", "dsram1.txt", "dsram2.txt", "dsram3.txt", 
										"tsram0.txt", "tsram1.txt", "tsram2.txt", "tsram3.txt", "stats0.txt", "stats1.txt", "stats2.txt", "stats3.txt" };


int main(int argc, char* argv[]) {
	int i = 0, j = 0, index, tag, temp = 0;
	int arbitrator_call = 0;
	int finished = -1;
	FILE* dmemin = NULL;
	FILE* dmemout = NULL;

	if (argc == 1) {
		for (i = 0; i < NUM_CORES; i++) {
			initializator(i, &cores_arr[i]); // initialize a core
			//open imem files
			cores_arr[i].imemP = fopen(default_files_names[i], "r");
			if (cores_arr[i].imemP == NULL)
			{
				printf("Error opening file!\n");
				return 1;
			}
		}

		dmemin = fopen(default_files_names[4], "r");
		if (NULL == dmemin) {
			printf("Error: failed opening file\n");
			exit(-1);
		}
		// write mem array into the file:
		dmemout = fopen(default_files_names[5], "w");
		if (NULL == dmemout) {
			printf("Error: failed opening file\n");
			exit(-1);
		}
	}
	else if (argc < 28) {
		printf("Not enough command line arguments\n");
		exit(-1);
	}
	else {
		for (i = 0; i < NUM_CORES; i++) {
			initializator(i, &cores_arr[i]); // initialize a core
			//open imem files
			cores_arr[i].imemP = fopen(argv[i + 1], "r");
			if (cores_arr[i].imemP == NULL)
			{
				printf("Error opening file!\n");
				return 1;
			}
		}

		dmemin = fopen(argv[5], "r");
		if (NULL == dmemin) {
			printf("Error: failed opening file\n");
			exit(-1);
		}
		// write mem array into the file:
		dmemout = fopen(argv[6], "w");
		if (NULL == dmemout) {
			printf("Error: failed opening file\n");
			exit(-1);
		}

	}


	max_mem = 0;
	max_mem = dmemin_read(dmemin, main_memory); // Read the memin file into the main memory array
	fclose(dmemin);

	FILE* bustrace = fopen("bustrace.txt", "w");
	if (NULL == bustrace) {
		printf("Error: failed opening file\n");
		exit(-1);
	}

	// bus initialization
	bus.bus_origid = -1;
	bus.bus_cmd = 0;
	bus.bus_addr = 0;
	bus.bus_data = 0;
	bus.bus_shared = 0;
	bus.cycles = 0;
	bus.master = -1;


	while (cores_arr[0].corehalt == 0 || cores_arr[1].corehalt == 0 || cores_arr[2].corehalt == 0 || cores_arr[3].corehalt == 0) { //if at least 1 core not halted

		if (bus.bus_origid != -1) { // Bus allocated in previous clock cycles - start/continue execution
			finished = bus_call(bustrace, bus_queue[0]);
		}
		for (i = 0; i < NUM_CORES; i++) { // Check for each core if it is requesting bus and update the arbitration queue
			if (cores_arr[bus_history[i]].transaction.bus_cmd != 0) {
				Round_Robin_update(cores_arr[bus_history[i]].coreNum);
			}
		}
		if ((bus_queue[0] != -1) && (bus.bus_origid == -1) ) { // Bus assigning by the arbitrator - if free
			bus.bus_origid = bus_queue[0];
			bus.bus_addr = cores_arr[bus_queue[0]].transaction.bus_addr;
		}

		for (i = 0; i < NUM_CORES; i++) { // Cores running
			if (cores_arr[i].corehalt == 0) // if core 0 not halted
				core(clock, &(cores_arr[i]),bus);
		}

		clock = clock + 1;
	}


	dmemout_write(dmemout, main_memory);
	
	for (i = 0; i < NUM_CORES; i++) {
		coreFiles(&(cores_arr[i])); // handle core 0 files
	}

	fclose(dmemout);
	fclose(bustrace);

	return 0;
}

