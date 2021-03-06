;
; Copyright (c) 2012, RISC OS Open Ltd
; Copyright (c) 2012, John Ballance
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met: 
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of RISC OS Open Ltd nor the names of its contributors
;       may be used to endorse or promote products derived from this software
;       without specific prior written permission.
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
; LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.
;
;

        AREA    |ARM$$code|, CODE, READONLY, PIC

;        GET     Hdr:ListOpts
;        GET     hdr/BCM2835
;        GET     hdr/StaticWS

        IMPORT  workspace

;
; Mailbox
;
; There are two mailboxes; we write to mailbox 1 and read from mailbox 0
;
MB_Base        *       &0000b800     ; offset from IO space start
MB_DBell0      *       &40           ; doorbell 0 (VCHIQ VC -> ARM)
MB_DBell1      *       &44           ; doorbell 1
MB_DBell2      *       &48           ; doorbell 2 (VCHIQ ARM -> VC)
MB_DBell3      *       &4c           ; doorbell 3
MB_ChRd        *       &80           ; normal read - offset from MB_Base
MB_ChWr        *       &a0           ; normal write
MB_ChRWTop     *       &8c           ; 4 word read or write
MB_Pol         *       &90           ; NonPOP read
MB_Snd         *       &94           ; sender read (bottom 2 bits)
MB_Sta         *       &98           ; status read
MB_Cnf         *       &9c           ; config r/w
;
; mailbox register bits
;
; MB_Sta
MB_Sta_Full    *       &80000000     ; mailbox full
MB_Sta_Empty   *       &40000000     ; mailbox empty
MB_Sta_Level   *       &000000ff     ; mailbox content count
; MB_Cnf
MB_Cnf_HDIrqEn *       &00000001     ; mailbox has-data irq enable
MB_Cnf_HSIrqEn *       &00000002     ; mailbox has-space irq enable
MB_Cnf_OpIrqEn *       &00000004     ; mailbox Opp irq en
MB_Cnf_MBClr   *       &00000008     ; write 1 then 0 to clear mailbox
MB_Cnf_HDIrq   *       &00000010     ; mailbox has-data irq pending
MB_Cnf_HSIrq   *       &00000020     ; mailbox has-space irq pending
MB_Cnf_OpIrq   *       &00000040     ; mailbox Opp irq pending
; these flags are reset on any write to this register
MB_Cnf_ErNone  *       &00000100     ; none read error
MB_Cnf_ErWFull *       &00000200     ; Write to full mailbox
MB_Cnf_ErREmty *       &00000400     ; read from empty mailbox
;
; mailbox channel numbers
MB_Chan_Pwr    *       0             ; Power channel
MB_Chan_FB     *       1             ; Frame Buffer channel
MB_Chan_VUart  *       2             ; Virtual UART channel
MB_Chan_VCHIQ  *       3             ; VCHIQ channel
MB_Chan_LEDS   *       4             ; LEDS channel
MB_Chan_Btn    *       5             ; Buttons channel
MB_Chan_TSc    *       6             ; TouchScreen channel
MB_Chan_ARM2VC *       8             ; ARM -> VC property channel
MB_Chan_VC2ARM *       9             ; VC -> ARM property channel
; far end replies on the same channel when command done.. e.g.
; command c0000001 gets 00000001 (ie channel1) reply
; Power channel bits
MB_Pwr_SDCard       *  0
MB_Pwr_UART         *  1
MB_Pwr_MiniUART     *  2
MB_Pwr_USB          *  3
MB_Pwr_I2C0         *  4
MB_Pwr_I2C1_MASK    *  5
MB_Pwr_I2C2_MASK    *  6
MB_Pwr_SPI_MASK     *  7
MB_Pwr_CCP2TX_MASK  *  8
; ARM2VC tags
; see https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface
; Note - this interface isn't implemented in the GPU firmware yet!
ARM2VC_Tag_End                   * &00000000 ; End of tag list
ARM2VC_Tag_GetFirmwareVersion    * &00000001 ; return 4 byte firmware version
ARM2VC_Tag_GetBoardModel         * &00010001 ; return 4 byte model
ARM2VC_Tag_GetBoardRevision      * &00010002 ; return 4 byte revision
ARM2VC_Tag_GetBoardMAC           * &00010003 ; return 6 byte MAC
ARM2VC_Tag_GetBoardSerial        * &00010004 ; return 8 byte serial
ARM2VC_Tag_GetARMMemory          * &00010005 ; return ARM mem base + size
ARM2VC_Tag_GetVCMemory           * &00010006 ; return VC mem base + size
ARM2VC_Tag_GetClocks             * &00010007 ; return clock tree
ARM2VC_Tag_GetConfig             * &00050001 ; get kernel boot args string
ARM2VC_Tag_GetDMAChannels        * &00060001 ; get mask of DMA channels usable by ARM
ARM2VC_Tag_GetPowerState         * &00020001 ; get power state of specified MB_Pwr_ device
ARM2VC_Tag_GetPowerTiming        * &00020002 ; get microsecond delay required after power on for indicated device
ARM2VC_Tag_SetPowerState         * &00028001 ; set power state of device
ARM2VC_Tag_GetClockState         * &00030001 ; get power state of clock
ARM2VC_Tag_SetClockState         * &00038001 ; set power state of clock
ARM2VC_Tag_GetClockRate          * &00030002 ; get rate of clock
ARM2VC_Tag_SetClockRate          * &00038002 ; set rate of clock
ARM2VC_Tag_FBAlloc               * &00040001 ; alloc framebuffer at given alignment
ARM2VC_Tag_FBRelease             * &00048001 ; release framebuffer
ARM2VC_Tag_FBBlank               * &00040002 ; blank screen
ARM2VC_Tag_FBGetPhysDimension    * &00040003 ; get physical display width/height
ARM2VC_Tag_FBTestPhysDimension   * &00044003 ; test physical display w/h
ARM2VC_Tag_FBSetPhysDimension    * &00048003 ; set physical display w/h
ARM2VC_Tag_FBGetVirtDimension    * &00040004 ; get virtual display w/h
ARM2VC_Tag_FBTestVirtDimension   * &00044004 ; test virtual display w/h
ARM2VC_Tag_FBSetVirtDimension    * &00048004 ; set virtual display w/h
ARM2VC_Tag_FBGetDepth            * &00040005 ; get display BPP
ARM2VC_Tag_FBTestDepth           * &00044005 ; test display BPP
ARM2VC_Tag_FBSetDepth            * &00048005 ; set display BPP
ARM2VC_Tag_FBGetPixelOrder       * &00040006 ; get RGB/BGR order
ARM2VC_Tag_FBTestPixelOrder      * &00044006 ; test RGB/BGR order
ARM2VC_Tag_FBSetPixelOrder       * &00048006 ; set RGB/BGR order
ARM2VC_Tag_FBGetAlphaMode        * &00040007 ; get alpha mode
ARM2VC_Tag_FBTestAlphaMode       * &00044007 ; test alpha mode
ARM2VC_Tag_FBSetAlphaMode        * &00048007 ; set alpha mode
ARM2VC_Tag_FBGetPitch            * &00040008 ; get row pitch
ARM2VC_Tag_FBGetVirtOffset       * &00040009 ; get X/Y offset of virtual display
ARM2VC_Tag_FBTestVirtOffset      * &00044009 ; test X/Y offset
ARM2VC_Tag_FBSetVirtOffset       * &00048009 ; set X/Y offset
ARM2VC_Tag_FBGetOverscan         * &0004000a ; get overscan values
ARM2VC_Tag_FBTestOverscan        * &0004400a ; test overscan values
ARM2VC_Tag_FBSetOverscan         * &0004800a ; set overscan values
ARM2VC_Tag_FBGetPalette          * &0004000b ; get full palette table
ARM2VC_Tag_FBTestPalette         * &0004400b ; test ranged update of table
ARM2VC_Tag_FBSetPalette          * &0004800b ; do ranged update of table

                                      

        EXPORT  HAL_SendHostMessage
        EXPORT  HAL_QueryPlatform
        EXPORT  MacAdd
        EXPORT  RamAd
        EXPORT  SerNo
        EXPORT  Displ
        EXPORT	Tags

        MACRO
$label  DoMemBarrier $r, $cond
$label  MOV$cond $r, #0
        MCR$cond p15, 0, $r, c7, c10,5
        MEND
        
        MACRO
$label  FlushDataCache $r, $cond
$label  MOV$cond $r, #0
        MCR$cond p15, 0, $r, c7, c14,0
        MEND

        MACRO
$label  FlushDataCacheRange $startaddr,$endaddr,$cond
$label  BIC$cond $startaddr, $startaddr, #&1f
        MCRR$cond p15, 0, $endaddr, $startaddr, c14
        MEND

; send a message packet to the host and await the reply
; on entry, r0 =  message channel to use and/or wholemessage
;           r1 -> message tag buffer, 16 byte aligned. or 0
;	    r2 -> io base (in current memory space)
;           
; on exit,  r0 = mailbox response word
;          
HAL_SendHostMessage  ROUT
        STMFD   r13!, {r1-r3, lr}
        DoMemBarrier r3
        FlushDataCache r3
;        LDR     r3, PeriBase
	MOV	r3, r2			; I/O base
	ADD     r3, r3, #MB_Base
; check we can send a message
001     LDR     r2,[r3, #MB_Sta]
        TST     r2, #MB_Sta_Full
        BNE     %BT001                ; write channel full
; send message
        TEQ     r1, #0
        BICNE   r1, r1, #&c0000000
;        LDRNE   r2, FB_CacheMode
;        ORRNE   r1, r1, r2
        ORR     r2, r0, r1
        AND     r1, r0, #&f            ; isolate channel number
        STR     r2,[r3, #MB_ChWr]
; await response and check it is ours
002     LDR     r0,[r3, #MB_Sta]
        TST     r0, #MB_Sta_Empty
        BNE     %BT002                ; still empty
        LDR     r0,[r3,#MB_ChRd]
        AND     r2, r0, #&f
        CMP     r2, r1                ; check its is our channel
        BNE     %BT002                ; not our reply
        DoMemBarrier r3
        LDMFD   r13!, {r1-r3, pc}     ; returning composite response in r0


Tags	ROUT
Tagsb    DCD     Tagsslen
        DCD     0        
Tagmac  DCD     ARM2VC_Tag_GetBoardMAC
        DCD     8
        DCD     0        
TMAClo   DCD     0        
TMAChi   DCD     0        
Tagserial
        DCD     ARM2VC_Tag_GetBoardSerial
        DCD     8
        DCD     0        
TSNlo    DCD     0
TSNhi    DCD     0        
Tagarmmem
        DCD     ARM2VC_Tag_GetARMMemory
        DCD     8
        DCD     0        
TARMbs   DCD     0
TARMsz   DCD     0
Tagvcmem
        DCD     ARM2VC_Tag_GetVCMemory
        DCD     8
        DCD     0        
TVCbs    DCD     0
TVCsz    DCD     0        
        DCD     ARM2VC_Tag_GetClockRate
        DCD     8
        DCD	0
TClk        DCD     0x1 ; EMMC
	DCD     0
        DCD     ARM2VC_Tag_End
Tagsslen *       . - Tags

; series of VC side query tags. Using inline code as this is writable at this 
; stage. This means the answers will be encapsulated in rom image!!
;
; CURRENT ASSIGNED SPACE 256 bytes.. BEWARE
;
tagb    DCD     tagslen
        DCD     0        
tagmac  DCD     ARM2VC_Tag_GetBoardMAC
        DCD     8
        DCD     0        
MAClo   DCD     0        
MAChi   DCD     0        
tagserial
        DCD     ARM2VC_Tag_GetBoardSerial
        DCD     8
        DCD     0        
SNlo    DCD     0
SNhi    DCD     0        
tagarmmem
        DCD     ARM2VC_Tag_GetARMMemory
        DCD     8
        DCD     0        
ARMbs   DCD     0
ARMsz   DCD     0
tagvcmem
        DCD     ARM2VC_Tag_GetVCMemory
        DCD     8
        DCD     0        
VCbs    DCD     0
VCsz    DCD     0        
tagdisplphyswh
        DCD     ARM2VC_Tag_FBSetPhysDimension
        DCD     8
        DCD     8
phyx    DCD     1920
        DCD     1080
tagdisplvirtwh
        DCD     ARM2VC_Tag_FBSetVirtDimension
        DCD     8
        DCD     8
virtx   DCD     1920
        DCD     1080
tagdisplvirtoffset
        DCD     ARM2VC_Tag_FBSetVirtOffset
        DCD     8
        DCD     8
vxoff   DCD     0
        DCD     0
tagdispldepth           
        DCD     ARM2VC_Tag_FBSetDepth
        DCD     4
        DCD     4
dispbpp DCD     32                 ; 32bit
tagdisplpixord           
        DCD     ARM2VC_Tag_FBSetPixelOrder
        DCD     4
        DCD     4
        DCD     1                  ; RGB
tagdisplalpha           
        DCD     ARM2VC_Tag_FBSetAlphaMode
        DCD     4
        DCD     4
        DCD     2                  ; channel 1=alpha reversed 2=ignore
taggetpitch           
        DCD     ARM2VC_Tag_FBGetPitch
        DCD     4
        DCD     0
dispit  DCD     0
tagdisplalloc           
        DCD     ARM2VC_Tag_FBAlloc
        DCD     8
        DCD     8
Dispbs  DCD     0x100000            ; megabyte aligned
Dispsz  DCD     0           
        DCD     ARM2VC_Tag_End
tagslen *       . - tagb


     


                END

