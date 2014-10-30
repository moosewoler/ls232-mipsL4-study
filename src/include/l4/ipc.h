#ifndef __L4_IPC_H__ 
#define __L4_IPC_H__ 
/****************************************************************************
 * $Id: ipc.h,v 1.12 1998/12/23 02:03:58 gernot Exp $
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

#include <l4/types.h>

/****************************************************************************
 * L4 IPC
 ****************************************************************************/

#ifdef _LANGUAGE_C

/****************************************************************************
 * L4 registered message. Structure used to pass registered message to the
 * libl4 C library which loads/stores messages to be sent/received into/from
 * registers from/into this structure. If you get my gist :-)
 ****************************************************************************/

#define L4_IPC_MAX_REG_MSG 8

typedef struct {
  dword_t reg[L4_IPC_MAX_REG_MSG];
} l4_ipc_reg_msg_t;

#endif

/****************************************************************************
 * Defines used for constructing send and receive descriptors
 ****************************************************************************/

#define L4_IPC_SHORT_MSG        0    /* register only ipc */

#ifdef _LANGUAGE_C
#define L4_IPC_STRING_SHIFT 8 /* shift amount to get strings
                                 from message dope */
#define L4_IPC_DWORD_SHIFT 13 /* shift ammount to get dwords
                                 from message dope */

#define L4_IPC_SHORT_FPAGE ((void *)2ul) /* register only ipc including
                                            sending fpages in registers */

/* macro for creating receive descriptor that receives register
   only ipc that includes fpages */
#define L4_IPC_MAPMSG(address, size)  \
     ((void *)(dword_t)( ((address) & L4_PAGEMASK) | ((size) << 2) \
                         | (unsigned long)L4_IPC_SHORT_FPAGE))
#else /* assembly */
     
#define L4_IPC_SHORT_FPAGE          2    
#define L4_IPC_NIL_DESCRIPTOR   (-1)
#define L4_IPC_DECEIT           1
#define L4_IPC_OPEN_IPC         1

#endif
     
/****************************************************************************
 * Some macros to make result checking easier
 ****************************************************************************/
     
#define L4_IPC_ERROR_MASK       0xF0
#define L4_IPC_DECEIT_MASK      0x01
#define L4_IPC_FPAGE_MASK       0x02
#define L4_IPC_REDIRECT_MASK    0x04
#define L4_IPC_SRC_MASK         0x08
#define L4_IPC_SND_ERR_MASK     0x10

#ifdef _LANGUAGE_C
#define L4_IPC_IS_ERROR(x)              (((x).msgdope) & L4_IPC_ERROR_MASK)
#define L4_IPC_MSG_DECEIVED(x)          (((x).msgdope) & L4_IPC_DECEIT_MASK)
#define L4_IPC_MSG_REDIRECTED(x)        (((x).msgdope) & L4_IPC_REDIRECT_MASK)
#define L4_IPC_SRC_INSIDE(x)            (((x).msgdope) & L4_IPC_SRC_MASK)
#define L4_IPC_SND_ERROR(x)             (((x).msgdope) & L4_IPC_SND_ERR_MASK)
#define L4_IPC_MSG_TRANSFER_STARTED(x) \
     ((((x).msgdope) & L4_IPC_ERROR_MASK) > 0x50)
     

/****************************************************************************
 * Prototypes for IPC calls implemented in libl4.a
 ****************************************************************************/
     
extern int
l4_mips_ipc_call(l4_threadid_t     dest, 
                 const void        *snd_msg,
                 l4_ipc_reg_msg_t  *snd_reg,
                 void              *rcv_msg,
                 l4_ipc_reg_msg_t  *rcv_reg,
                 l4_timeout_t      timeout,
                 l4_msgdope_t      *result);

extern int
l4_mips_ipc_reply_and_wait(l4_threadid_t     dest, 
                           const void        *snd_msg, 
                           l4_ipc_reg_msg_t  *snd_reg,
                           l4_threadid_t     *src,
                           void              *rcv_msg,
                           l4_ipc_reg_msg_t  *rcv_reg,
                           l4_timeout_t      timeout,
                           l4_msgdope_t      *result);

extern int 
l4_mips_ipc_reply_deceiving_and_wait(l4_threadid_t     dest,
                                     l4_threadid_t     vsend,
                                     const void        *snd_msg, 
                                     l4_ipc_reg_msg_t  *snd_reg,
                                     l4_threadid_t     *src,
                                     void              *rcv_msg, 
                                     l4_ipc_reg_msg_t  *rcv_reg,
                                     l4_timeout_t      timeout, 
                                     l4_msgdope_t      *result);

extern int 
l4_mips_ipc_send(l4_threadid_t     dest, 
                 const void        *snd_msg,
                 l4_ipc_reg_msg_t  *snd_reg,
                 l4_timeout_t      timeout,
                 l4_msgdope_t      *result);

extern int 
l4_mips_ipc_send_deceiving(l4_threadid_t     dest,
                           l4_threadid_t     vsend,
                           const void        *snd_msg, 
                           l4_ipc_reg_msg_t  *snd_reg,
                           l4_timeout_t      timeout,
                           l4_msgdope_t      *result);

extern int 
l4_mips_ipc_wait(l4_threadid_t     *src,
                 void              *rcv_msg,
                 l4_ipc_reg_msg_t  *rcv_reg,
                 l4_timeout_t      timeout,
                 l4_msgdope_t      *result);

extern int 
l4_mips_ipc_receive(l4_threadid_t     src,
                    void              *rcv_msg,
                    l4_ipc_reg_msg_t  *rcv_reg,
                    l4_timeout_t      timeout,
                    l4_msgdope_t      *result);

extern int l4_mips_ipc_sleep(l4_timeout_t t,
                             l4_msgdope_t *result);

/* IPC bindings for chiefs */

extern int 
l4_mips_ipc_chief_wait(l4_threadid_t     *src,
                       l4_threadid_t     *real_dst,
                       void              *rcv_msg,
                       l4_ipc_reg_msg_t  *rcv_reg,
                       l4_timeout_t      timeout,
                       l4_msgdope_t      *result);

extern int 
l4_mips_ipc_chief_receive(l4_threadid_t     src,
                          l4_threadid_t     *real_dst,
                          void              *rcv_msg,
                          l4_ipc_reg_msg_t  *rcv_reg,
                          l4_timeout_t      timeout,
                          l4_msgdope_t      *result);

extern int
l4_mips_ipc_chief_call(l4_threadid_t     dest,
                       l4_threadid_t     vsend,
                       const void        *snd_msg,
                       l4_ipc_reg_msg_t  *snd_reg,
                       l4_threadid_t     *real_dst,
                       void              *rcv_msg,
                       l4_ipc_reg_msg_t  *rcv_reg,
                       l4_timeout_t      timeout,
                       l4_msgdope_t      *result);

extern int
l4_mips_ipc_chief_reply_and_wait(l4_threadid_t     dest,
                                 l4_threadid_t     vsend,
                                 const void        *snd_msg, 
                                 l4_ipc_reg_msg_t  *snd_reg,
                                 l4_threadid_t     *src,
                                 l4_threadid_t     *real_dst,
                                 void              *rcv_msg,
                                 l4_ipc_reg_msg_t  *rcv_reg,
                                 l4_timeout_t      timeout,
                                 l4_msgdope_t      *result);

extern int 
l4_mips_ipc_chief_send(l4_threadid_t     dest,
                       l4_threadid_t     vsend,
                       const void        *snd_msg,
                       l4_ipc_reg_msg_t  *snd_reg,
                       l4_timeout_t      timeout,
                       l4_msgdope_t      *result);

/****************************************************************************
 * some functions to examine fpages
 ****************************************************************************/
extern int l4_ipc_fpage_received(l4_msgdope_t msgdope); /* test if fpages
                                                           received */
extern int l4_ipc_is_fpage_granted(l4_fpage_t fp);
extern int l4_ipc_is_fpage_writable(l4_fpage_t fp);


/****************************************************************************
 * Symbolic constants for error codes, see reference manual for details
 ****************************************************************************/

#define L4_IPC_ERROR(x)                 (((x).msgdope) & L4_IPC_ERROR_MASK)
#endif 
#define L4_IPC_ENOT_EXISTENT            0x10
#define L4_IPC_RETIMEOUT                0x20
#define L4_IPC_SETIMEOUT                0x30
#define L4_IPC_RECANCELED               0x40
#define L4_IPC_SECANCELED               0x50
#define L4_IPC_REMAPFAILED              0x60
#define L4_IPC_SEMAPFAILED              0x70
#define L4_IPC_RESNDPFTO                0x80
#define L4_IPC_SESNDPFTO                0x90
#define L4_IPC_RERCVPFTO                0xA0
#define L4_IPC_SERCVPFTO                0xB0
#define L4_IPC_REABORTED                0xC0
#define L4_IPC_SEABORTED                0xD0
#define L4_IPC_REMSGCUT                 0xE0
#define L4_IPC_SEMSGCUT                 0xF0


/****************************************************************************
 * Size limitations on memory based IPC
 ****************************************************************************/

#define L4_MAX_DMSG_SIZE       (4*1024*1024) /* max direct message size */
#define L4_MAX_STRING_SIZE     (4*1024*1024) /* max indirect message size */

#endif /* __L4_IPC__ */
