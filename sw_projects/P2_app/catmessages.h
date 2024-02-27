/////////////////////////////////////////////////////////////
//
// Saturn project: Artix7 FPGA + Raspberry Pi4 Compute Module
// PCI Express interface from linux on Raspberry pi
// this application uses C code to emulate HPSDR protocol 2 
//
// copyright Laurence Barker November 2021
// licenced under GNU GPL3
//
// catmessages.h:
//
// handle incoming CAT messages
//
//////////////////////////////////////////////////////////////

#ifndef __catmessages_h
#define __catmessages_h


//
// VFO A frequency 
//
void HandleZZFA(void);


void HandleZZZD(void);
void HandleZZZU(void);
void HandleZZZE(void);
void HandleZZZP(void);
void HandleZZZI(void);
void HandleZZZS(void);


#endif  //#ifndef