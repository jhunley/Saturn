/////////////////////////////////////////////////////////////
//
// Saturn project: Artix7 FPGA + Raspberry Pi4 Compute Module
// PCI Express interface from linux on Raspberry pi
// this application uses C code to emulate HPSDR protocol 2 
//
// copyright Laurence Barker November 2021
// licenced under GNU GPL3
//
// catmessages.c:
//
// handle incoming CAT messages
//
//////////////////////////////////////////////////////////////

#include "threaddata.h"
#include <stdint.h>
#include "../common/saturntypes.h"
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../common/saturnregisters.h"
#include "../common/saturndrivers.h"
#include "../common/hwaccess.h"
#include "../common/debugaids.h"
#include "g2v2panel.h"
#include "catmessages.h"
#include "cathandler.h"


//
// CAT handlers pick up their parameters from global values
// (this is done because then one jump table can be used for all)
// the parsed result will be in ParsedString, ParsedInt or ParsedBool as set in message table
//



//
// ZZFA
// only really here for test - not used operationally
//
void HandleZZFA(void)
{
    printf("ZZFA: Frequency=%s\n", ParsedString);
}


//
// combined VFO status 
//
void HandleZZXV(void)                          // VFO status
{
    SetG2V2ZZXVState((uint32_t)ParsedInt);
//    printf("ZZXV: param=%04x\n", ParsedInt);
}


//
// 2 Tone test 
//
void HandleZZUT(void)                          // 2 tone test
{
    SetG2V2ZZUTState(ParsedBool);
    //printf("ZZUT: param=%04x\n", (int)ParsedBool);
}


//
// RX1/RX2
//
void HandleZZYR(void)                          // RX1/2
{
    SetG2V2ZZYRState(ParsedBool);
//    printf("ZZUT: param=%04x\n", (int)ParsedBool);
}