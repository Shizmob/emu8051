/* 8051 emulator core
 * Copyright 2006 Jari Komppa
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the 
 * "Software"), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject 
 * to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE. 
 *
 * (i.e. the MIT License)
 *
 * emu8051.h
 * Emulator core header file
 */
#ifndef EM8051_MINIMAL
#define EM8051_MINIMAL 0
#endif

struct em8051;

// Operation: returns number of ticks the operation should take
typedef int (*em8051operation)(struct em8051 *aCPU); 

// Decodes opcode at position, and fills the buffer with the assembler code. 
// Returns how many bytes the opcode takes.
typedef int (*em8051decoder)(struct em8051 *aCPU, int aPosition, char *aBuffer);

// Callback: some exceptional situation occurred. See EM8051_EXCEPTION enum, below
typedef void (*em8051exception)(struct em8051 *aCPU, int aCode);

// Callback: an SFR register is about to be read (not called for 'a' ops nor psw changes)
// Default is to return the value in the SFR register. Ports may act differently.
typedef int (*em8051sfrread)(struct em8051 *aCPU, int aRegister);

// Callback: an SFR register has changed (not called for 'a' ops)
// Default is to do nothing
typedef void (*em8051sfrwrite)(struct em8051 *aCPU, int aRegister);

// Callback: writing to external memory
// Default is to update external memory
// (can be used to control some peripherals)
typedef void (*em8051xwrite)(struct em8051 *aCPU, int aAddress, int aValue);

// Callback: reading from external memory
// Default is to return the value in external memory 
// (can be used to control some peripherals)
typedef int (*em8051xread)(struct em8051 *aCPU, int aAddress);


struct em8051
{
    unsigned char *mCodeMem; // 1k - 64k, must be power of 2
    int mCodeMemSize; 
    unsigned char *mExtData; // 0 - 64k, must be power of 2
    int mExtDataSize;
    unsigned char *mLowerData; // 128 bytes
    unsigned char *mUpperData; // 0 or 128 bytes; leave to NULL if none
    unsigned char *mSFR; // 128 bytes; (special function registers)
    int mPC; // Program Counter; outside memory area
    int mTickDelay; // How many ticks should we delay before continuing
    em8051operation op[256]; // function pointers to opcode handlers
#if !EM8051_MINIMAL
    em8051decoder dec[256];
#endif
    em8051exception except; // callback: exceptional situation occurred
    em8051sfrread sfrread; // callback: SFR register being read
    em8051sfrwrite sfrwrite; // callback: SFR register written
    em8051xread xread; // callback: external memory being read
    em8051xwrite xwrite; // callback: external memory being written

    // Internal values for interrupt services etc.
    int mInterruptActive;
    // Stored register values for interrupts (exception checking)
    int int_a[2];
    int int_psw[2];
    int int_sp[2];
};

// set the emulator into reset state. Must be called before tick(), as
// it also initializes the function pointers. aWipe tells whether to reset
// all memory to zero.
void em8051_reset(struct em8051 *aCPU, int aWipe);

// run one emulator tick, or 12 hardware clock cycles.
// returns 1 if a new operation was executed.
int em8051_tick(struct em8051 *aCPU);

// decode the next operation as character string.
// buffer must be big enough (64 bytes is very safe). 
// Returns length of opcode.
int em8051_decode(struct em8051 *aCPU, int aPosition, unsigned char *aBuffer);

#if !EM8051_MINIMAL
// Load an intel hex format object file. Returns negative for errors.
int em8051_load_obj(struct em8051 *aCPU, char *aFilename);
#endif

// Alternate way to execute an opcode (switch-structure instead of function pointers)
int em8051_do_op(struct em8051 *aCPU);

// Internal: Pushes a value into stack
void em8051_push_to_stack(struct em8051 *aCPU, int aValue);


// SFR register locations
enum EM8051_SFR_REGS
{
    EM8051_REG_ACC = 0xE0 - 0x80,
    EM8051_REG_B   = 0xF0 - 0x80,
    EM8051_REG_PSW = 0xD0 - 0x80,
    EM8051_REG_SP  = 0x81 - 0x80,
    EM8051_REG_DPL = 0x82 - 0x80,
    EM8051_REG_DPH = 0x83 - 0x80,
    EM8051_REG_P0  = 0x80 - 0x80,
    EM8051_REG_P1  = 0x90 - 0x80,
    EM8051_REG_P2  = 0xA0 - 0x80,
    EM8051_REG_P3  = 0xB0 - 0x80,
    EM8051_REG_IP  = 0xB8 - 0x80,
    EM8051_REG_IE  = 0xA8 - 0x80,
    EM8051_REG_TMOD = 0x89 - 0x80,
    EM8051_REG_TCON = 0x88 - 0x80,
    EM8051_REG_TH0 = 0x8C - 0x80,
    EM8051_REG_TL0 = 0x8A - 0x80,
    EM8051_REG_TH1 = 0x8D - 0x80,
    EM8051_REG_TL1 = 0x8B - 0x80,
    EM8051_REG_SCON = 0x98 - 0x80,
    EM8051_REG_PCON = 0x87 - 0x80
};

enum EM8051_PSW_BITS
{
    EM8051_PSW_P = 0,
    EM8051_PSW_UNUSED = 1,
    EM8051_PSW_OV = 2,
    EM8051_PSW_RS0 = 3,
    EM8051_PSW_RS1 = 4,
    EM8051_PSW_F0 = 5,
    EM8051_PSW_AC = 6,
    EM8051_PSW_C = 7
};

enum EM8051_PSW_MASKS
{
    EM8051_PSWMASK_P = 0x01,
    EM8051_PSWMASK_UNUSED = 0x02,
    EM8051_PSWMASK_OV = 0x04,
    EM8051_PSWMASK_RS0 = 0x08,
    EM8051_PSWMASK_RS1 = 0x10,
    EM8051_PSWMASK_F0 = 0x20,
    EM8051_PSWMASK_AC = 0x40,
    EM8051_PSWMASK_C = 0x80
};

enum EM8051_IE_MASKS
{
    EM8051_IEMASK_EX0 = 0x01,
    EM8051_IEMASK_ET0 = 0x02,
    EM8051_IEMASK_EX1 = 0x04,
    EM8051_IEMASK_ET1 = 0x08,
    EM8051_IEMASK_ES  = 0x10,
    EM8051_IEMASK_ET2 = 0x20,
    EM8051_IEMASK_UNUSED = 0x40,
    EM8051_IEMASK_EA  = 0x80
};

enum EM8051_PT_MASKS
{
    EM8051_PTMASK_PX0 = 0x01,
    EM8051_PTMASK_PT0 = 0x02,
    EM8051_PTMASK_PX1 = 0x04,
    EM8051_PTMASK_PT1 = 0x08,
    EM8051_PTMASK_PS  = 0x10,
    EM8051_PTMASK_PT2 = 0x20,
    EM8051_PTMASK_UNUSED1 = 0x40,
    EM8051_PTMASK_UNUSED2 = 0x80
};

enum EM8051_TCON_MASKS
{
    EM8051_TCONMASK_IT0 = 0x01,
    EM8051_TCONMASK_IE0 = 0x02,
    EM8051_TCONMASK_IT1 = 0x04,
    EM8051_TCONMASK_IE1 = 0x08,
    EM8051_TCONMASK_TR0 = 0x10,
    EM8051_TCONMASK_TF0 = 0x20,
    EM8051_TCONMASK_TR1 = 0x40,
    EM8051_TCONMASK_TF1 = 0x80
};

enum EM8051_TMOD_MASKS
{
    EM8051_TMODMASK_M0_0 = 0x01,
    EM8051_TMODMASK_M1_0 = 0x02,
    EM8051_TMODMASK_CT_0 = 0x04,
    EM8051_TMODMASK_GATE_0 = 0x08,
    EM8051_TMODMASK_M0_1 = 0x10,
    EM8051_TMODMASK_M1_1 = 0x20,
    EM8051_TMODMASK_CT_1 = 0x40,
    EM8051_TMODMASK_GATE_1 = 0x80
};

enum EM8051_IP_MASKS
{
    EM8051_IPMASK_PX0 = 0x01,
    EM8051_IPMASK_PT0 = 0x02,
    EM8051_IPMASK_PX1 = 0x04,
    EM8051_IPMASK_PT1 = 0x08,
    EM8051_IPMASK_PS  = 0x10,
    EM8051_IPMASK_PT2 = 0x20
};

enum EM8051_EXCEPTION
{
    EM8051_EXCEPTION_STACK,  // stack address > 127 with no upper memory, or roll over
    EM8051_EXCEPTION_ACC_TO_A, // acc-to-a move operation; illegal (acc-to-acc is ok, a-to-acc is ok..)
    EM8051_EXCEPTION_IRET_PSW_MISMATCH, // psw not preserved over interrupt call (doesn't care about P, F0 or UNUSED)
    EM8051_EXCEPTION_IRET_SP_MISMATCH,  // sp not preserved over interrupt call
    EM8051_EXCEPTION_IRET_ACC_MISMATCH, // acc not preserved over interrupt call
    EM8051_EXCEPTION_ILLEGAL_OPCODE     // for the single 'reserved' opcode in the architecture
};

