#ifndef _GT64010A_H
#define _GT64010A_H
/****************************************************************************
 * $Id: gt64010a.h,v 1.2 1998/01/22 05:46:04 kevine Exp $
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
 * Register Map
 ****************************************************************************/

/* power up base address of chip */
#define GT_BASE_ADDR           0x14000000 

/* CPU Interface */
#define GT_CPU_IC               0x000

/* Processor Address Space */
#define GT_RAS10_LDA            0x008
#define GT_RAS10_HDA            0x010
#define GT_RAS32_LDA            0x018
#define GT_RAS32_HDA            0x020
#define GT_CS20_LDA             0x028
#define GT_CS20_HDA             0x030
#define GT_CS3B_LDA             0x038
#define GT_CS3B_HDA             0x040
#define GT_PCI_IO_LDA           0x048
#define GT_PCI_IO_HDA           0x050
#define GT_PCI_MEM_LDA          0x058
#define GT_PCI_MEM_HDA          0x060
#define GT_ISD                  0x068
#define GT_BUSERR_L             0x070
#define GT_BUSERR_H             0x078

/* DRAM and Device Address Space */
#define GT_RAS0_LDA             0x400
#define GT_RAS0_HDA             0x404
#define GT_RAS1_LDA             0x408
#define GT_RAS1_HDA             0x40c
#define GT_RAS2_LDA             0x410
#define GT_RAS2_HDA             0x414
#define GT_RAS3_LDA             0x418
#define GT_RAS3_HDA             0x41c
#define GT_CS0_LDA              0x420
#define GT_CS0_HDA              0x424
#define GT_CS1_LDA              0x428
#define GT_CS1_HDA              0x42c
#define GT_CS2_LDA              0x430
#define GT_CS2_HDA              0x434
#define GT_CS3_LDA              0x438
#define GT_CS3_HDA              0x43c
#define GT_BCS_LDA              0x440
#define GT_BCS_HDA              0x444
#define GT_ADE                  0x470

/* DRAM configuration */
#define GT_DRAM_C               0x448

/* DRAM Parameters */
#define GT_DRAM_BANK0           0x44c
#define GT_DRAM_BANK1           0x450
#define GT_DRAM_BANK2           0x454
#define GT_DRAM_BANK3           0x458

/* Device Parameters */
#define GT_DEV_BANK0            0x45c
#define GT_DEV_BANK1            0x460
#define GT_DEV_BANK2            0x464
#define GT_DEV_BANK3            0x468
#define GT_BOOT_BANK            0x46c

/* DMA Record */
#define GT_DMA0_BC              0x800
#define GT_DMA1_BC              0x804
#define GT_DMA2_BC              0x808
#define GT_DMA3_BC              0x80c
#define GT_DMA0_SA              0x810
#define GT_DMA1_SA              0x814
#define GT_DMA2_SA              0x818
#define GT_DMA3_SA              0x81c
#define GT_DMA0_DA              0x820
#define GT_DMA1_DA              0x824
#define GT_DMA2_DA              0x828
#define GT_DMA3_DA              0x82c
#define GT_DMA0_NRP             0x830
#define GT_DMA1_NRP             0x834
#define GT_DMA2_NRP             0x838
#define GT_DMA3_NRP             0x83c

/* DMA Channel Control */
#define GT_DMA0_CNTRL           0x840
#define GT_DMA1_CNTRL           0x844
#define GT_DMA2_CNTRL           0x848
#define GT_DMA3_CNTRL           0x84c

/* DMA Arbiter */
#define GT_DMA_ARBITER          0x860

/* Timer / Counter */
#define GT_TIMER0               0x850
#define GT_TIMER1               0x854
#define GT_TIMER2               0x858
#define GT_TIMER3               0x85c
#define GT_TIMER_CNTRL          0x864

/* PCI Internal */
#define GT_PCI_CMD              0xc00
#define GT_PCI_TOR              0xc04
#define GT_PCI_RAS10_BS         0xc08
#define GT_PCI_RAS32_BS         0xc0c
#define GT_PCI_CS20_BS          0xc10
#define GT_PCI_CS3B_BS          0xc14
#define GT_PCI_SERR_MASK        0xc28
#define GT_PCI_INT_ACK          0xc34
#define GT_PCI_CONF_ADDR        0xcf8
#define GT_PCI_CONF_DATA        0xcfc

/* Interrupts */
#define GT_INT_CAUSE            0xc18
#define GT_INT_CPU_MASK         0xc1c
#define GT_INT_PCI_MASK         0xc24

/* pci config registers */
#define GT_PCI_CONF_DVID        0x000
#define GT_PCI_CONF_STCMD       0x004
#define GT_PCI_CONF_CCREVID     0x008
#define GT_PCI_CONF_HDRLAT      0x00c
#define GT_PCI_CONF_RAS10_BASE  0x010
#define GT_PCI_CONF_RAS10_SWAP  0x028
#define GT_PCI_CONF_RAS32_BASE  0x014
#define GT_PCI_CONF_RAS32_SWAP  0x02c
#define GT_PCI_CONF_CS20_BASE   0x018
#define GT_PCI_CONF_CS3B_BASE   0x01c
#define GT_PCI_CONF_CS3B_SWAP   0x030
#define GT_PCI_CONF_REG_MEM     0x020
#define GT_PCI_CONF_REG_IO      0x024
#define GT_PCI_CONF_REG_EN      0x034
#define GT_PCI_CONF_INTPL       0x03c

/****************************************************************************
 * Device Bank Parameters LITTLE ENDIAN, as per docs 
 ****************************************************************************/

#define GT_DB_TurnOff_SHFT            0
#define GT_DB_TurnOff_MASK            (7 << GT_DB_TurnOff_SHFT)

#define GT_DB_AccToFirst_SHFT         3
#define GT_DB_AccToFirst_MASK         (0xf << GT_DB_AccToFirst_SHFT)

#define GT_DB_AccToNext_SHFT          7
#define GT_DB_AccToNext_MASK          (0xf << GT_DB_AccToNext_SHFT)

#define GT_DB_ADStoWr_SHFT            11
#define GT_DB_ADStoWr_MASK            (7 << GT_DB_ADStoWr_SHFT)

#define GT_DB_WrActive_SHFT           14
#define GT_DB_WrActive_MASK           (7 << GT_DB_WrActive_SHFT)

#define GT_DB_WrHigh_SHFT             17
#define GT_DB_WrHigh_MASK             (7 << GT_DB_WrActive_SHFT)

#define GT_DB_DevWidth_SHFT           20
#define GT_DB_DevWidth_MASK           (3 << GT_DB_DevWidth_SHFT)
#define GT_DB_DevWidth_8Bit           0
#define GT_DB_DevWidth_16Bit          (1 << GT_DB_DevWidth_SHFT)
#define GT_DB_DevWidth_32Bit          (2 << GT_DB_DevWidth_SHFT)
#define GT_DB_DevWidth_64Bit          (3 << GT_DB_DevWidth_SHFT)

#define GT_DB_DevLoc_SHFT             23
#define GT_DB_DevLoc_MASK             (1 <<  GT_DB_DevLoc_SHFT)
#define GT_DB_DevLoc_Even             0
#define GT_DB_DevLoc_Odd              GT_DB_DevLoc_MASK

#define GT_DB_LatchFunct_SHFT         25
#define GT_DB_LatchFunct_MASK         (1 << GT_DB_LatchFunct_SHFT)
#define GT_DB_LatchFunct_Trans        0
#define GT_DB_LatchFunct_Enable       GT_DB_LatchFunct_MASK

#define GT_DB_Parity_SHFT             30
#define GT_DB_Parity_MASK             (1 << GT_DB_Parity_SHFT)
#define GT_DB_Parity_Off              0
#define GT_DB_Parity_On               GT_DB_Parity_MASK

/****************************************************************************
 * DRAM CONFIGURATION
 ****************************************************************************/

#define GT_DRAM_C_RefIntCnt_SHFT      0
#define GT_DRAM_C_RefIntCnt_MASK      037777

#define GT_DRAM_C_StagRef_SHFT        16
#define GT_DRAM_C_StagRef_MASK        (1 <<  GT_DRAM_C_StagRef_SHFT)
#define GT_DRAM_C_StagRef_Off         GT_DRAM_C_StagRef_MASK
#define GT_DRAM_C_StagRef_On          0

#define GT_DRAM_C_ADSFunct_SHFT       17
#define GT_DRAM_C_ADSFunct_MASK       (1 << GT_DRAM_C_ADSFunct_SHFT)
#define GT_DRAM_C_ADSFunct_ADSonly    GT_DRAM_C_ADSFunct_MASK
#define GT_DRAM_C_ADSFunct_DAdr11     0

#define GT_DRAM_C_DRAMLatch_SHFT      18
#define GT_DRAM_C_DRAMLatch_MASK      (1 << GT_DRAM_C_DRAMLatch_SHFT)
#define GT_DRAM_C_DRAMLatch_Active    0
#define GT_DRAM_C_DRAMLatch_Trans     GT_DRAM_C_DRAMLatch_MASK

/****************************************************************************
 * DRAM PARAMETERS
 ****************************************************************************/


#define GT_DRAM_P_CASWr_SHFT          0
#define GT_DRAM_P_CASWr_MASK          1
#define GT_DRAM_P_CASWr_1cyc          0
#define GT_DRAM_P_CASWr_2cyc          1

#define GT_DRAM_P_RAStoCASWr_SHFT     1
#define GT_DRAM_P_RAStoCASWr_MASK     (1 << GT_DRAM_P_RAStoCASWr_SHFT)
#define GT_DRAM_P_RAStoCASWr_2cyc     0
#define GT_DRAM_P_RAStoCASWr_3cyc     GT_DRAM_P_RAStoCASWr_MASK

#define GT_DRAM_P_CASRd_SHFT          2
#define GT_DRAM_P_CASRd_MASK          (1 << GT_DRAM_P_CASRd_SHFT)
#define GT_DRAM_P_CASRd_1cyc          0
#define GT_DRAM_P_CASRd_2cyc          GT_DRAM_P_CASRd_MASK

#define GT_DRAM_P_RAStoCASRd_SHFT     3
#define GT_DRAM_P_RAStoCASRd_MASK     (1 << GT_DRAM_P_RAStoCASRd_SHFT)
#define GT_DRAM_P_RAStoCASRd_2cyc     0
#define GT_DRAM_P_RAStoCASRd_3cyc     GT_DRAM_P_RAStoCASRd_MASK

#define GT_DRAM_P_Refresh_SHFT        4
#define GT_DRAM_P_Refresh_MASK        (3 << GT_DRAM_P_Refresh_SHFT)
#define GT_DRAM_P_Refresh_512         0
#define GT_DRAM_P_Refresh_1K          (1 << GT_DRAM_P_Refresh_SHFT)
#define GT_DRAM_P_Refresh_2K          (2 << GT_DRAM_P_Refresh_SHFT)
#define GT_DRAM_P_Refresh_4K          (3 << GT_DRAM_P_Refresh_SHFT)

#define GT_DRAM_P_BankWidth_SHFT      6
#define GT_DRAM_P_BankWidth_MASK      (1 << GT_DRAM_P_BankWidth_SHFT)
#define GT_DRAM_P_BankWidth_32Bit     0
#define GT_DRAM_P_BankWidth_64Bit     GT_DRAM_P_BankWidth_MASK

#define GT_DRAM_P_BankLoc_SHFT        7
#define GT_DRAM_P_BankLoc_MASK        (1 << GT_DRAM_P_BankLoc_SHFT)
#define GT_DRAM_P_BankLoc_Even        0
#define GT_DRAM_P_BankLoc_Odd         GT_DRAM_P_BankLoc_MASK

#define GT_DRAM_P_Parity_SHFT         8
#define GT_DRAM_P_Parity_MASK         (1 << GT_DRAM_P_Parity_SHFT)
#define GT_DRAM_P_Parity_Off          0
#define GT_DRAM_P_Parity_On           GT_DRAM_P_Parity_MASK

/****************************************************************************
 * PCI registers constants
 ****************************************************************************/

#define GT_PCI_NOBYTESWAP             1
#define GT_PCI_BYTESWAP               0

#define GT_PCI_SYNCALL                0
#define GT_PCI_SYNCHIGH               (1 << 1)
#define GT_PCI_SYNCSYNC               (2 << 1)
#define GT_PCI_CONFIGEN               (1 << 31)

#define GT_PCI_IOEN                   1
#define GT_PCI_MEMEN                  (1 << 1)
#define GT_PCI_MASEN                  (1 << 2)
#define GT_PCI_MEMWRINV               (1 << 4)
#define GT_PCI_PERREN                 (1 << 6)
#define GT_PCI_SERREN                 (1 << 8)

#define GT_PCI_RAS10DIS                (1 << 8)
#define GT_PCI_RAS32DIS                (1 << 7)
#define GT_PCI_CS20DIS                 (1 << 6)
#define GT_PCI_CS3BDIS                 (1 << 5)
#define GT_PCI_INTMEMDIS               (1 << 4)
#define GT_PCI_INTIODIS                (1 << 3)
#define GT_PCI_SWRAS10DIS              (1 << 2)
#define GT_PCI_SWRAS32DIS              (1 << 1)
#define GT_PCI_SWCS3BDIS               1

/****************************************************************************
 * Interrrupt cause register masks
 ****************************************************************************/

#define GT_INT_TIM0EXP              (1 << 8)
#define GT_INT_MASABORT             (1 << 18)
#define GT_INT_TARABORT             (1 << 19)

/****************************************************************************
 * Timer register masks
 ****************************************************************************/
#define GT_TIM0_EN                  1
#define GT_TIM0_CNT                 0
#define GT_TIM0_TMR                 (1 << 1)
#define GT_TIM1_EN                  (1 << 2)
#define GT_TIM1_CNT                 0
#define GT_TIM1_TMR                 (1 << 3)
#define GT_TIM2_EN                  (1 << 4)
#define GT_TIM2_CNT                 0
#define GT_TIM2_TMR                 (1 << 5)
#define GT_TIM3_EN                  (1 << 6)
#define GT_TIM3_CNT                 0
#define GT_TIM3_TMR                 (1 << 7)



#endif
