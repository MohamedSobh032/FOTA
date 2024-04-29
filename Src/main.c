/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "LBIT_MATH.h"
#include "LSTD_TYPES.h"
#include "MRCC_Interface.h"
#include "MGPIO_Interface.h"
#include "MSTK_Interface.h"
#include "MUSART_Interface.h"
#include "MFMI_Interface.h"
#include "SHPR_Interface.h"
#include "main.h"

u8 BOOT_u8DataRecord[115] = {0};			/* Data Records Array */
u8 BOOT_u8Buffer = 0;						/* Ring Buffer Variable */
u8 BOOT_u8ContFlag = 0;						/* Continue or Not Flag */
u8 BOOT_u8RecordLength = 0;					/* Record Length
											 * Will be used for Double Record Mode
											 */
u8 BOOT_u8TimeOut = 0;						/* Timeout Flag for Receiving the Ring Buffer */
									/*************  Booting Sequence  *************/
void BOOT_voidJump(void)
{
	/* Set the MSP */
	u32 MSP_VALUE = *((volatile u32*)0x08004000);
	__asm volatile("MSR MSP,%0"::"r"(MSP_VALUE));
	/* Set the Vector Table */
	*((volatile u32*) 0xE000ED08) = 0x08004000;
	/* Jump to the Reset Handler of the APPLICATION CODE */
	void (*BOOT_voidJump)(void);									/* Create a Pointer to Function */
	u32 BOOT_ResetHandlerAddress = *((volatile u32*)0x08004004); 	/* Variable of the Address of the Reset Handler */
	BOOT_voidJump = (void*)BOOT_ResetHandlerAddress;				/* Point it to the Address of the Reset Handler */
	BOOT_voidJump();												/* Jump to the Reset Handler */
}
									/*************   Hardware Reset   *************/
void BOOT_voidHWReset(void)
{
	MGPIO_voidSetPinValue(ESP8266_RESET_PIN, MGPIO_OUTPUT_HIGH);
	MSTK_voidBusyWait(1000);
	MGPIO_voidSetPinValue(ESP8266_RESET_PIN, MGPIO_OUTPUT_LOW);
	MSTK_voidBusyWait(1000);
	MGPIO_voidSetPinValue(ESP8266_RESET_PIN, MGPIO_OUTPUT_HIGH);
}
									/*************  ESP8266 Settings  *************/
void BOOT_voidESPSettings(void)
{
	/* Hardware Reset the Module */
	BOOT_voidHWReset();
	/* Initialize the Wanted Settings of the ESP8266
	 * NO ECHOEING
	 * STATION MODE
	 */
	/* Disable the Echoeing Response of the ESP8266 */
	do {
		BOOT_u8ContFlag = 0;
		BOOT_u8TimeOut = 0;
		MUSART_voidTransmitString(BOOT_USART,(u8*)"ATE0\r\n");
		/* Read the Buffer */
		while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
			MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
			if (BOOT_u8Buffer == 'O') { 										/* if 'O', Read Next Element */
				MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
				if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }		/* 'K' Means '\r\nOK' --> break */
			} else if (BOOT_u8Buffer == 'R') {									/* if 'R', Read the Next Element */
				MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
				if (BOOT_u8Buffer == 'R') { BOOT_u8ContFlag = 0; break; }		/* 'K' Means '\r\nERROR' --> break */
			} else if (BOOT_u8Buffer == 'b') { BOOT_voidHWReset(); break; }		/* 'b' Means 'busy' --> Reset the Module */
			BOOT_u8TimeOut++;
		}
	} while(BOOT_u8ContFlag == 0);
	/* Set Station Mode */
	do {
		BOOT_u8ContFlag = 0;
		BOOT_u8TimeOut = 0;
		MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CWMODE=1\r\n");
		/* Read the Buffer */
		while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
			MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
			if (BOOT_u8Buffer == 'O') { 										/* if 'O', Read Next Element */
				MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
				if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }		/* 'K' Means '\r\nOK' --> break */
			} else if (BOOT_u8Buffer == 'R') {									/* if 'R', Read the Next Element */
				MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
				if (BOOT_u8Buffer == 'R') { BOOT_u8ContFlag = 0; break; }		/* 'K' Means '\r\nERROR' --> break */
			} else if (BOOT_u8Buffer == 'b') { BOOT_voidHWReset(); break; }		/* 'b' Means 'busy' --> Reset the Module */
			BOOT_u8TimeOut++;
		}
	} while(BOOT_u8ContFlag == 0);
}
									/************* Start of Boot Code *************/
int main(void)
{
	/* Initialize System Clock */
	MRCC_voidInitSysAndBusClock();
	/* Enable Wanted Peripherals Clock */
	MRCC_voidEnablePeriphClock(MRCC_BUS_AHB1,   MRCC_AHB1_GPIOAEN);
	MRCC_voidEnablePeriphClock(MRCC_BUS_AHB1LP, MRCC_AHB1LP_FLITFLPEN);
	MRCC_voidEnablePeriphClock(MRCC_BUS_APB2,   MRCC_APB2_USART1EN);
	/* Initialize Used Pins */
	MGPIO_voidSetPinMode(ESP8266_RESET_PIN, MGPIO_MODE_OUTPUT);
	MGPIO_voidSetPinOutputSpeed(ESP8266_RESET_PIN, MGPIO_LOW_SPEED);
	MGPIO_voidSetPinOutputType(ESP8266_RESET_PIN, MGPIO_OUTPUT_TYPE_PP);
	MGPIO_voidSetPinValue(ESP8266_RESET_PIN, MGPIO_OUTPUT_HIGH);
	MGPIO_voidSetPinMode(GPIOA, MGPIO_PIN09, MGPIO_MODE_ALTERNATE);
	MGPIO_voidSetPinMode(GPIOA, MGPIO_PIN10, MGPIO_MODE_ALTERNATE);
	MGPIO_voidSetPinAFDirection(GPIOA, MGPIO_PIN09, MGPIO_AF07);
	MGPIO_voidSetPinAFDirection(GPIOA, MGPIO_PIN10, MGPIO_AF07);
	/* Initialize USART */
	MUSART_InitTypeDef uart = {115200, MUSART_DATAWIDTH_8BIT,
							MUSART_STOP_ONE_BIT, MUSART_DISABLE,
							MUSART_PARITY_EVEN, MUSART_DIRECTION_TX_RX,
							MUSART_DISABLE, MUSART_OVER_SAMPLING_16};
	MUSART_ClockInitTypeDef uart_clock = {MUSART_DISABLE,0,0,0};
	MUSART_voidInit(BOOT_USART, &uart, &uart_clock);
	MUSART_voidEnable(BOOT_USART);
	MUSART_voidRxIntStatus(BOOT_USART,MUSART_DISABLE);
	/* Initialize Systick for the Busy Flag */
	MSTK_voidInit();
	/* Initialize Flash Driver */
	MFMI_voidInit();
	/* Initialize ESP8266 Settings */
	BOOT_voidESPSettings();
	/* Get the Firmware Status */
	do {
		/* Connect using TCP Protocol */
		do {
			BOOT_u8ContFlag = 0;
			BOOT_u8TimeOut = 0;
			MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CIPSTART=\"TCP\",\"69.197.143.14\",80\r\n");
			/* Read the Buffer */
			while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
				MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
				if (BOOT_u8Buffer == 'b') { BOOT_voidESPSettings(); break; } 		/* 'b' Means 'busy' --> Reset the Module */
				else if (BOOT_u8Buffer == 'O') { 									/* if 'O', Read Next Element */
					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
					if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }		/* 'K' Means 'CONNECT\r\nOK' --> break */
					else if (BOOT_u8Buffer == 'R') {BOOT_u8ContFlag = 0; break; }	/* 'R' Means 'ERROR' --> Connect Again */
				}
				else if (BOOT_u8Buffer == 'Y') { 									/* if 'Y', Read Next Element */
					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
					if (BOOT_u8Buffer == ' ') { BOOT_u8ContFlag = 1; break; }		/* ' ' Means 'ALREADY CONNECTED' --> break */
				}
				BOOT_u8TimeOut++;
			}
		} while(BOOT_u8ContFlag == 0);
		/* TCP is now Connected, Send Data Length
		 * AT+CIPSEND=<DataLength>
		 * BOOT_u8ContFlag = 2 --> Skip Data Get URL Command
		 */
		do {
			BOOT_u8ContFlag = 0;
			BOOT_u8TimeOut = 0;
			MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CIPSEND=50\r\n");
			/* Read the Buffer */
			while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
				MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
				if (BOOT_u8Buffer == 'v') { BOOT_u8ContFlag = 2; break; }	 		/* 'v' Means 'link is not valid' --> Connect Again */
				else if (BOOT_u8Buffer == 'O') { 									/* if 'O', Read Next Element */
					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
					if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }		/* 'K' Means '\r\nOK' --> break */
				} else if (BOOT_u8Buffer == 'b') { BOOT_voidESPSettings(); break; }	/* 'b' Means 'busy' --> Reset the Module */
				BOOT_u8TimeOut++;
			}
		} while (BOOT_u8ContFlag == 0);
		/* Length is now Sent, Start Sending Data
		 * GET STATUS
		 * O --> Same Old Firmware in the Flash
		 * N --> New Firmware to be Updated
		 */
		if (BOOT_u8ContFlag != 2) {
			BOOT_u8ContFlag = 0;
			BOOT_u8TimeOut = 0;
			MUSART_voidTransmitString(BOOT_USART,(u8*)"GET http://sobhhhh.freevar.com/ReadorNotFile.txt\r\n");
			/* Read the Buffer */
			while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
				MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
				if (BOOT_u8Buffer == '+') {
					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
					if (BOOT_u8Buffer == 'N' || BOOT_u8Buffer == 'O') { BOOT_u8ContFlag = 1; break; }
				}
				BOOT_u8TimeOut++;
			}
		}
	} while (BOOT_u8ContFlag == 0 || BOOT_u8ContFlag == 2);
    if (BOOT_u8Buffer == 'O') { BOOT_voidJump(); } 			/* if O --> Jump to the Booting Sequence With No Updating */
    else if (BOOT_u8Buffer == 'N') 							/* if N --> Start FOTA Sequence */
    {
    	/* Line Counters */
    	u8 BOOT_u8CounterD0 = 1;
    	u8 BOOT_u8CounterD1 = '0';
    	u8 BOOT_u8CounterD2 = '0';
    	/* Erase the Current Firmware
    	 * CONFIGURABLE IN main.h
    	 * By Increasing or Decreasing the Wanted Sectors
    	 * Line Counters
    	 */
    	for (BOOT_u8CounterD0 = 1; BOOT_u8CounterD0 < _SECTORS_TO_BE_ERASED_+1; BOOT_u8CounterD0++) {
    		MFMI_voidSectorErase(BOOT_u8CounterD0);
    	}
    	BOOT_u8CounterD0 = '0';
    	/* Fetch the New Firmware
    	 * Get the New Record Lines
    	 */
    	while (1)
    	{
    		do {
    			/* Connect using TCP Protocol */
    			do {
    				BOOT_u8ContFlag = 0;
    				BOOT_u8TimeOut = 0;
    				MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CIPSTART=\"TCP\",\"69.197.143.14\",80\r\n");
    				/* Read the Buffer */
    				while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
    					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
    					if (BOOT_u8Buffer == 'b') { BOOT_voidESPSettings(); break; }		/* 'b' Means 'busy' --> Reset the Module */
    					else if (BOOT_u8Buffer == 'O') { 									/* if 'O', Read Next Element */
    						MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
    						if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }		/* 'K' Means 'CONNECT\r\nOK' --> break */
    						else if (BOOT_u8Buffer == 'R') {BOOT_u8ContFlag = 0; break; }	/* 'R' Means 'ERROR' --> Connect Again */
    					} else if (BOOT_u8Buffer == 'Y') { 									/* if 'Y', Read Next Element */
    						MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
    						if (BOOT_u8Buffer == ' ') { BOOT_u8ContFlag = 1; break; }		/* ' ' Means 'ALREADY CONNECTED' --> break */
    					}
    					BOOT_u8TimeOut++;
    				}
    			} while(BOOT_u8ContFlag == 0);
    			/* TCP is now Connected, Send Data Length
    			 * AT+CIPSEND=<DataLength>
    			 * BOOT_u8ContFlag = 2 --> Skip Data Get URL Command
    			 */
        		do {
        			BOOT_u8ContFlag = 0;
        			BOOT_u8TimeOut = 0;
        			if (BOOT_u8CounterD2 != '0')      { MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CIPSEND=53\r\n"); }
        			else if (BOOT_u8CounterD1 != '0') { MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CIPSEND=52\r\n"); }
        			else 							  { MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CIPSEND=51\r\n"); }
        			/* Read the Buffer */
        			while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
        				MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
        				if (BOOT_u8Buffer == 'v') { BOOT_u8ContFlag = 2; break; } 	 	/* 'v' Means 'link is not valid' --> Connect Again */
        				else if (BOOT_u8Buffer == 'O') { 								/* if 'O', Read Next Element */
        					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
        					if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }		/* 'K' Means '\r\nOK' --> break */
        				} else if (BOOT_u8Buffer == 'b') { BOOT_voidESPSettings(); break; }	/* 'b' Means 'busy' --> Reset the Module */
        				BOOT_u8TimeOut++;
        			}
        		} while (BOOT_u8ContFlag == 0);
        		/* Get the Data Record from the Website */
        		if (BOOT_u8ContFlag != 2) {
        			MUSART_voidTransmitString(BOOT_USART,(u8*)"GET http://sobhhhh.freevar.com/getline.php?line=");
        			if (BOOT_u8CounterD2 != '0') {											/* If Not '0' --> Hundreds */
        				MUSART_voidTransmitByte(BOOT_USART,BOOT_u8CounterD2);
        				MUSART_voidTransmitByte(BOOT_USART,BOOT_u8CounterD1);
        				MUSART_voidTransmitByte(BOOT_USART,BOOT_u8CounterD0);
        			} else if (BOOT_u8CounterD1 != '0') {									/* If Not '0' --> Tens */
        				MUSART_voidTransmitByte(BOOT_USART,BOOT_u8CounterD1);
        				MUSART_voidTransmitByte(BOOT_USART,BOOT_u8CounterD0);
        			} else { MUSART_voidTransmitByte(BOOT_USART,BOOT_u8CounterD0); }		/* If Both '0' --> Ones*/
        			MUSART_voidTransmitString(BOOT_USART,(u8*)"\r\n");
        			/* Receive the Data Record */
        			MUSART_u8ReceiveStringSynchBlocking(BOOT_USART,BOOT_u8DataRecord);
        		}
    		} while (BOOT_u8DataRecord[21] != ':' && BOOT_u8DataRecord[22] != ':');
    		/* Parse and Flash */
    		SHPR_voidParseHexRecord(&BOOT_u8DataRecord[22]);
    		BOOT_u8RecordLength = SHPR_u8GetRecordLength(&BOOT_u8DataRecord[22]);
    		SHPR_voidParseHexRecord(&BOOT_u8DataRecord[35+(BOOT_u8RecordLength*2)]);
    		/* Restart the Buffer */
    		BOOT_u8DataRecord[20] = 0; BOOT_u8DataRecord[21] = 0;
    		/* AT+CIPCLOSE
    		 * Ensure that Connection is Closed
    		 * Always Closes After Receiving But For Extra Ensuring
    		 */
    		do {
    			BOOT_u8ContFlag = 0;
    			BOOT_u8TimeOut = 0;
    			MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CIPCLOSE\r\n");
    			/* Read Buffer */
    			while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
    				MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
    				if (BOOT_u8Buffer == 'O') { 											/* if 'O', Read Next Element */
    					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
    					if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }			/* 'K' Means '\r\nOK' --> break */
    				} else if (BOOT_u8Buffer == 'E') { 										/* if 'E', Read Next Element */
    					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
    					if (BOOT_u8Buffer == 'R') { BOOT_u8ContFlag = 1; break; }			/* 'R' Means '\r\nERROR' --> break */
    				} else if (BOOT_u8Buffer == 'b') { BOOT_voidESPSettings(); break; }		/* 'b' Means 'busy' --> Reset the Module */
    				BOOT_u8TimeOut++;
    			}
    		} while (BOOT_u8ContFlag == 0);
    	    /* Increment the Counter */
    	    BOOT_u8CounterD0 += 2;
    	    if (BOOT_u8CounterD0 >= 58) { BOOT_u8CounterD0 -= 10; BOOT_u8CounterD1++;
    	    	if (BOOT_u8CounterD1 >= 58) { BOOT_u8CounterD1 -= 10; BOOT_u8CounterD2++; }
    	    }
			/* Check if it was the Last Record Received */
			if (BOOT_u8DataRecord[30] == '1' || BOOT_u8DataRecord[43+(BOOT_u8RecordLength*2)] == '1') {
				/* Set the Status of the Firmware to Old
				 * So When Fetching Again, it Does Not Update it
				 */
				do {
					/* Connect using TCP Protocol */
					do {
						BOOT_u8ContFlag = 0;
						BOOT_u8TimeOut = 0;
						MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CIPSTART=\"TCP\",\"69.197.143.14\",80\r\n");
						/* Read the Buffer */
						while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
							MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
							if (BOOT_u8Buffer == 'b') { BOOT_voidESPSettings(); break; }	/* 'b' Means 'busy' --> Reset the Module */
							else if (BOOT_u8Buffer == 'O') { 								/* if 'O', Read Next Element */
								MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
								if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }	/* 'K' Means 'CONNECT\r\nOK' --> break */
							} else if (BOOT_u8Buffer == 'Y') { 								/* if 'Y', Read Next Element */
								MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
								if (BOOT_u8Buffer == ' ') { BOOT_u8ContFlag = 1; break; }	/* ' ' Means 'ALREADY CONNECTED' --> break */
							}
							BOOT_u8TimeOut++;
						}
					} while(BOOT_u8ContFlag == 0);
					/* TCP is now Connected, Send Data Length
					 * AT+CIPSEND=<DataLength>
					 * BOOT_u8ContFlag = 2 --> Skip Data Get URL Command
					 */
					do {
						BOOT_u8ContFlag = 0;
						BOOT_u8TimeOut = 0;
						MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CIPSEND=50\r\n");
						/* Read the Buffer */
						while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
							MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
							if (BOOT_u8Buffer == 'v') { BOOT_u8ContFlag = 2; break; } 	/* 'v' Means 'link is not valid' --> Connect Again */
							else if (BOOT_u8Buffer == 'O') { 								 	 /* if 'O', Read Next Element */
								MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
								if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }		 /* 'K' Means '\r\nOK' --> break */
							}  else if (BOOT_u8Buffer == 'b') { BOOT_voidESPSettings(); break; } /* 'b' Means 'busy' --> Reset the Module */
							BOOT_u8TimeOut++;
						}
					} while (BOOT_u8ContFlag == 0);
					/* Length is now Sent, Start Sending Data */
					if (BOOT_u8ContFlag != 2) {
						BOOT_u8ContFlag = 0;
						BOOT_u8TimeOut = 0;
						MUSART_voidTransmitString(BOOT_USART,(u8*)"GET http://sobhhhh.freevar.com/getline.php?end\r\n");
						/* Read the Buffer */
						while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
							MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
							if (BOOT_u8Buffer == 'O') { 										 /* if 'O', Read Next Element */
								MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
								if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }		 /* 'K' Means '\r\nOK' --> break */
							}  else if (BOOT_u8Buffer == 'b') { BOOT_voidESPSettings(); break; } /* 'b' Means 'busy' --> Reset the Module */
							BOOT_u8TimeOut++;
						}
					}
				} while (BOOT_u8ContFlag == 0 || BOOT_u8ContFlag == 2);
	    		do {
	    			BOOT_u8ContFlag = 0;
	    			BOOT_u8TimeOut = 0;
	    			MUSART_voidTransmitString(BOOT_USART,(u8*)"AT+CIPCLOSE\r\n");
	    			/* Read Buffer */
	    			while (BOOT_u8TimeOut < BOOT_THRESHOLD_VALUE) {
	    				MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
	    				if (BOOT_u8Buffer == 'O') { 											/* if 'O', Read Next Element */
	    					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
	    					if (BOOT_u8Buffer == 'K') { BOOT_u8ContFlag = 1; break; }			/* 'K' Means '\r\nOK' --> break */
	    				} else if (BOOT_u8Buffer == 'E') { 										/* if 'E', Read Next Element */
	    					MUSART_u8ReceiveByteSynchBlocking(BOOT_USART, &BOOT_u8Buffer);
	    					if (BOOT_u8Buffer == 'R') { BOOT_u8ContFlag = 1; break; }			/* 'R' Means '\r\nERROR' --> break */
	    				} else if (BOOT_u8Buffer == 'b') { BOOT_voidESPSettings(); break; }		/* 'b' Means 'busy' --> Reset the Module */
	    				BOOT_u8TimeOut++;
	    			}
	    		} while (BOOT_u8ContFlag == 0);
				BOOT_voidJump();
    		}
    	}								/************* End of While Loop    *************/
    }									/************* End of FOTA Sequence *************/
}										/************* End of Project Code  *************/
