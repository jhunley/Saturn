/////////////////////////////////////////////////////////////
//
// Saturn project: Artix7 FPGA + Raspberry Pi4 Compute Module
// PCI Express interface from linux on Raspberry pi
// this application uses C code to emulate HPSDR protocol 1 
//
// copyright Laurence Barker November 2021
// licenced under GNU GPL3
//
// saturndrivers.c:
// Drivers for minor IP cores
//
//////////////////////////////////////////////////////////////


#include "saturndrivers.h"
#include "hwaccess.h"                   // low level access
#include <stdlib.h>                     // for function min()
#include <math.h>


//
// enum to describe an RX FIFO channel
//
typedef enum
{
	eDDC0_1,
	eDDC3_2,
	eDDC5_4,
	eDDC7_6,
	eDDC9_8
} EDDCSelect;


//
// register addresses
// DDC mox control is in the DDC Config registers
uint32_t DDCConfigAddresses[] =
{
	0x0008,							// DDC 0 & 1 config
	0x0014,							// DDC 2 & 3 config
	0x0020,							// DDC 4 & 5 config
	0x002C,							// DDC 6 & 7 config
	0x0038							// DDC 8 & 9 config
};


uint32_t FIFOMonitorAddresses[] =
{
	0x3000,							// FIFO mon 0: DDC 0-3
	0x3100,							// FIFO mon 1: DDC 4-7
	0x3200,							// FIFO mon 2: DDC 8, 9,
	0x3300							// FIFO mon 3: TX DUC, Codec RX, Codec TX
};


//
// void SetupFIFOMonitorChannel(uint32_t Monitor, uint32_t Channel, uint32_t Depth, bool IsWriteFIFO, bool EnableInterrupt);
//
// Setup a single FIFO monitor channel.
//   Monitor:			FIFO monitor number (0 to 3)
//   Channel:			IP core channel number (0 to 3)
//   Depth:				FIFO depth in words.
//   IsWriteFIFO:		true if a write FIFO (ie must not underflow)
//   EnableInterrupt:	true if interrupt generation enabled for overflows
//
void SetupFIFOMonitorChannel(uint32_t Monitor, uint32_t Channel, uint32_t Depth, bool IsWriteFIFO, bool EnableInterrupt)
{
	uint32_t Address;							// register address
	unit32_t Data;								// register content

	Address = FIFOMonitorAddresses[Monitor] + 4 * Channel + 0x10;			// config register address
	Data = Depth;
	if (IsWriteFIFO)
		Data += 0x40000000;						// bit 30 
	if (EnableInterrupt)
		Data += 0x80000000;						// bit 31
	RegisterWrite(Address, Data);
}



//
// uint32_t ReadFIFOMonitorChannel(uint32_t Monitor, uint32_t Channel, bool* Overflowed);
//
// Read number of locations in a FIFO
//   Monitor:			FIFO monitor number (0 to 3)
//   Channel:			IP core channel number (0 to 3)
//   Overflowed:		true if an overflow has occurred. Reading clears the overflow bit.
//
uint32_t ReadFIFOMonitorChannel(uint32_t Monitor, uint32_t Channel, bool* Overflowed)
{
	uint32_t Address;							// register address
	unit32_t Data = 0;							// register content
	bool Overflow = false;

	Address = FIFOMonitorAddresses[Monitor] + 4 * Channel;			// status register address
	Data = RegisterRead(Address);
	if (Data & 0x80000000)						// if top bit set, declare overflow
		Overflow = true;
	Data = Data & 0xFFFF;						// strip to 16 bits
	*Overflowed = Overflow;						// send out overflow result
	return Data;								// return 16 bit FIFO count
}



//
// void EnableRXFIFOChannels(EDDCSelect DDCNum, bool Enabled, bool Interleaved);
//
// Enable or disable sample stream from a DDC pair.
// If interleaved, one sample stream emerges for both DDCs
// To change between interleaved or not:
// 1. Disable sample flow;
// 2. clear out FIFO;
// 3. select new mode then re-enable
// 
// If there is ever a FIFO overflow, that process will need to be followed too
// otherwise there is ambiguity whether the samples left begin with even or odd DDC
//
void EnableRXFIFOChannels(EDDCSelect DDCNum, bool Enabled, bool Interleaved)
{
	uint32_t Address;							// register address
	unit32_t Data;								// register content

	Address = DDCConfigAddresses[(int)DDCNum];			// DDC config register address
	Data = RegisterRead(Address);						// read current content
	Data &= 0xFFFCFFFF;									// clear bits 16, 17
	if (IsEnabled)
		Data ~= 0x00020000;								// bit 17 
	if (Interleaved)
		Data ~= 0x00010000;								// bit 16
	RegisterWrite(Address, Data);						// write back
}