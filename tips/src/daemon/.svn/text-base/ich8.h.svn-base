//****************************************
// Name:	ich8.h
//
// Dscription:	
//****************************************

#ifndef _ICH8_H_
#define _ICH8_H_

// include .H file
#include <stdio.h>
#include <sys/io.h>
#include <unistd.h>
#include <stdlib.h>

// data type
typedef signed char		INT8;
typedef unsigned char	UINT8;
typedef signed short	INT16;
typedef unsigned short	UINT16;
typedef signed int		INT32;
typedef unsigned int	UINT32;

// debug_mode
// #define DEBUG_MODE
#if defined (DEBUG_MODE)
#define Trace(Arguments)		printf Arguments
#else
#define Trace(Arguments)
#endif

//
#define IO_MODE_IN	1
#define IO_MODE_OUT	0

// define ICH8 LPC Interface Bridge (D31:F0)
#define SB_BUS				0		// South Bridge Bus Number
#define SB_DEV				0x1f	// South Bridge Device Number
#define SB_FUNC				0		// South Bridge Function Number
#define SB_REG_GPIOBASE		0x48	// GPIO Base Address Reg

#define PCI_INDEX_GPIO (\
		((SB_BUS << 16) + (SB_DEV << 11) + (SB_FUNC << 8) + (SB_REG_GPIOBASE)) \
		| 0x80000000 \
		& 0xfffffffc)

// define
#define getgroup(pin)	((pin) / 32)
#define getshift(pin)	((pin) % 32)
#define getmask(pin)	(1 << getshift(pin))

// define ICH8 GPIO Funtion
UINT16 Ich_Get_GPIOBase(void);
UINT16 Ich_Get_GPIO_USE_SEL(UINT8 Pin);
UINT16 Ich_Get_GP_IO_SEL(UINT8 Pin);
UINT16 Ich_Get_GP_LVL(UINT8 Pin);

void Ich_SetGpioMode(UINT8 Pin, UINT8 Mode);
void Ich_SetGpiolevel(UINT8 Pin, UINT8 Level);
void Ich_GpioInitialization(UINT8 *PinArray, UINT8 TotalNumber);

UINT8 Ich_QueryGpioStatus(UINT8 Pin, UINT8 *Level)
	// Output:  return = 1, input mode (IO_MODE_IN)
	//                   0, output mode (IO_MODE_OUT)
	;

#endif
