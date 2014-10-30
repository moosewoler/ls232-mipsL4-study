/****************************************************************************
 * $Id: ipc_test_client.c,v 1.19 1999/10/23 07:11:36 gernot Exp $
 * Copyright (C) 1997, 1998 Kevin Elphinstone, Univeristy of New South
 * Wales.
 *
 * This file is part of the L4/MIPS micro-kernel distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ****************************************************************************/
#include <libc.h>
#include <assert.h>
#include <l4/ipc.h>
#include <l4/syscalls.h>
#include <l4/sigma0.h>
#ifdef INDY
#include <kernel/indy.h>
#elif defined(P4000)
#include "../drivers/sonic.h"
#endif


#define STACK_SIZE 512
unsigned long _stack[STACK_SIZE];
unsigned long *_sp = &_stack[STACK_SIZE -1];


static const l4_threadid_t SERIAL_TID = {0x1002000000060001};
#ifdef P4000
static const l4_threadid_t SONIC_TID =  {0x1002000000080001};
#endif
static const l4_threadid_t SERVER_TID = {0x1002000000080001};
static const l4_threadid_t MEM_TID =    {0x1002000000080401};
static const l4_threadid_t NEW_TASK_TID =    {0x0200000};
static const l4_threadid_t NIL_TID =    {0x000000};

void ping_thread(void);

void ex_reg_loop(void);

extern void timestamp_init(void);
extern unsigned int timestamp(void);
unsigned int  time_start[1000], time_end[1000];
l4_threadid_t thrd0_tid;

dword_t thread_1_stack[128];
dword_t thread_8_stack[128];
void thread_1(void)
{
  int r;
  l4_threadid_t tid;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  
  r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG,
		       &msg,  L4_IPC_NEVER, &result);
}

#ifdef INDY
#define INSTR_PER_T            100
#elif defined(P4000)
#define INSTR_PER_T            20
#elif defined(U4600)
#define INSTR_PER_T            2
#endif


dword_t mem_stack[128];
void mem_thread(void)
{
  int *p;
  int *pb, *pe;
  char *c;
  int r,i,j;
  l4_threadid_t tid;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  dword_t dummy;
  unsigned time;
  l4_fpage_t fpage;
  c = (char *) &msg.reg[0];

  

  /***********************************************************************/
  sprintf(c, "touching 1000 pages\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  for (i = 0; i < 1000; i++)
  {
    time_start[i] = timestamp();
    dummy = *(volatile unsigned long *) (8ul * 1024 * 1024 + i *4096);
    time_end[i] = timestamp();
  }
  for (j = 0; j < 10; j++)
  {
    time = 0;
    for (i = 0; i < 100; i++)
    {
      /* sprintf(c,"page fault test = %d, time = %lu\r\n",i + 10 * j, 
	      ((time_start[i+10*j] - time_end[i+10*j]) * INSTR_PER_T )
	      );
      
      r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
			   L4_IPC_NEVER, &result); */
      
      time += ((time_start[i+10*j] - time_end[i+10*j]) * INSTR_PER_T );
      
    }
    sprintf(c,"page fault test %d avg 100 faults, time = %u\r\n", j,
	    time / 100);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
			 L4_IPC_NEVER, &result);
    

  }


  /**********************************************************************/

  sprintf(c,"tlb exerciser\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);



  pb = (int *)(8ul * 1024 * 1024);

  pe = (int *) (((long) pb) + (96 * 4096));
  
  for (p = pb; p < pe;  p = (int *)((long)p + 4096))
  {
    dummy = *(volatile int *) p;
  }
  p = pb;
  for (j = 0; j < 1; j++)
  {
    time_start[0] = timestamp();
    for (i = 0; i < 1000; i++)
    {
      dummy = *(volatile int *) p;
      
      p = (int *)((long)p + 4096);
      if (p == pe) p = pb;
    }
    time_end[0] = timestamp();
    sprintf(c,"tlb time = %d\r\n",
	    (time_start[0] -  time_end[0]) * INSTR_PER_T / 100);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
			 L4_IPC_NEVER, &result);
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  }
  
  
  /**********************************************************************/
  sprintf(c,"unmap/page fault test starting\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  fpage = l4_fpage(8ul * 1024 * 1024,  L4_LOG2_PAGESIZE, 0, 0);
  time_start[0] = timestamp();
  for (i = 0; i < 1000; i++)
  {
    l4_fpage_unmap(fpage,  L4_FP_FLUSH_PAGE |  L4_FP_ALL_SPACES);
    dummy = *(volatile unsigned long *) (8ul * 1024 * 1024 );
  }
  time_end[0] = timestamp();
  
  sprintf(c,"unmap/page fault time = %d\r\n",
	  (time_start[0] -  time_end[0]) * INSTR_PER_T / 1000);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);

  /***********************************************************************/
  sprintf(c,"unmapping 1000 pages starting\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  
  time_start[0] = timestamp();
  for (i = 0; i < 1000; i++)
  {
    fpage = l4_fpage(8ul * 1024 * 1024 + i * 4096,  L4_LOG2_PAGESIZE, 0, 0);
    time_start[i] = timestamp();
    l4_fpage_unmap(fpage,  L4_FP_FLUSH_PAGE |  L4_FP_ALL_SPACES);
    time_end[i] = timestamp();
  }

  for (j = 0; j < 10; j++)
  {
    time = 0;
    for (i = 0; i < 100; i++)
    {
      time += ((time_start[i+10*j] - time_end[i+10*j]) * INSTR_PER_T );
      
    }
    sprintf(c,"unmap page test %d avg 100 faults, time = %u\r\n", j,
	    time / 100);
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
			 L4_IPC_NEVER, &result);
  }

  /***********************************************************************/

  sprintf(c,"doing unmap 1000 pages in single call\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);

  /* ensure there mapped */
  pb = (int *)(8ul * 1024 * 1024);
  
  pe = (int *) (((long) pb) + (1000 * 4096));
  
  for (p = pb; p < pe;  p = (int *)((long)p + 4096))
  {
    dummy = *(volatile int *) p;
  }

  fpage = l4_fpage(8ul * 1024 * 1024 + i * 4096,  22, 0, 0);
  time_start[0] = timestamp();
  l4_fpage_unmap(fpage,  L4_FP_FLUSH_PAGE |  L4_FP_ALL_SPACES);
  time_end[0] = timestamp();

  sprintf(c,"unmap time (avg per mapped page) = %d\r\n",
	  (time_start[0] -  time_end[0]) * INSTR_PER_T / 1000);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
  
  r = l4_mips_ipc_reply_and_wait(thrd0_tid,
				 L4_IPC_SHORT_MSG,
				 &msg,
				 &tid,
				 L4_IPC_SHORT_MSG,
				 &msg,
				 L4_IPC_NEVER, &result);
}

#ifdef P4000
struct sonic_reg *sonic;

unsigned int timestamp(void)
{
  return (sonic->wt0 | (sonic->wt1 << 16));
}

#endif


void local_server_loop(l4_threadid_t);

void ipc_local_server(void)
{
  int r,i;
  l4_msgdope_t result;
  unsigned long dummy;
  l4_threadid_t tid;
  char *c;
  l4_ipc_reg_msg_t msg;
  
  tid = l4_myself();

  c = (char *) &msg.reg[0];
  sprintf(c, "IPC local server, my thread id is 0x%lx\r\n",tid.ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG,
		       &msg,  L4_IPC_NEVER, &result);
  assert(r == 0);

  local_server_loop(tid);
}

void client_loop();
void id_loop();

void main()
{
  l4_fpage_t fpage;
  int r,i;
  l4_msgdope_t result;
  l4_ipc_reg_msg_t msg;
  char *c;
  l4_threadid_t tid, page, pre;
  long count;
  unsigned long dummy;

  /* ensure various data is mapped */
  for (i = 0; i < 1000;i++)
  {
    time_start[i] = 0;
    time_end[i] = 0;
  }
  for (i = 0; i < 128; i++)
  {
    mem_stack[i] = 0;
    thread_1_stack[i] = 0;
  }
  
  tid = thrd0_tid = l4_myself();
  count = 0;
  c = (char *) &msg.reg[0];

  sprintf(c, "Universal performance tester, thread id is 0x%lx\n",tid.ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
#ifdef INDY
  
  msg.reg[0] = SIGMA0_DEV_MAP;
  msg.reg[1] = HPC3_INT_ADDR;
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);


  
#elif defined(P4000)
  /* map the sonic registers */
  msg.reg[0] = SIGMA0_DEV_MAP;
  msg.reg[1] = SONIC_BASE;

  sonic = (struct sonic_reg *) msg.reg[1];
  
  r = l4_mips_ipc_call(SIGMA0_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_MAPMSG(0, L4_WHOLE_ADDRESS_SPACE),
		       &msg, L4_IPC_NEVER, &result);
#elif defined(U4600)
  timestamp_init();
#endif
  

  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  sprintf(c, "IPC test\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  /***********************************************************************/
  /* do ipc tests */

  pre.ID = -1;
  page.ID = -1;
  tid.ID = 8 << 10;
  l4_thread_ex_regs(tid,
		    (dword_t) ipc_local_server,
		    (dword_t) &thread_8_stack[127],
		    &pre,
		    &page,
		    &count,
		    &count);
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);

  time_start[0] = timestamp();

  local_client_loop();

  time_start[1] =  timestamp();

  sprintf(c, "IPC intra-task time test, time = %u\r\n",
	  (((time_start[0] - time_start[1]) * INSTR_PER_T / 1000) - 23) / 2
	  );
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
  assert(r == 0);

  for (i = 0; i < 1; i++) 
  {
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);

    time_start[0] = timestamp();
    
    client_loop();
    
    time_start[1] =  timestamp();
    
    sprintf(c,"IPC inter-task time test %d, time = %u\r\n", i,
	    (((time_start[0] - time_start[1]) * INSTR_PER_T / 1000) - 23) / 2
	    );
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
			 L4_IPC_NEVER, &result);
    assert(r == 0);
  }
  
  /***********************************************************************/
  sprintf(c, "id myself test starting\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  for (i = 0; i < 1; i++)
  {
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);

    time_start[0] = timestamp();
    id_loop();
    
    time_start[1] = timestamp();
    
    sprintf(c,"id myself test %d, time = %u\r\n", i,
	    ((time_start[0] - time_start[1]) * INSTR_PER_T / 1000000) - 7
	    );
    
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
			 L4_IPC_NEVER, &result);
  }

  /***********************************************************************/
  sprintf(c, "id_nearest test starting\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  r = l4_id_nearest(SERIAL_TID, &tid);
  
  sprintf(c, "id serial 0x%lx -> 0x%lx, 0x%x\r\n",
	  *(dword_t *)(&(SERIAL_TID)), *(dword_t *)(&tid), r);
  
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  tid.id.task = 124;
  r = l4_id_nearest(tid, &tid);
  
  sprintf(c, "id serial task 124 -> 0x%lx, 0x%x\r\n",
	  *(dword_t *)(&tid), r);
  
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);

  
  /***********************************************************************/
  /* page fault test from other thread */

  sprintf(c, "page fault test thread starting\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  tid.ID = 2 << 10;
  pre.ID = -1;
  page = MEM_TID;
  l4_thread_ex_regs(tid,
		    (dword_t) mem_thread,
		    (dword_t) &mem_stack[127],
		    &pre,
		    &page,
		    &count,
		    &count);
  
  r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG,
		       &msg,  L4_IPC_NEVER, &result);

  /***********************************************************************/
  /* do ex_reg tests */
  sprintf(c, "ex reg test starting\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  for (i = 0; i < 1; i++)
  {
    r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
    page.ID = -1ul;
    pre.ID = -1ul;
    
    time_start[0] = timestamp();
    ex_reg_loop();
    time_start[1] = timestamp();
    
    sprintf(c,"ex_reg test %d, time = %u\r\n", i,
	    ((time_start[0] - time_start[1]) * INSTR_PER_T / 1000000) - 22
	    );
    
    r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
			 L4_IPC_NEVER, &result);
  }


  /***********************************************************************/
  /* do ex_reg tests */
  sprintf(c, "ex reg ping  test starting\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG, &msg,
		       L4_IPC_NEVER, &result);
  
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  page.ID = -1ul;
  pre.ID = -1ul;
  tid.ID = 4 << 10;
  time_start[0] = timestamp();
  for (i = 0; i < 10000; i++)
  {
    l4_thread_ex_regs(tid, (dword_t)ping_thread,  
			   -1, &pre, &page, &count, &count);
    r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG,
			 &msg,  L4_IPC_NEVER, &result);
  }
  time_end[0] = timestamp();

  sprintf(c,"ex reg: time = %d\r\n",
	   (time_start[0] - time_end[0]) * INSTR_PER_T/ 10000);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
	
 /***********************************************************************/
  sprintf(c,"redirect to chief check\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  tid = NEW_TASK_TID;

  tid = l4_task_new(tid, 0,  
		    0x0007fff8, 0x00080118, MEM_TID, MEM_TID);
  r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG,
		       &msg,  L4_IPC_NEVER, &result);
  
  sprintf(c,"return check: res =  0x%lx, tid = 0x%lx\r\n",
	  result.msgdope, tid.ID);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);

  /***********************************************************************/

  sprintf(c,"deceiving ipc check\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);


  sprintf(c, "my thread id is 0x%lx, vsend 0x%lx\r\n", thrd0_tid.ID, tid.ID);
  r = l4_mips_ipc_send_deceiving(SERIAL_TID,
				 tid,
				 (void *) (L4_IPC_SHORT_MSG |
					   L4_IPC_DECEIT_MASK),
				 &msg,
				 L4_IPC_NEVER,
				 &result);
  
  /***********************************************************************/
  sprintf(c,"create task test starting\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  tid = NEW_TASK_TID;
  for (i = 0; i < 10; i++)
  {
    time_start[i] = timestamp();
    tid = l4_task_new(tid, 0,  
		      0x0007fff8, 0x00080118, MEM_TID, MEM_TID);
    r = l4_mips_ipc_wait(&tid, L4_IPC_SHORT_MSG,
			 &msg,  L4_IPC_NEVER, &result);
    time_end[i] = timestamp();

    time_start[10+i] = timestamp();
    tid = l4_task_new(tid, thrd0_tid.ID, 0, 0, NIL_TID, NIL_TID);
    time_end[10+i] = timestamp();
    tid.id.task ++;
  }

  for (i = 0; i < 10; i++)
  {
   sprintf(c,"task_new: create = %d, delete = %d\r\n",
	   (time_start[i] - time_end[i]) * INSTR_PER_T,
	   (time_start[10+i] - time_end[10+i]) * INSTR_PER_T);
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
  }

  
  /**************************************************************************/
  sprintf(c,"test unmap  fault test starting -- should hang!\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);
  r = l4_mips_ipc_sleep(L4_IPC_TIMEOUT(0,0,1,5,0,0),&result);
  fpage = l4_fpage(8ul * 1024 * 1024,  12, 0, 0);

  l4_fpage_unmap(fpage,  L4_FP_FLUSH_PAGE |  L4_FP_ALL_SPACES);
  dummy = *(volatile unsigned long *) (8ul * 1024 * 1024 );
  
  sprintf(c,"unmap didn't work\r\n");
  r = l4_mips_ipc_send(SERIAL_TID, L4_IPC_SHORT_MSG,  &msg,
		       L4_IPC_NEVER, &result);

  /**************************************************************/
   while(1);
}















