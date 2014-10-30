/****************************************************************************
 *      $Id: debugger.c,v 1.37 1999/10/01 05:08:22 alanau Exp $
 *      Copyright (C) 1997, 1998 Kevin Elphinstone, Univeristy of New South
 *      Wales.
 *
 *      This file is part of the L4/MIPS micro-kernel distribution.
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version 2
 *      of the License, or (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *      
 ****************************************************************************/
#include <kernel/kernel.h>
#include <kernel/kutils.h>
#include <kernel/vm.h>
#include <l4/sigma0.h>
#include <alloca.h>
#include <r4kc0.h>

#define ENTER 13
#define BACKSPACE 8
#define DELETE 127
#define CONTROL_R 18
#define CONTROL_U 21
#define CONTROL_C 3
#define CONTROL_D 4
#define MAXLINE 80
#define OUTBUFFSIZE 256


/* expected extern funcs for serial i/o */
extern void reset_serial_io(void);
extern int inch(void);
extern void outch(int);

/* some internal prototypes */
void do_command(char *);
void printbuff(void);

/* prototypes for various command functions */
void ct_comm(int argc, char *argv[]); 
void pm_comm(int argc, char *argv[]); 
void pt_comm(int argc, char *argv[]); /* print tcb command */
void pr_comm(int argc, char *argv[]); /* print register command */
void pk_comm(int argc, char *argv[]); /* print kernel variables command */
void pc_comm(int argc, char *argv[]); /* print kernel variables command */
void bl_comm(int argc, char *argv[]); /* print busy list */

void pgpt_comm(int argc, char *argv[]);
void bon_comm(int argc, char *argv[]);
void rbt_comm(int argc, char *argv[]);
void help_comm(int argc, char *argv[]);
void version_comm(int argc, char *argv[]);

struct com_list {
  char *com;
  void (*com_func)(int argc, char *argv[]);
  char *desc;
} commands[] = {
  {"bl", bl_comm, " print and check busy list"},
  {"ct", ct_comm, " address: change current tcb base to address"},
  {"pm", pm_comm, " [number] address: print memory at address"},
  {"pt", pt_comm, ": print tcb variables in current tcb"},
  {"pr", pr_comm, " [register]: print register[s]"},
  {"pk", pk_comm, ": print kernel variables"},
  {"pc", pc_comm, ": print coprocessor values"},
  {"rbt", rbt_comm, ": reboot the board"},
  {"pgpt", pgpt_comm, ": print gpt tree"},
  {"bon", bon_comm, ": switch conditional break points on"},
  {"version", version_comm, ": print kernel version and build number"},
  {"?", help_comm, ": help command"},
  {0,0,0}};

char outbuff[OUTBUFFSIZE];
char line[MAXLINE];
long regset[31];
tcb_t *tb;
kernel_vars *k;


char *reg_names[] = { "hi", "lo",
                      "ra", "s8", "gp", "t9", "t8", "s7", "s6",
		      "s5", "s4", "s3", "s2", "s1", "s0", "t3",
		      "t2", "t1", "t0", "a7", "a6", "a5", "a4",
		      "a3", "a2", "a1", "a0", "v1", "v0", "AT",
		      "sp", "k0", "k1", 0};

/* protos for getting and printing c0 */
extern void get_bva(udw_t *);
extern void get_epc(udw_t *);
extern void get_ehi(udw_t *);
extern void get_prid(udw_t *);
extern void get_tlb(udw_t *);
extern void get_xc(udw_t *);
extern void get_st(udw_t *);
extern void get_cs(udw_t *);
extern void get_cmp(udw_t *);


void print_bva(udw_t *);
void print_epc(udw_t *);
void print_ehi(udw_t *);
void print_prid(udw_t *);
void print_tlb(udw_t *);
void print_xc(udw_t *);
void print_st(udw_t *);
void print_cs(udw_t *);
void print_cmp(udw_t *);

struct c0_r {
  char *name;
  void (*get_func)(udw_t *);
  void (*print_func)(udw_t *);
} c0_reg [] = {{"bva", get_bva, print_bva},
	       {"epc", get_epc, print_epc}, 
	       {"ehi", get_ehi, print_ehi}, 
	       {"prid", get_prid, print_prid}, 
	       {"tlb", get_tlb, print_tlb},
	       {"xc", get_xc, print_xc},
	       {"st", get_st, print_st},
	       {"cs", get_cs, print_cs},
	       {"cmp", get_cmp, print_cmp},
	       {0,0}};

struct fs {
  int state;
  char *name;
} fs_names[] = {{FS_WAIT, "WAIT"},
		{FS_BUSY, "BUSY"},
		{FS_WAKEUP, "WAKEUP"},
		{FS_POLL, "POLL"},
		{FS_LOCKS, "LOCKS"},
		{FS_LOCKR, "LOCKR"},
		{FS_INACTIVE, "INACTIVE"},
		{FS_DYING, "DYING"},
		{0,0}};

void debug(char *msg)
{
  int c;
  int i;
  int lineptr;

  /* print msg on why called */
  sprintf(outbuff, "L4 KERNEL DEBUGGER: %s\n\r", msg);
  printbuff();

  k = KERNEL_BASE;
  tb = (tcb_t *)((k->stack_bottom -1) & (~(unsigned long)TCBO));
  sprintf(outbuff,"TCB BASE: 0x%llx\r\n", tb);
  printbuff();
  
  

  /* implement a basic command line with usual key mappings */

  outch('K');
  outch('D');
  outch('B');
  outch('G');
  outch('>');
  outch(' ');

  lineptr = 0;
  while (1)
  {
    c = inch();
    if (iscntrl(c))
    {
      switch(c)
      {
      case CONTROL_D:
	return;
      case CONTROL_C:
	/* ignore exit(0); */
	break;
      case BACKSPACE:
      case DELETE:
	if (lineptr > 0)
	{
	  lineptr--;
	  outch('\b');
	  outch(' ');
	  outch('\b');
	}
	break;
      case CONTROL_U:
	while (lineptr > 0)
	{
	  lineptr--;
	  outch('\b');
	  outch(' ');
	  outch('\b');
	}
	break;
      case CONTROL_R:
	outch('\n');
	outch('\r');
	i = 0;
	while(i < lineptr)
	{
	  outch(line[i]);
	  i++;
	}
	break;
      case ENTER:
	outch('\n');
	outch('\r');
	line[lineptr] = 0;
	do_command(line);
	lineptr = 0;
	outch('K');
	outch('D');
	outch('B');
	outch('G');
	outch('>');
	outch(' ');
	break;
      default:
	/* do nothing */
	/* printf("%d\n",c);*/
	break;
      }
    }
    else
    {
      if (lineptr < (MAXLINE - 1))
      {
	outch(c);
	line[lineptr] = (char) c;
	lineptr++;
      }
    }
  }
}

#define MAXARGS 5

void do_command(char *line)
{
  int i, in_space, argc;
  char *argv[MAXARGS];
    
  /* split the command input buff up into argc,argv style
   */

  i = 0;
  in_space = 1;
  argc = 0;
  while (line[i] != 0)
  {
    if (isspace(line[i]))
    {
      if (!in_space)
      {
	in_space = 1;
	line[i] = 0;
      }
    }
    else
    {
      if (in_space)
      {
	in_space = 0;
	if (argc < MAXARGS)
	{
	  argv[argc] = &line[i];
	  argc++;
	}
      }
    }
    i++;
  }

  if (argc == 0)
  {
    return;
  }

  /* try to match input with a valid command, and call function
   * on a match
   */
  i = 0;
  while(commands[i].com != 0)
  {
    if (strcmp(commands[i].com, argv[0]) == 0)
    {
      commands[i].com_func(argc, argv);
      return;
    }
    i++;
  }
  sprintf(outbuff,"Error: %s: unknown command\n\r", argv[0]);
  printbuff();
}

void printbuff(void)
{
  int i;
  i = 0;
  while (outbuff[i] != 0 && i < OUTBUFFSIZE)
  {
    outch(outbuff[i]);
    i++;
  }
}

/* for testing debugger under unix

   serial_init()
{
  int r;
  struct termios new;


  r = tcgetattr(0, &new);

  assert(r >= 0);

  new.c_lflag = 0;
  new.c_cc[VEOF] = 1;
  new.c_cc[VEOL] = 1;

  r = tcsetattr(0, TCSANOW,&new);


  assert(r>=0);
}
*/

void pt_comm(int argc, char *argv[]) /* print tcb command */
{
  int i;
  if (argc != 1)
  {
    sprintf(outbuff, "%s: syntax error\n\r", argv[0]);
    printbuff();
    return;
  }
  sprintf(outbuff, "TCB: 0x%llx\r\n", tb);
  printbuff();
  sprintf(outbuff,"stack pointer: 0x%llx\r\n",tb->stack_pointer);
  printbuff();
  sprintf(outbuff,"soon wakeup link: 0x%llx\r\n",tb->soon_wakeup_link);
  printbuff();
  sprintf(outbuff,"wake up link: 0x%llx\r\n",tb->wakeup_link);
  printbuff();
  sprintf(outbuff,"late wakeup link: 0x%llx\r\n",tb->late_wakeup_link);
  printbuff();
  sprintf(outbuff,"wake up time: 0x%llx\r\n",tb->wakeup);
  printbuff();
  sprintf(outbuff,"busy link: 0x%llx\r\n",tb->busy_link);
  printbuff();
  sprintf(outbuff,"interrupted link: 0x%llx\r\n", tb->int_link);
  printbuff();
  sprintf(outbuff,"remaining time slice: 0x%llx\r\n",tb->rem_timeslice);
  printbuff();
  sprintf(outbuff,"time slice: 0x%llx\r\n",tb->timeslice);
  printbuff();
  sprintf(outbuff,"mcp: 0x%llx\r\n",tb->mcp);
  printbuff();
  sprintf(outbuff,"tsp: 0x%llx\r\n",tb->tsp);
  printbuff();
  sprintf(outbuff,"ctsp: 0x%llx\r\n",tb->ctsp);
  printbuff();
  sprintf(outbuff,"send queue start: 0x%llx\r\n",tb->sndq_start);
  printbuff();
  sprintf(outbuff,"send queue end: 0x%llx\r\n",tb->sndq_end);
  printbuff();
  sprintf(outbuff,"send queue prev: 0x%llx\r\n",tb->sndq_prev);
  printbuff();
  sprintf(outbuff,"send queue next: 0x%llx\r\n",tb->sndq_next);
  printbuff();
  sprintf(outbuff,"my thread id: 0x%llx\r\n",tb->myself);
  printbuff();

  sprintf(outbuff,"fine state:");
  printbuff();
  i = 0;
  while(fs_names[i].state != 0)
  {
    if (tb->fine_state & fs_names[i].state)
    {
      sprintf(outbuff," %s",fs_names[i].name);
      printbuff();
    }
    i++;
  }
  
  sprintf(outbuff,"\r\ncoarse state: 0x%lx\r\n",tb->coarse_state);
  printbuff();
  sprintf(outbuff,"present list next: 0x%llx\r\n",tb->present_next);
  printbuff();
  sprintf(outbuff,"waiting for: 0x%llx\r\n",tb->wfor);
  printbuff();
  sprintf(outbuff,"receive descriptor: 0x%llx\r\n",tb->recv_desc);
  printbuff();
  sprintf(outbuff,"thread ASID: 0x%llx\r\n",tb->asid);
  printbuff();
  sprintf(outbuff,"gpt pointer: 0x%llx\r\n",tb->gpt_pointer);
  printbuff();
  sprintf(outbuff,"pager tid: 0x%llx\r\n",tb->pager_tid);
  printbuff();
  sprintf(outbuff,"exception tid: 0x%llx\r\n",tb->excpt_tid);
  printbuff();
  sprintf(outbuff,"comm partner: 0x%llx\r\n",tb->comm_partner);
  printbuff();
  sprintf(outbuff,"wdw map addr: 0x%llx\r\n",tb->wdw_map_addr);
  printbuff();
  sprintf(outbuff,"interrupt mask: 0x%llx\r\n",tb->interrupt_mask);
  printbuff();
  sprintf(outbuff,"stacked fine state:");
  printbuff();
  i = 0;
  while(fs_names[i].state != 0)
  {
    if (tb->stacked_fine_state & fs_names[i].state)
    {
      sprintf(outbuff," %s",fs_names[i].name);
      printbuff();
    }
    i++;
  }
  sprintf(outbuff,"\r\nstacked_comm_prtnr: 0x%llx\r\n",tb->stacked_comm_prtnr);
  printbuff();
  sprintf(outbuff,"timeout: 0x%lx\r\n",tb->timeout);
  printbuff();
  sprintf(outbuff,"sister_task: 0x%llx\r\n",tb->sister_task);
  printbuff();
  sprintf(outbuff,"sister_task_prev: 0x%llx\r\n",tb->sister_task_prev);
  printbuff();
  sprintf(outbuff,"child task: 0x%llx\r\n",tb->child_task);
  printbuff();
  sprintf(outbuff,"pt size: %lld\r\n",tb->pt_size);
  printbuff();
  sprintf(outbuff,"pt number: %lld\r\n",tb->pt_number);
  printbuff();
   sprintf(outbuff,"cpu time: %lld\r\n",tb->cpu_time);
  printbuff();
}

void pk_comm(int argc, char *argv[])
{
  
  if (argc != 1)
  {
    sprintf(outbuff, "%s: syntax error\n\r", argv[0]);
    printbuff();
    return;
  }

  sprintf(outbuff,"kernel stack bottom: 0x%llx\r\n",k->stack_bottom);
  printbuff();
  sprintf(outbuff,"soon wake up list: 0x%llx\r\n",k->soon_wakeup_list);
  printbuff();
  sprintf(outbuff,"wake up list 0x%llx\r\n", k->wakeup_list);
  printbuff();
  sprintf(outbuff,"late wake up list: 0x%llx\r\n", k->late_wakeup_list);
  printbuff();
  sprintf(outbuff,"present list: 0x%llx\r\n", k->present_list);
  printbuff();
  sprintf(outbuff,"interrupted list: 0x%llx\r\n", k->int_list);
  printbuff();
  sprintf(outbuff,"remaining time slice: 0x%llx\r\n",k->timeslice);
  printbuff();
  sprintf(outbuff,"current priority: 0x%llx\r\n",k->priority);
  printbuff();
  sprintf(outbuff,"clock: 0x%llx\r\n", k->clock);
  printbuff();
  sprintf(outbuff,"memory_size: 0x%llx\r\n", k->memory_size);
  printbuff();
  sprintf(outbuff,"frame_table_base: 0x%llx\r\n", k->frame_table_base);
  printbuff();
  sprintf(outbuff,"frame_table_size: 0x%llx\r\n", k->frame_table_size);
  printbuff();
  sprintf(outbuff,"frame_table_pointer: 0x%llx\r\n", k->frame_table_pointer);
  printbuff();
  sprintf(outbuff,"gpt_pointer: 0x%llx\r\n", k->gpt_pointer);
  printbuff();
  sprintf(outbuff,"asid free list: 0x%llx\r\n", k->free_asid_list);
  printbuff();
  sprintf(outbuff,"asid fifo count: %lld\r\n", k->asid_fifo_count);
  printbuff();
  sprintf(outbuff,"tcb_gpt_guard: 0x%llx\r\n", k->tcb_gpt_guard);
  printbuff();
  sprintf(outbuff,"tcb_gpt_pointer: 0x%llx\r\n", k->tcb_gpt_pointer);
  printbuff();
  sprintf(outbuff,"s0_save: 0x%llx\r\n", k->s0_save);
  printbuff();
  sprintf(outbuff,"s1_save: 0x%llx\r\n", k->s1_save);
  printbuff();
  sprintf(outbuff,"s2_save: 0x%llx\r\n", k->s2_save);
  printbuff();
  sprintf(outbuff,"s3_save: 0x%llx\r\n", k->s3_save);
  printbuff();
  sprintf(outbuff,"s4_save: 0x%llx\r\n", k->s4_save);
  printbuff();
  sprintf(outbuff,"trace_reg_save: 0x%llx\r\n", k->trace_reg_save);
  printbuff();
  sprintf(outbuff,"tlb_t0_save: 0x%llx\r\n", k->tlb_t0_save);
  printbuff();
  sprintf(outbuff,"tlb_t1_save: 0x%llx\r\n", k->tlb_t1_save);
  printbuff();
  sprintf(outbuff,"tlb_t2_save: 0x%llx\r\n", k->tlb_t2_save);
  printbuff();
  sprintf(outbuff,"tlb_t3_save: 0x%llx\r\n", k->tlb_t3_save);
  printbuff();
  sprintf(outbuff,"tlb_t8_save: 0x%llx\r\n", k->tlb_t8_save);
  printbuff();
  sprintf(outbuff,"profile_addr: 0x%llx\r\n", k->profile_addr);
  printbuff();
  sprintf(outbuff,"frame_list: 0x%llx\r\n", k->frame_list);
  printbuff();
  sprintf(outbuff,"gpt_free_list: 0x%llx\r\n", k->gpt_free_list);
  printbuff();
  sprintf(outbuff,"mt_free_list: 0x%llx\r\n", k->mt_free_list);
  printbuff();
  sprintf(outbuff,"int0_thread: 0x%llx\r\n", k->int0_thread);
  printbuff();
  sprintf(outbuff,"int1_thread: 0x%llx\r\n", k->int1_thread);
  printbuff();
  sprintf(outbuff,"int2_thread: 0x%llx\r\n", k->int2_thread);
  printbuff();
  sprintf(outbuff,"int3_thread: 0x%llx\r\n", k->int3_thread);
  printbuff();
  sprintf(outbuff,"int4_thread: 0x%llx\r\n", k->int4_thread);
  printbuff();
  sprintf(outbuff,"sigz_tcb: 0x%llx\r\n", k->sigz_tcb);
  printbuff();
  sprintf(outbuff,"fp_thread: 0x%llx\r\n", k->fp_thread);
  printbuff();
  sprintf(outbuff,"tlb_miss: %lld\r\n", k->tlb_miss);
  printbuff();
  sprintf(outbuff,"tlb_miss_time: %lld\r\n", k->tlb_miss_time);
  printbuff();
  sprintf(outbuff,"tlb2_miss: %lld\r\n", k->tlb2_miss);
  printbuff();
}

void pr_comm(int argc, char *argv[])
{
  int i;
  if (argc == 1)
  {
    /* print all registers */
    i = 0;
    while (reg_names[i] != 0)
    {
      sprintf(outbuff, "%s: 0x%llx\r\n", reg_names[i],regset[i]);
      printbuff();
      i++;
    }
  }
  else if (argc == 2)
  {
    i = 0;
    while(reg_names[i] != 0)
    {
      if (strcmp(reg_names[i],argv[1]) == 0)
      {
	sprintf(outbuff, "%s: 0x%llx\r\n",reg_names[i],regset[i]);
	printbuff();
	return;
      }
      i++;
    }
    sprintf(outbuff, "%s: %s: unknown register name\n\r", argv[0],argv[1]);
    printbuff();
  }
  else
  {
    sprintf(outbuff,"%s: too many args\r\n",argv[0]);
    printbuff();
  }
}

udw_t c0_space[96];

void pc_comm(int argc, char *argv[])
{
  int i;
  if (argc == 2)
  {
    i = 0;
    while(c0_reg[i].name != 0)
    {
      if (strcmp(c0_reg[i].name, argv[1]) == 0)
      {
	(c0_reg[i].get_func)(c0_space);
	(c0_reg[i].print_func)(c0_space);
	return;
      }
      i++;
    }
    sprintf(outbuff, "%s: %s: unknown register name\n\r", argv[0],argv[1]);
    printbuff();
  }
  else
  {
    sprintf(outbuff,"%s: too many args\r\n",argv[0]);
    printbuff();
  }
}

void help_comm(int argc, char *argv[])
{
  int i;
  if (argc != 1)
  {
    sprintf(outbuff,"%s: syntax error\r\n");
    printbuff();
  }

  i = 0;
  while(commands[i].com != 0)
  {
    sprintf(outbuff,"%s%s\r\n", commands[i].com, commands[i].desc);
    printbuff();
    i++;
  }
}

void ct_comm(int argc, char *argv[])
{
  unsigned long address;
  int r;
  
  if (argc != 2)
  {
    sprintf(outbuff,"%s: incorrect args\r\n",argv[0]);
    printbuff();
    return;
  }

  r = llatob(&address, argv[1], 0);
  if (r == 1)
  {
    tb = (tcb_t *)(address & (~(unsigned long)TCBO));
    sprintf(outbuff, "tcb = 0x%llx\r\n", tb);
    printbuff();
  }
}

void pm_comm(int argc, char *argv[])
{
  unsigned long address;
  unsigned long number;
  
  int r;
  
  if (argc == 2)
  {
      r = llatob(&address, argv[1], 0);
      if (r == 1)
      {
	sprintf(outbuff, "0x%llx = 0x%llx\r\n", address,
		*((unsigned long *)address));
	printbuff();
	return;
      }
  }
  else if (argc == 3)
  {
    r = llatob(&number, argv[1], 0);
    if (r == 1)
    {
      r = llatob(&address, argv[2], 0);
      if (r == 1)
      {
	unsigned long i;
	number *= 8;
	for (i = 0; i < number;i += 8)
	{
	  sprintf(outbuff, "0x%llx = 0x%llx\r\n", address+i,
		  *((unsigned long *)(address+i)));
	  printbuff();
	}
	return;
      }
    }
  }
  sprintf(outbuff,"%s: incorrect args\r\n",argv[0]);
  printbuff();
  return;
}


void print_bva(udw_t *v)
{
  sprintf(outbuff, "bva = 0x%llx\r\n", *v);
  printbuff();
}

    
void print_epc(udw_t *v)
{
  sprintf(outbuff, "epc = 0x%llx\r\n", *v);
  printbuff();
}

void print_ehi(udw_t *v)
{
  sprintf(outbuff, "ehi = 0x%llx\r\n", *v);
  printbuff();
}

void print_cmp(udw_t *v)
{
  sprintf(outbuff, "cmp = 0x%llx\r\n", *v);
  printbuff();
}

void print_prid(udw_t *v)
{
  sprintf(outbuff, "prid imp = 0x%x rev = 0x%x\r\n", (int) *v >> 8,
	  (int) *v & 0xff);
  printbuff();
}

void print_xc(udw_t *v)
{
  sprintf(outbuff, "vpn2 = 0x%llx r = 0x%llx\r\n", (*v & 0x3fffffff) << 9,
	  *v >> 31 & 0x3);
  printbuff();
}

void print_cs(udw_t *v)
{
  sprintf(outbuff, "exc=%d ip=%x ce=%d bd=%d\r\n",
	  *v >> 2 & 037,
	  *v >> 8 & 0xff,
	  *v >> 28 & 3,
	  *v >> 31 & 1);
  printbuff();
}

void print_st(udw_t *v)
{
  sprintf(outbuff, "ie=%d exl=%d erl=%d ksu=%d ux=%d sx=%d kx=%d\r\n",
	  *v & 1,
	  *v >> 1 & 1,
	  *v >> 2 & 1,
	  *v >> 3 & 3,
	  *v >> 5 & 1,
	  *v >> 6 & 1,
	  *v >> 7 & 1);
  printbuff();
  sprintf(outbuff,"im=%x de=%d ce=%d ch=%d sr=%d bev=%d re=%d fr=%d cu=%x\r\n",
	  *v >> 8 & 0xff,
	  *v >> 16 & 1,
	  *v >> 17 & 1,
	  *v >> 18 & 1,
	  *v >> 20 & 1,
	  *v >> 22 & 1,
	  *v >> 25 & 1,
	  *v >> 26 & 1,
	  *v >> 28 & 0xf);
  printbuff();
}

void print_tlb(udw_t *v)
{
  int i;
  for (i = 0; i < 48; i++)
  {
    sprintf(outbuff, "%d: hi = 0x%llx ",  48 - i, v[2*i]);
    printbuff();
    sprintf(outbuff, "lo0 = 0x%lx d0 = %d v0 = %d g0 = %d lo1 = 0x%lx d1 = %d v1 = %d g1 = %d\r\n",
	    (v[2*i+1] & 0xffffffff) >> 6 << 12,
	    v[2*i+1] >> 2 & 01,
	    v[2*i+1] >> 1 & 01,
	    v[2*i+1] & 01,
	    (v[2*i+1] & 0xffffffff00000000) >> 38 << 12,
	    v[2*i+1] >> 34 & 01,
	    v[2*i+1] >> 33 & 01,
	    v[2*i+1] >> 32 & 01);
    printbuff();
  }
}

w_t is_leaf(udw_t guard,
	    udw_t level,
	    udw_t size,
	    udw_t u)
{
  return (
	  (~u & ((1ul << size) - 1ul)) 
	  == 
	  (
	   (guard >> (level - size)) & 
	   ((1ul << size) - 1ul)
	   )
	  ) ||
    (
     ((1ul << size) + u)
     ==
     ((guard >> (level - size)) & ((1ul << (size + 1)) - 1ul))
     );
  
}



void pgpt_comm(int argc, char *argv[])
{
  udw_t address, size;
  int r;
  
  if (argc == 1)
  {
    vm_print_fpage(tb->gpt_pointer, 0, 64);
  }
  else if (argc == 3)
  {
    r = llatob(&address, argv[1], 0);
    if (r != 1)
    {
      return;
    }
    r = llatob(&size, argv[2], 0);
    if (r != 1)
    {
      return;
    }
    sprintf(outbuff,"addr = 0x%llx, size = %d\r\n", address, size);
    printbuff();
    vm_print_fpage(tb->gpt_pointer, address, size);
  }
  else
  {
    sprintf(outbuff, "%s: syntax error\n\r", argv[0]);
    printbuff();
    return;
  }
  return;
}

void bl_comm(int argc, char *argv[]) /* print busy list */
{
  kernel_vars *k;
  tcb_t *p;
  short int i;

  k = KERNEL_BASE;

  for (i = 0; i <= MAX_PRIORITY; i++)
  {  
    p = k->prio_busy_list[i];

   
    if (p == 0)  /* this priority has empty busy list */
      continue;
    else
    {
       sprintf(outbuff, "Priority %d\n\r", i);
       printbuff();
    }

    do {
      if (p == 0)
      {
	sprintf(outbuff, "Error: circular busy list broken at priority %d\n\r", i);
	printbuff();
	return;
      }
      else
      {
	sprintf(outbuff, "\ttcb = 0x%llx\n\r", p);
	printbuff();
	p = p->busy_link;
      }
    } while (p != k->prio_busy_list[i]);
  }
  return;
}
void version_comm(int argc, char *argv[])
{
  l4_kernel_info *ki;
  ki = (l4_kernel_info *) PHYS_TO_CKSEG1(KERNEL_INFO_PAGE);
  sprintf(outbuff, "L4 version %d build %d\n\r",
	  ki->version, ki->build);
  printbuff();
  return;
}

     
void bon_comm(int argc, char *argv[]) /* print busy list */
{
  kernel_vars *k;
  
  k = KERNEL_BASE;

  k->break_on = 1;
}
