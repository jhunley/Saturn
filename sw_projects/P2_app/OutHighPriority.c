/////////////////////////////////////////////////////////////
//
// Saturn project: Artix7 FPGA + Raspberry Pi4 Compute Module
// PCI Express interface from linux on Raspberry pi
// this application uses C code to emulate HPSDR protocol 2 
//
// copyright Laurence Barker November 2021
// licenced under GNU GPL3
// derived from Pavel Demin code 
//
// OutHighPriority.c:
//
// handle "outgoing high priority data" message
//
//////////////////////////////////////////////////////////////

#include "threaddata.h"
#include <stdint.h>
#include "saturntypes.h"
#include "OutHighPriority.h"
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>




// this runs as its own thread to send outgoing data
// thread initiated after a "Start" command
// will be instructed to stop & exit by main loop setting enable_thread to 0
// this code signals thread terminated by setting active_thread = 0
//
void *OutgoingHighPriority(void *arg)
{
//
// variables for outgoing UDP frame
//
  struct iovec iovecinst;                                 // instance of iovec
  struct msghdr datagram;
  uint8_t UDPBuffer[VHIGHPRIOTIYFROMSDRSIZE];             // DDC frame buffer
  uint32_t SequenceCounter = 0;                           // UDP sequence count

  struct ThreadSocketData *ThreadData;            // socket etc data for this thread
  struct sockaddr_in DestAddr;                    // destination address for outgoing data
  bool InitError = false;
  int Error;

//
// initialise. Create memory buffers and open DMA file devices
//
  ThreadData = (struct ThreadSocketData *)arg;
  ThreadData->Active = true;
  printf("spinning up outgoing high priority with port %d\n", ThreadData->Portid);



  while(!(ThreadData->Cmdid & VBITDATARUN))
  {
    usleep(100);
  }
  printf("starting outgoing high priority data\n");
  //
  // initialise outgoing data packet
  //
  memcpy(&DestAddr, &reply_addr, sizeof(struct sockaddr_in));           // local copy of PC destination address
  memset(&iovecinst, 0, sizeof(struct iovec));
  memset(&datagram, 0, sizeof(datagram));
  iovecinst.iov_base = UDPBuffer;
  iovecinst.iov_len = VHIGHPRIOTIYFROMSDRSIZE;
  datagram.msg_iov = &iovecinst;
  datagram.msg_iovlen = 1;
  datagram.msg_name = &DestAddr;                   // MAC addr & port to send to
  datagram.msg_namelen = sizeof(DestAddr);

  while(!InitError)                               // main loop
  {
      // send a dummy high priority packet with zero data
      memset(UDPBuffer, 0,sizeof(UDPBuffer));                      // clear the whole packet
      *(uint32_t *)UDPBuffer = htonl(SequenceCounter++);        // add sequence count
      Error = sendmsg(ThreadData -> Socketid, &datagram, 0);

      if(Error == -1)
      {
        printf("High Priority Send Error, errno=%d\n", errno);
        printf("socket id = %d\n", ThreadData -> Socketid);
        InitError=true;
      }
  }
//
// tidy shutdown of the thread
//
  printf("shutting down outgoing mic thread\n");
  ThreadData->Active = false;                   // signal closed
  return NULL;
}

