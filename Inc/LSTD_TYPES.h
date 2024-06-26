/******************************************************/
/* SWC: STD Types                                     */
/* Author: Mohamed Sobh                               */
/* Version: v2.0                                      */
/* Date: 02 APR 2024                                  */
/* Description: This is the implem. of Standard Types */
/******************************************************/
/* Header Guard File */
#ifndef _LSTD_TYPES_H_
#define _LSTD_TYPES_H_

typedef char*				p8;

typedef unsigned char 	    u8;
typedef unsigned short int	u16;
typedef unsigned long int   u32;

typedef signed char 		s8;
typedef signed short int	s16;
typedef signed long int		s32;

typedef float 			    f32;
typedef double              u64;

typedef enum {
	false = 0,
	true = 1
} bool;

#endif /* _LSTD_TYPES_H_ */
