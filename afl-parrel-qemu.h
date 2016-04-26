/*
 * afl-parrel-qemu.h
 *
 *  Created on: 2016年4月25日
 *      Author: epeius
 */

#ifndef AFL_PARREL_QEMU_H_
#define AFL_PARREL_QEMU_H_

#define glue(x, y) x ## y
#define PARAL_QEMU(name) glue(parallel_qemu_, name)

// Qemu queue as a FIFO file
#define QEMUQUEUE "/tmp/afl_qemu_queue"

// Read 512 bytes from FIFO each time
#define FIFOBUFFERSIZE 512

// Every control pipe
#define CTRLPIPE(_x) (_x + 226)

// Defines for qemu instance
typedef struct qemuInstance{
    u32 pid;            /* Pid of current qemu instance */
    u8* trace_bits;     /* Trace bits */
    u32 ctrl_pipe;      /* Control pipe for qemu */
    u8* testcaseDir;    /* Directory for testcase */
}QemuInstance;

// Initial QEMU ready FIFO (NOTE: use FIFO to speed up efficiency)
void PARAL_QEMU(InitQemuQueue) (void);

// Set up trace-bits bitmap for each qemu instance
void PARAL_QEMU(setupTracebits) (void);


#endif /* AFL_PARREL_QEMU_H_ */
