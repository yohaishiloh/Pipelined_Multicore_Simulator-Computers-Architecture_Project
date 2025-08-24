#define _CRT_SECURE_NO_WARNINGS

#include "header.h"



// These function contain the entire operation of a core 
void core(long clock, coreArgs* coreArgsStruct, bus_struct bus)
{

	
	char imemline[IMEM_LINE_SIZE] = { '\0' }; // Contain line read from imem
	int decodeBubble = 0; // Indicator of a stall in decode stage
	int memBubble = 0; // Indicator of a stall in mem stage
	int btaken = 0; // indicator if a branch is taken or not
	int index; 
	int offset;
	int tag;

	char buffer[20];// buffer for txt file
	char regbuff[CORE_REG_NUM][20]; // buffer for txt file
	char pcBuffF[20];// buffer for txt file
	char pcBuffD[20];// buffer for txt file
	char pcBuffE[20];// buffer for txt file
	char pcBuffM[20];// buffer for txt file
	char pcBuffW[20];// buffer for txt file



	for (int i = 2; i < CORE_REG_NUM;i++) // Formating register values to hexa strings for coretrace txt
	{
		sprintf(regbuff[i], "%08X", coreArgsStruct->regfile[i]);
	}


	//START pipeline
	//// FETCH:


	

	for (int i = 0; i <= coreArgsStruct->pc; i++) { // jump to the line according to the pc

		fgets(imemline, IMEM_LINE_SIZE, coreArgsStruct->imemP); //get the line

	}
	rewind(coreArgsStruct->imemP);
	if (imemline[0] == '\n') strcpy(imemline, "00000000");
	imemline[strcspn(imemline, "\n")] = '\0'; //removing \n from the line

	strcpy(coreArgsStruct->ifidRegnew.instruction, imemline); // write the instruction to IF/ID reg 
	coreArgsStruct->ifidRegnew.pc = coreArgsStruct->pc; // save pc of inst

	//End Fetch
	///////////////////////////////////////////////////////////////////////////////////////////////////


	////DECODE:
	coreArgsStruct->idieRegnew.pc = coreArgsStruct->ifidRegold.pc; // save pc of inst
	char opcode[3];
	char rd[2];
	char rs[2];
	char rt[2];
	char imm[4];

	//Breaking down the instruction to the relavent parts
	//opcode
	strncpy(opcode, coreArgsStruct->ifidRegold.instruction, 2); // copy the hexa bits from inst.
	opcode[2] = '\0';
	

	coreArgsStruct->idieRegnew.opcode = (int)strtol(opcode, NULL, 16); // Translating the hexa bits to decimal integer
 
	//rd
	strncpy(rd, coreArgsStruct->ifidRegold.instruction + 2, 1);
	rd[1] = '\0';
	coreArgsStruct->idieRegnew.rd = (int)strtol(rd, NULL, 16);
	
	//rs
	strncpy(rs, coreArgsStruct->ifidRegold.instruction + 3, 1);
	rs[1] = '\0';
	

	coreArgsStruct->idieRegnew.rs = (int)strtol(rs, NULL, 16);
	
	//rt
	strncpy(rt, coreArgsStruct->ifidRegold.instruction + 4, 1);
	rt[1] = '\0';
	coreArgsStruct->idieRegnew.rt = (int)strtol(rt, NULL, 16);
	
	//imm
	strncpy(imm, coreArgsStruct->ifidRegold.instruction + 5, 3);
	imm[3] = '\0';
	coreArgsStruct->idieRegnew.imm = (int)strtol(imm, NULL, 16);
	

	//imm sign extend
	if (imm[0] >= '8')
	{
		coreArgsStruct->idieRegnew.imm = coreArgsStruct->idieRegnew.imm | 0xFFFFF000;
	}
	
	coreArgsStruct->regfile[1] = coreArgsStruct->idieRegnew.imm; //set r1 = imm



	//RAW (written to rs rt)Hazard check

	if ((coreArgsStruct->idieRegnew.opcode != 20) &&
		(
			((coreArgsStruct->idieRegold.opcode >= 0 && coreArgsStruct->idieRegold.opcode <= 8) || coreArgsStruct->idieRegold.opcode == 16)
			|| ((coreArgsStruct->ieimRegold.opcode >= 0 && coreArgsStruct->ieimRegold.opcode <= 8) || coreArgsStruct->ieimRegold.opcode == 16)
			|| ((coreArgsStruct->imwbRegold.opcode >= 0 && coreArgsStruct->imwbRegold.opcode <= 8) || coreArgsStruct->imwbRegold.opcode == 16))

		) // if  not halt and there is an inst that writing to rd in ie/im/wb

	{
		// ALU ins and lw : comparing rs/ rt to rd in case they are not r0 or r1
		if ((coreArgsStruct->idieRegnew.opcode >= 0 && coreArgsStruct->idieRegnew.opcode <= 8) || coreArgsStruct->idieRegnew.opcode == 16 )
		{
			if (((coreArgsStruct->idieRegnew.rs != 0 && coreArgsStruct->idieRegnew.rs != 1 &&  ((coreArgsStruct->idieRegnew.rs == coreArgsStruct->ieimRegold.rd && coreArgsStruct->ieimRegold.opcode != 18 )|| (coreArgsStruct->idieRegnew.rs == coreArgsStruct->imwbRegold.rd && coreArgsStruct->imwbRegold.opcode != 18) || (coreArgsStruct->idieRegnew.rs == coreArgsStruct->idieRegold.rd && coreArgsStruct->idieRegold.opcode != 18))
				)|| (coreArgsStruct->idieRegnew.rt != 0 && coreArgsStruct->idieRegnew.rt != 1 && ((coreArgsStruct->idieRegnew.rt == coreArgsStruct->ieimRegold.rd && coreArgsStruct->ieimRegold.opcode != 18 )|| (coreArgsStruct->idieRegnew.rt == coreArgsStruct->imwbRegold.rd && coreArgsStruct->imwbRegold.opcode != 18) || (coreArgsStruct->idieRegnew.rt == coreArgsStruct->idieRegold.rd && coreArgsStruct->idieRegold.opcode != 18)))))  
			{
				//handle stall
				decodeBubble = 1; //stall indicator
				coreArgsStruct->idieRegnew.opcode = 18; // putting bubble in pipe
			
			}
		}
		// for branch inst and sw : comparing rs/ rt to rd in case they are not r0 or r1 (in older version these ifs needed to be seperated
		else if ((coreArgsStruct->idieRegnew.opcode >= 9 && coreArgsStruct->idieRegnew.opcode <= 14) || coreArgsStruct->idieRegnew.opcode == 17)
		{
			if (coreArgsStruct->idieRegnew.rs != 0 && coreArgsStruct->idieRegnew.rs != 1 &&  ((coreArgsStruct->idieRegnew.rs == coreArgsStruct->ieimRegold.rd && coreArgsStruct->ieimRegold.opcode != 18) || (coreArgsStruct->idieRegnew.rs == coreArgsStruct->imwbRegold.rd && coreArgsStruct->imwbRegold.opcode != 18) || (coreArgsStruct->idieRegnew.rs == coreArgsStruct->idieRegold.rd && coreArgsStruct->idieRegold.opcode != 18))
				|| (coreArgsStruct->idieRegnew.rt != 0 && coreArgsStruct->idieRegnew.rt != 1 && ((coreArgsStruct->idieRegnew.rt == coreArgsStruct->ieimRegold.rd && coreArgsStruct->ieimRegold.opcode != 18) || (coreArgsStruct->idieRegnew.rt == coreArgsStruct->imwbRegold.rd && coreArgsStruct->imwbRegold.opcode != 18) || (coreArgsStruct->idieRegnew.rt == coreArgsStruct->idieRegold.rd && coreArgsStruct->idieRegold.opcode != 18)))) // comparing rs/ rt to rd in case they are not r0 or r1 
			{
				//handle stall
				decodeBubble = 1; //stall indicator
				coreArgsStruct->idieRegnew.opcode = 18;
//				

			}
		}



	}


	if (decodeBubble != 1)
	{
		coreArgsStruct->idieRegnew.rdVal = coreArgsStruct->regfile[coreArgsStruct->idieRegnew.rd]; // cpy rd value from regfile
		coreArgsStruct->idieRegnew.rsVal = coreArgsStruct->regfile[coreArgsStruct->idieRegnew.rs]; // cpy rs value from regfile
		coreArgsStruct->idieRegnew.rtVal = coreArgsStruct->regfile[coreArgsStruct->idieRegnew.rt]; // cpy rt value from regfile

		//branch resulution

		switch (coreArgsStruct->idieRegnew.opcode)
		{
		case 9://beq
			if (coreArgsStruct->idieRegnew.rsVal == coreArgsStruct->idieRegnew.rtVal) btaken = 1;
			break;
		case 10://bne
			if (coreArgsStruct->idieRegnew.rsVal != coreArgsStruct->idieRegnew.rtVal) btaken = 1;
			break;
		case 11://blt
			if (coreArgsStruct->idieRegnew.rsVal < coreArgsStruct->idieRegnew.rtVal) btaken = 1;
			break;
		case 12://bgt
			if (coreArgsStruct->idieRegnew.rsVal > coreArgsStruct->idieRegnew.rtVal) btaken = 1;
			break;
		case 13://ble
			if (coreArgsStruct->idieRegnew.rsVal <= coreArgsStruct->idieRegnew.rtVal) btaken = 1;
			break;
		case 14://bge
			if (coreArgsStruct->idieRegnew.rsVal >= coreArgsStruct->idieRegnew.rtVal) btaken = 1;
			break;
		case 15://jal
			btaken = 1;
			break;
		case 20://halt
			coreArgsStruct->ifidRegnew.stateHalt = 1; //halting fetch and decode stages
			break;
		}

		if (btaken == 1 || coreArgsStruct->idieRegnew.opcode == 17)
		{
			//RAW Hazard check (written to rd)
			if (
				((coreArgsStruct->idieRegold.opcode >= 0 && coreArgsStruct->idieRegold.opcode <= 8) || coreArgsStruct->idieRegold.opcode == 16)
				|| ((coreArgsStruct->ieimRegold.opcode >= 0 && coreArgsStruct->ieimRegold.opcode <= 8) || coreArgsStruct->ieimRegold.opcode == 16)
				|| ((coreArgsStruct->imwbRegold.opcode >= 0 && coreArgsStruct->imwbRegold.opcode <= 8) || coreArgsStruct->imwbRegold.opcode == 16))


			{
				if (((coreArgsStruct->idieRegnew.rd == coreArgsStruct->ieimRegold.rd && coreArgsStruct->ieimRegold.opcode != 18 && coreArgsStruct->ieimRegold.opcode != 17) || (coreArgsStruct->idieRegnew.rd == coreArgsStruct->imwbRegold.rd && coreArgsStruct->imwbRegold.opcode != 18 && coreArgsStruct->imwbRegold.opcode != 17) || (coreArgsStruct->idieRegnew.rd == coreArgsStruct->idieRegold.rd && coreArgsStruct->idieRegold.opcode != 18 && coreArgsStruct->idieRegold.opcode != 17)))
				{
					decodeBubble = 1;
					coreArgsStruct->idieRegnew.opcode = 18;
				}
				else if(coreArgsStruct->idieRegnew.opcode !=17) 
					coreArgsStruct->pc = (coreArgsStruct->idieRegnew.rdVal & 0x000003FF) - 1;
			}
			else if(coreArgsStruct->idieRegnew.opcode != 17)
				coreArgsStruct->pc = (coreArgsStruct->idieRegnew.rdVal & 0x000003FF) - 1;
		}

	}



	//END DECODE
	////////////////////////////////////////////////////////////////////////////////////////////////////////


	//EXECUTE
	//passing from register to register relavant data
	coreArgsStruct->ieimRegnew.pc = coreArgsStruct->idieRegold.pc; // save pc of inst
	coreArgsStruct->ieimRegnew.opcode = coreArgsStruct->idieRegold.opcode; // save opcode
	coreArgsStruct->ieimRegnew.rd = coreArgsStruct->idieRegold.rd;// save rd
	coreArgsStruct->ieimRegnew.rs = coreArgsStruct->idieRegold.rs; // save rs
	coreArgsStruct->ieimRegnew.rt = coreArgsStruct->idieRegold.rt;// save rt
	coreArgsStruct->ieimRegnew.imm = coreArgsStruct->idieRegold.imm;// save imm
	coreArgsStruct->ieimRegnew.rdVal = coreArgsStruct->idieRegold.rdVal;// save rdval
	coreArgsStruct->ieimRegnew.rsVal = coreArgsStruct->idieRegold.rsVal;// save rdval
	coreArgsStruct->ieimRegnew.rtVal = coreArgsStruct->idieRegold.rtVal;// save rdval


	switch (coreArgsStruct->idieRegold.opcode)
	{
	case 0: //add
		coreArgsStruct->ieimRegnew.rdVal = coreArgsStruct->idieRegold.rsVal + coreArgsStruct->idieRegold.rtVal;
		break;
	case 1: //sub
		coreArgsStruct->ieimRegnew.rdVal = coreArgsStruct->idieRegold.rsVal - coreArgsStruct->idieRegold.rtVal;
		break;
	case 2: //and
		coreArgsStruct->ieimRegnew.rdVal = coreArgsStruct->idieRegold.rsVal & coreArgsStruct->idieRegold.rtVal;
		break;
	case 3:// or
		coreArgsStruct->ieimRegnew.rdVal = coreArgsStruct->idieRegold.rsVal | coreArgsStruct->idieRegold.rtVal;
		break;
	case 4: // xor
		coreArgsStruct->ieimRegnew.rdVal = coreArgsStruct->idieRegold.rsVal ^ coreArgsStruct->idieRegold.rtVal;
		break;
	case 5://mul
		coreArgsStruct->ieimRegnew.rdVal = coreArgsStruct->idieRegold.rsVal * coreArgsStruct->idieRegold.rtVal;
		break;
	case 6://sll
		coreArgsStruct->ieimRegnew.rdVal = coreArgsStruct->idieRegold.rsVal << coreArgsStruct->idieRegold.rtVal;
		break;
	case 7://sra
		coreArgsStruct->ieimRegnew.rdVal = coreArgsStruct->idieRegold.rsVal >> coreArgsStruct->idieRegold.rtVal;
		break;
	case 8://srl
		coreArgsStruct->ieimRegnew.rdVal = ((unsigned int)coreArgsStruct->idieRegold.rsVal) >> coreArgsStruct->idieRegold.rtVal;
		break;
	case 15://jal
		coreArgsStruct->ieimRegnew.rdVal = coreArgsStruct->idieRegold.pc + 1;
		break;
	case 16://lw
		coreArgsStruct->ieimRegnew.addrResolution = coreArgsStruct->idieRegold.rsVal + coreArgsStruct->idieRegold.rtVal;
		break;
	case 17://sw
		coreArgsStruct->ieimRegnew.addrResolution = coreArgsStruct->idieRegold.rsVal + coreArgsStruct->idieRegold.rtVal;
		break;
	case 20://halt
		coreArgsStruct->idieRegnew.stateHalt = 1; //halting execute stage 
		break;
	}

	//END EXECUTE
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//MEM
	//passing from register to register relavant data
	coreArgsStruct->imwbRegnew.pc = coreArgsStruct->ieimRegold.pc; // save pc of inst
	coreArgsStruct->imwbRegnew.opcode = coreArgsStruct->ieimRegold.opcode; // save opcode
	coreArgsStruct->imwbRegnew.rd = coreArgsStruct->ieimRegold.rd; // save rd
	coreArgsStruct->imwbRegnew.rs = coreArgsStruct->ieimRegold.rs;// save rs
	coreArgsStruct->imwbRegnew.rt = coreArgsStruct->ieimRegold.rt;// save rt
	coreArgsStruct->imwbRegnew.imm = coreArgsStruct->ieimRegold.imm;// save imm
	coreArgsStruct->imwbRegnew.rdVal = coreArgsStruct->ieimRegold.rdVal;// save rdval
	coreArgsStruct->imwbRegnew.rsVal = coreArgsStruct->ieimRegold.rsVal;// save rdval
	coreArgsStruct->imwbRegnew.rtVal = coreArgsStruct->ieimRegold.rtVal;// save rdval

	offset = (coreArgsStruct->ieimRegold.addrResolution & 0x00000003); //set offset to address[1:0]
	index = (coreArgsStruct->ieimRegold.addrResolution & 0x000000FC) >> 2; //set index to address[7:2]
	tag = (coreArgsStruct->ieimRegold.addrResolution & 0x000FFF00) >> 8; //set tag to address[19:8]


	if (coreArgsStruct->transaction.bus_cmd == 0 && bus.bus_origid != coreArgsStruct->coreNum) //checking if not waiting for transaction to end and if not flushing data
	
	{
	
		switch (coreArgsStruct->ieimRegold.opcode)
		{
		case 16: //lw

			if ((coreArgsStruct->tsram[index] & 0x00000FFF) == tag && ((coreArgsStruct->tsram[index]) >> 12) != 0) //compare tag and check mesi state not valid
			{
				//cache hit

				//checking if this a real hit and not a hit after we miss and read the block
				if(coreArgsStruct->miss == 0 )(coreArgsStruct->readHit)++; //increment read hit counter 
				coreArgsStruct->miss = 0; // setting miss indicator to zero 
				coreArgsStruct->imwbRegnew.rdVal = coreArgsStruct->dsram[(index << 2) + offset]; // reading the data from cache

				
			}
			else //cache miss
			{

				if (((coreArgsStruct->tsram[index]) >> 12) == 3) // if block is modified a flush is needed
				{
					coreArgsStruct->miss = 1; // setting miss indicator to 1 for counting purposes 
					(coreArgsStruct->readMiss)++; //increment read miss counter
					//setting the transaction
					coreArgsStruct->transaction.bus_origid = coreArgsStruct->coreNum;
					coreArgsStruct->transaction.bus_cmd = 3;
					coreArgsStruct->transaction.bus_addr = ((coreArgsStruct->tsram[index] & 0x00000FFF) << 8) + (index << 2) + offset;
					memBubble = 1;
					coreArgsStruct->imwbRegnew.opcode = 18;
				}
				
				else //send busrd transaction to bus and put bubble in pipeline
				{
					coreArgsStruct->miss = 1;
					(coreArgsStruct->readMiss)++;
					coreArgsStruct->transaction.bus_origid = coreArgsStruct->coreNum;
					coreArgsStruct->transaction.bus_cmd = 1;
					coreArgsStruct->transaction.bus_addr = coreArgsStruct->ieimRegold.addrResolution;
					memBubble = 1;
					coreArgsStruct->imwbRegnew.opcode = 18;
				}
			}
			break;
		case 17://sw
			if (((coreArgsStruct->tsram[index] & 0x00000FFF) == tag )&& (((coreArgsStruct->tsram[index]) >> 12) != 0)){
					
		
					if (((coreArgsStruct->tsram[index]) >> 12) == 1){ //if block shared need to send busrdx to bus
						coreArgsStruct->miss = 1;
						(coreArgsStruct->writeMiss)++;
						coreArgsStruct->transaction.bus_origid = coreArgsStruct->coreNum;
						coreArgsStruct->transaction.bus_cmd = 2;
						coreArgsStruct->transaction.bus_addr = ((coreArgsStruct->tsram[index] & 0x00000FFF) << 8) + (index << 2) + offset;
						memBubble = 1;
						coreArgsStruct->imwbRegnew.opcode = 18;

					}
					else{
						//cache hit
						if(coreArgsStruct->miss == 0)(coreArgsStruct->writeHit)++;
						coreArgsStruct->miss = 0;
						coreArgsStruct->dsram[(index << 2) + offset] = coreArgsStruct->regfile[coreArgsStruct->ieimRegold.rd];
						coreArgsStruct->tsram[index] = (coreArgsStruct->tsram[index] | 0x00003000);
					}	
			}
			else { //cache miss

				if (((coreArgsStruct->tsram[index]) >> 12) == 3) {
					coreArgsStruct->miss = 1;
					(coreArgsStruct->writeMiss)++;
					coreArgsStruct->transaction.bus_origid = coreArgsStruct->coreNum;
					coreArgsStruct->transaction.bus_cmd = 3;
					coreArgsStruct->transaction.bus_addr = ((coreArgsStruct->tsram[index] & 0x00000FFF) << 8) + (index << 2) + offset;
					memBubble = 1;
					coreArgsStruct->imwbRegnew.opcode = 18;
				}
				else{
					coreArgsStruct->miss = 1;
					(coreArgsStruct->writeMiss)++;
					coreArgsStruct->transaction.bus_origid = coreArgsStruct->coreNum;
					coreArgsStruct->transaction.bus_cmd = 2;
					coreArgsStruct->transaction.bus_addr = coreArgsStruct->ieimRegold.addrResolution;
					memBubble = 1;
					coreArgsStruct->imwbRegnew.opcode = 18;
				}
			}
			break;

		case 20://halt
			coreArgsStruct->ieimRegnew.stateHalt = 1;//halting mem stage
			break;



		}
	}
	else // waiting for transaction to end
	{
		memBubble = 1;
		coreArgsStruct->imwbRegnew.opcode = 18;

	}





	//END MEM
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

	// WRITE BACK
	if (coreArgsStruct->imwbRegold.opcode != 18)//if not bubble
		(coreArgsStruct->totalInst)++; // total instruction counter

	if (coreArgsStruct->imwbRegold.opcode == 20)//halt is reached
		coreArgsStruct->corehalt = 1; // halting the core 
	// write the data to regfile making sure not to write to r0 r1
	else if (((coreArgsStruct->imwbRegold.opcode >= 0 && coreArgsStruct->imwbRegold.opcode <= 8) || coreArgsStruct->imwbRegold.opcode == 16) && coreArgsStruct->imwbRegold.rd != 0 && coreArgsStruct->imwbRegold.rd != 1)
	{
		coreArgsStruct->regfile[coreArgsStruct->imwbRegold.rd] = coreArgsStruct->imwbRegold.rdVal;
	}
	else if (coreArgsStruct->imwbRegold.opcode == 15)
		coreArgsStruct->regfile[15] = coreArgsStruct->imwbRegold.rdVal;

	// END WRITE BACK
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

	//END pipeline

	//handling coretrace file

	if (coreArgsStruct->ifidRegold.stateHalt == 1 || strcmp(coreArgsStruct->ifidRegnew.instruction, "12000000") == 0) //if fetch or decode not active or halted
	{
		strcpy(pcBuffF, "---");
	}
	else
	{
		sprintf(pcBuffF, "%03X", coreArgsStruct->ifidRegnew.pc);
	}



	if (coreArgsStruct->ifidRegold.stateHalt == 1 || strcmp(coreArgsStruct->ifidRegold.instruction, "12000000") == 0) //if fetch or decode not active or halted
	{
		strcpy(pcBuffD, "---");
	}
	else
	{
		sprintf(pcBuffD, "%03X", coreArgsStruct->ifidRegold.pc);
	}
	if (coreArgsStruct->idieRegold.stateHalt == 1 || (coreArgsStruct->idieRegold.opcode == 18))//if execute not active or halted
		strcpy(pcBuffE, "---");
	else
		sprintf(pcBuffE, "%03X", coreArgsStruct->idieRegold.pc);

	if (coreArgsStruct->ieimRegold.stateHalt == 1 || (coreArgsStruct->ieimRegold.opcode == 18)) //if mem not active or halted
		strcpy(pcBuffM, "---");
	else
		sprintf(pcBuffM, "%03X", coreArgsStruct->ieimRegold.pc);
	if (coreArgsStruct->imwbRegold.stateHalt == 1 || (coreArgsStruct->imwbRegold.opcode == 18))////if write back not active or halted
		strcpy(pcBuffW, "---");
	else
		sprintf(pcBuffW, "%03X", coreArgsStruct->imwbRegold.pc);


	


	fprintf(coreArgsStruct->coretraceP, "%ld %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s \n", clock, pcBuffF, pcBuffD, pcBuffE, pcBuffM, pcBuffW, regbuff[2],
		regbuff[3], regbuff[4], regbuff[5], regbuff[6], regbuff[7], regbuff[8], regbuff[9], regbuff[10], regbuff[11], regbuff[12], regbuff[13], regbuff[14], regbuff[15]);//write coretrace

	

	// set the "Q" of the FF in the pipline register to be equal to the "D" 
	//do it only if not bubble and not halt
	if (memBubble == 0)
	{
		if (decodeBubble == 0)
		{
			if (coreArgsStruct->ifidRegold.stateHalt == 0)
			{
				coreArgsStruct->pc = coreArgsStruct->pc + 1;
				if (coreArgsStruct->pc == 1024) coreArgsStruct->pc = 0;
				coreArgsStruct->ifidRegold = coreArgsStruct->ifidRegnew; // IF/ID old = IF/ID new (the end of the last cycle(new) is the begining of the current cycle(old)

			}
		}
		else if (decodeBubble == 1) coreArgsStruct->decodeStall++;
		if (coreArgsStruct->idieRegold.stateHalt == 0) coreArgsStruct->idieRegold = coreArgsStruct->idieRegnew; // ID/IE old = ID/IE new (the end of the last cycle(new) is the begining of the current cycle(old)



		if (coreArgsStruct->ieimRegold.stateHalt == 0) coreArgsStruct->ieimRegold = coreArgsStruct->ieimRegnew; // IE/IM old = IE/IM new (the end of the last cycle(new) is the begining of the current cycle(old)
	}
	else if (memBubble == 1) coreArgsStruct->memStall++;
	if (coreArgsStruct->imwbRegold.stateHalt == 0)coreArgsStruct->imwbRegold = coreArgsStruct->imwbRegnew; // IM/WB old = IM/WB new (the end of the last cycle(new) is the begining of the current cycle(old)

	if (coreArgsStruct->corehalt == 1) //if whole core finished the job
	{
		coreArgsStruct->totalClock = clock + 1;//set total cycles when core finished

	}
}
