/********************************************/
/* SWC: FMI Driver                          */
/* Author: Mohamed Sobh                     */
/* Version: v0.0                            */
/* Date: 18 JAN 2024                        */
/* Description: This is the implem. of FMI  */
/********************************************/

#include <LBIT_MATH.h>
#include <LSTD_TYPES.h>
#include "MFMI_Config.h"
#include "MFMI_Interface.h"
#include "MFMI_Private.h"

/*************************************************/
/* Func. Name: MFMI_voidInit                     */
/* i/p arguments: nothing 					     */
/* o/p arguments: nothing                        */
/* Desc. : This API initializes the Flash Driver */
/*************************************************/
void MFMI_voidInit(void)
{
	/* Unlock the Flash */
	Private_voidFlashUnlock();
	/* Set the Programming Size */
	WRITE_BITS(FMI->CR,MFMI_PROGRAM_SIZE,TWO_BITS,MFMI_CR_PSIZE_BIT);
	/* Lock the Flash */
	Private_voidFlashLock();
}

/***********************************************************/
/* Func. Name: MFMI_voidSectorErase                        */
/* i/p arguments: Copy_u8SectorNumber: Sector to be Erased */
/* o/p arguments: nothing                              	   */
/* Desc. : This API Erases a Full Sector in the Flash      */
/***********************************************************/
void MFMI_voidSectorErase(u8 Copy_u8SectorNumber)
{
	/* Wait for the Flash to be not Busy */
	while(GET_BIT(FMI->SR,MFMI_SR_BSY_BIT));
	/* Unlock the Flash */
	Private_voidFlashUnlock();
	/* Select Sector Number */
	WRITE_BITS(FMI->CR,Copy_u8SectorNumber,FOUR_BITS,MFMI_CR_SNB_BIT);
	/* Select Erase Operation */
	SET_BIT(FMI->CR,MFMI_CR_SER_BIT);
	/* Start Operation */
	SET_BIT(FMI->CR,MFMI_CR_STRT_BIT);
	/* Wait for the Flash to be not Busy */
	while(GET_BIT(FMI->SR,MFMI_SR_BSY_BIT));
	/* Clear EOP by Writing 1 */
	SET_BIT(FMI->SR,MFMI_SR_EOP_BIT);
	/* Deselect Erase Operation */
	CLR_BIT(FMI->CR,MFMI_CR_SER_BIT);
	/* Lock the Flash */
	Private_voidFlashLock();
}

/*********************************************************************************/
/* Func. Name: MFMI_voidFlashByte                                                */
/* i/p arguments: Copy_u32Address: Start Address of the Data to be Written       */
/* i/p arguments: Copy_u16DataToBeWritten: Array Contains the Data to be Written */
/* i/p arguments: Copy_u16Length: Array Size 									 */
/* o/p arguments: nothing                              				   			 */
/* Desc. : This API Flashes an Array at a Specific Address 			   			 */
/*********************************************************************************/
void MFMI_voidProgram(u32 Copy_u32Address, u16* Copy_u16DataToBeWritten, u16 Copy_u16Length)
{
	/* Wait for the Flash to be not Busy */
	while(GET_BIT(FMI->SR,MFMI_SR_BSY_BIT));
	/* Unlock the Flash */
	Private_voidFlashUnlock();
	/* Select Programming Operation */
	SET_BIT(FMI->CR,MFMI_CR_PG_BIT);
	/* Write The Data to be Written */
	u16 Local_u16Counter = 0;
	for (Local_u16Counter = 0; Local_u16Counter < Copy_u16Length; Local_u16Counter++)
	{
		*(volatile u16*)Copy_u32Address = Copy_u16DataToBeWritten[Local_u16Counter];
		Copy_u32Address += 2;
		/* Wait for the Flash to be not Busy */
		while(GET_BIT(FMI->SR,MFMI_SR_BSY_BIT));
		/* Clear EOP by Writing 1 */
		SET_BIT(FMI->SR,MFMI_SR_EOP_BIT);
	}
	/* Deselect Programming Operation */
	CLR_BIT(FMI->CR,MFMI_CR_PG_BIT);
	/* Lock the Flash */
	Private_voidFlashLock();
}

/************************************/
/* Func. Name: MFMI_voidLock        */
/* i/p arguments: nothing 		    */
/* o/p arguments: nothing           */
/* Desc. : This API Locks the Flash */
/************************************/
static void Private_voidFlashLock()
{ SET_BIT(FMI->CR,MFMI_CR_LOCK_BIT); }

/**************************************/
/* Func. Name: MFMI_voidUnlock        */
/* i/p arguments: nothing 		      */
/* o/p arguments: nothing             */
/* Desc. : This API Unlocks the Flash */
/**************************************/
static void Private_voidFlashUnlock()
{
	FMI->KEYR = MFMI_KEYR_KEY1;
	FMI->KEYR = MFMI_KEYR_KEY2;
}
