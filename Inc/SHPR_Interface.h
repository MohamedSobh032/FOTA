/**********************************************/
/* SWC: HPR Driver                            */
/* Author: Mohamed Sobh                       */
/* Version: v0.0                              */
/* Date: 18 JAN 2024                          */
/* Description: This is the implem. of HPR    */
/**********************************************/
/* File Guard */
#ifndef _SHPR_INTERFACE_H_
#define _SHPR_INTERFACE_H_

u8 SHPR_u8GetRecordLength(u8* Copy_u8DataRecord);
void SHPR_voidParseHexRecord(u8* Copy_u8DataRecord);

#endif /* _SHPR_INTERFACE_H_ */