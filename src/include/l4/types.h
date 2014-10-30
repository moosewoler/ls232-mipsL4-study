#ifndef __L4TYPES_H__ 
#define __L4TYPES_H__ 
/****************************************************************************
 * $Id: types.h,v 1.13 1998/12/23 02:04:01 gernot Exp $
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

/****************************************************************************
 * Define the basic types upon which to build other types
 ****************************************************************************/
#if defined(_LANGUAGE_C)

#if _MIPS_SZPTR == 64    /* SGI 64-bit compiler */
typedef unsigned char       byte_t;       /* 8-bit int */
typedef unsigned short int  hword_t;      /* 16-bit int */
typedef unsigned int        word_t;       /* 32-bit int */
typedef unsigned long       dword_t;      /* 64-bit int */
typedef long                cpu_time_t;

#else                   /* gcc or SGI 32-bit compiler */  

typedef unsigned char       byte_t;
typedef unsigned short int  hword_t;
typedef unsigned int        word_t;
typedef unsigned long long  dword_t;
typedef long long           cpu_time_t;

#endif

/* define struct to access upper
   and lower 32-bits of 64-bit int */
typedef struct {
  word_t high, low;
} l4_low_high_t;


/****************************************************************************
 * Structures for accessing L4 thread identifiers
 ****************************************************************************/

/* the basic layout of a tid */
typedef struct {
  unsigned nest:4;
  unsigned chief:11;
  unsigned site:17;
  unsigned version_high:4;
  unsigned task:11;
  unsigned lthread:7;
  unsigned version_low:10;
} l4_threadid_struct_t;

/* the general purpose thread id type giving access as */
typedef union {
  dword_t ID;                        /* 64-bit int */
  l4_low_high_t lh;                  /* two 32-bit ints */  
  l4_threadid_struct_t id;           /* individual fields in struct */
} l4_threadid_t;

typedef l4_threadid_t l4_taskid_t;   /* task id is same as thread id */

/* the layout of an interrupt id */
typedef struct {
  unsigned  _pad:32;
  unsigned  _pad2:29;
  unsigned intr:3;
} l4_intrid_struct_t;

/* the general purpose interrupt id */
typedef union {
  dword_t ID;
  l4_low_high_t lh;
  l4_intrid_struct_t id;
} l4_intrid_t;


/****************************************************************************
 * useful thread id constants, macros and functions
 ****************************************************************************/

#ifdef __GNUC__ /* for gcc */

#if _MIPS_SZPTR == 64

#define L4_NIL_ID               ((l4_threadid_t)0UL)
#define L4_INVALID_ID           ((l4_threadid_t)0xffffffffffffffffUL)
#define l4_is_nil_id(id)        ((id).ID == 0UL)
#define l4_is_invalid_id(id)    ((id).ID == 0xffffffffffffffffUL)

#else /* _MIPS_SZ_PTR == 32 */

#define L4_NIL_ID               ((l4_threadid_t)0ULL)
#define L4_INVALID_ID           ((l4_threadid_t)0xffffffffffffffffULL)
#define l4_is_nil_id(id)        ((id).ID == 0ULL)
#define l4_is_invalid_id(id)    ((id).ID == 0xffffffffffffffffULL)

#endif /* _MIPS_SZPTR == 64 */

#else  /* for SGI 64-bit compiler */

extern const l4_threadid_t _l4_nil_tid;  /* constants defined in libl4.a */
extern const l4_threadid_t _l4_invalid_tid;

#define L4_NIL_ID               _l4_nil_tid
#define L4_INVALID_ID           _l4_invalid_tid
#define l4_is_nil_id(id)        ((id).ID == 0ul)
#define l4_is_invalid_id(id)    ((id).ID == 0xfffffffffffffffful)

#endif  /* for SGI 64-bit compiler */

#define thread_equal(t1, t2) ((t1).ID == (t2).ID)

/* test if two threads are in same task */
extern int task_equal(l4_threadid_t t1, l4_threadid_t t2);

/* get the task id of given thread, ie thread id of lthread 0 */
extern l4_threadid_t get_taskid(l4_threadid_t t);

#else /* for 64-bit assembly */

#define L4_NIL_ID               0
#define L4_INVALID_ID           0xffffffffffffffff

#endif /* for 64-bit assembly */



/****************************************************************************
 * L4 flex pages
 ****************************************************************************/

#if defined(_LANGUAGE_C)

/* layout of an fpage */
typedef struct {
  unsigned pageh:32; /* upper 32-bits */ 
  unsigned page:20;  /* lower 32-bits */
  unsigned zero:3;
  unsigned size:7;
  unsigned write:1;
  unsigned grant:1;
} l4_fpage_struct_t;

/* general purpose fpage type allowing access as */
typedef union {
  dword_t fpage;         /* a 64-bit int */
  l4_fpage_struct_t fp;  /* fields in struct */
} l4_fpage_t;

/* a send page */
typedef struct {
  dword_t snd_base;
  l4_fpage_t fpage;
} l4_snd_fpage_t;

#if (_MIPS_SZPTR == 64)
#define L4_PAGESIZE     (0x1000uL)   /* L4/MIPS page size */
#else /* 32-bit */
#define L4_PAGESIZE     (0x1000uLL)  
#endif /* 32-bit */
#else /* assembler */
#define L4_PAGESIZE     0x1000
#endif /* assembler */

/****************************************************************************
 * useful constants, macros and functions to manipulate fpages
 ****************************************************************************/

#define L4_PAGEMASK     (~(L4_PAGESIZE - 1))
#define L4_LOG2_PAGESIZE (12)
#define L4_WHOLE_ADDRESS_SPACE (64)
#define L4_FPAGE_RO     0           /* read-only fpage */
#define L4_FPAGE_RW     1           /* read-write fpage */
#define L4_FPAGE_MAP    0           /* map fpage */
#define L4_FPAGE_GRANT  1           /* grant fpage */ 

#define L4_FPAGE_GRANT_MASK 1 /* masks for manipulations as integer */
#define L4_FPAGE_RW_MASK    2

#if defined(_LANGUAGE_C)

/* function to build fpage descriptors */
extern  l4_fpage_t l4_fpage(dword_t address, /* address of fpage */
                            unsigned int size, /* size of fpage in 'bits' */
                            unsigned char write, /* read-only / read-write */
                            unsigned char grant); /* map or grant */

/****************************************************************************
 * L4 message dopes
 ****************************************************************************/

/* layout of a message dope */
typedef struct {
  unsigned pad:32;    /* upper 32-bits zero */
  unsigned dwords:19;
  unsigned strings:5;
  unsigned error_code:3;
  unsigned snd_error:1;
  unsigned src_inside:1;
  unsigned msg_redirected:1;
  unsigned fpage_received:1;
  unsigned msg_deceited:1;
} l4_msgdope_struct_t;

/* general purpose msgdope type allowing access as */
typedef union {
  dword_t msgdope;   /* 64-bit int */
  l4_msgdope_struct_t md; /* fields in struct */
} l4_msgdope_t;

/****************************************************************************
 * L4 string dopes
 ****************************************************************************/
 
typedef struct {
  dword_t snd_size;    /* size of string to send */
  dword_t snd_str;     /* pointer to string to send */
  dword_t rcv_size;    /* size of receive buffer */
  dword_t rcv_str;     /* pointer to receive buffer */
} l4_strdope_t;


/****************************************************************************
 * L4 message header
 ****************************************************************************/

typedef struct {
  l4_fpage_t rcv_fpage;     /* rcv fpage option */
  l4_msgdope_t size_dope;   /* size dope of message */
  l4_msgdope_t snd_dope;    /* send dope of message */
} l4_msghdr_t;


/****************************************************************************
 * L4 timeouts
 ****************************************************************************/

/* layout of a timeout */
typedef struct {
  unsigned pad:32;
  unsigned rcv_man:8;     /* receive mantissa */
  unsigned snd_man:8;     /* send mantissa */
  unsigned rcv_pfault:4;  /* receive pagefault timeout */
  unsigned snd_pfault:4;  /* send pagefault timeout */
  unsigned snd_exp:4;     /* send exponent */
  unsigned rcv_exp:4;     /* receive exponent */
} l4_timeout_struct_t;

/* general purpose timeout type that allows access as */
typedef union {
  dword_t timeout;             /* timeout as 64-bit int */
  l4_timeout_struct_t to;      /* timeout as fields in struct */
} l4_timeout_t;

#endif


/****************************************************************************
 * useful constants, macros and functions for manipulating timeouts
 ****************************************************************************/

/* masks for manipulating timeouts as integers */
#define L4_RCV_EXP_MASK        0x0000000f
#define L4_SND_EXP_MASK        0x000000f0
#define L4_SND_PFLT_MASK       0x00000f00
#define L4_RCV_PFLT_MASK       0x0000f000
#define L4_SND_MAN_MASK        0x00ff0000
#define L4_RCV_MAN_MASK        0xff000000

#if defined(_LANGUAGE_C)

/* function to build timeout descriptor */
extern  l4_timeout_t L4_IPC_TIMEOUT(byte_t snd_man, /* send mantissa */
                                    byte_t snd_exp, /* send exponent */
                                    byte_t rcv_man, /* receive mantissa */
                                    byte_t rcv_exp, /* receive exponent */
                                    byte_t snd_pflt, /* send pageflt timeout */
                                    byte_t rcv_pflt); /* rcv pageflt timeout */

/* constant to specify to never timeout during ipc */
#ifdef __GNUC__
#define  L4_IPC_NEVER ((l4_timeout_t) {timeout: 0})
#else
#define  L4_IPC_NEVER _l4_ipc_never
extern const l4_timeout_t _l4_ipc_never;
#endif
#else /* assembler */
#define L4_IPC_NEVER      0
#endif


/****************************************************************************
 * l4_schedule param word: NOT USED in current version 
 ****************************************************************************/

#if defined(_LANGUAGE_C)
typedef struct {
  unsigned pad:32;
  unsigned time_man:8;
  unsigned time_exp:4;
  unsigned tstate:4;
  unsigned zero:8;
  unsigned prio:8;
} l4_sched_param_struct_t;

typedef union {
  dword_t sched_param;
  l4_sched_param_struct_t sp;
} l4_sched_param_t;

#endif

#endif /* __L4TYPES_H__ */ 
