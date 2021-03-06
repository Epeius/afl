/*
 * afl-parrel-qemu.h
 *
 *  Created on: 2016年4月25日
 *      Author: epeius
 */

#ifndef AFL_PARREL_QEMU_H_
#define AFL_PARREL_QEMU_H_

#include "SegSynchronizationWrapper.h"

#define glue(x, y) x ## y
#define PARAL_QEMU(name) glue(parallel_qemu_, name)

// Qemu queue as a FIFO file
#define QEMUQUEUE "/tmp/afl_qemu_queue"

// Read 512 bytes from FIFO each time
#define FIFOBUFFERSIZE 512

// Synchronization ID
#define SMKEY 0x200

// Share memory ID
#define READYSHMID 1234

// Wait for all the qemus until they are all free
#define WAIT_ALLQEMUS_FREE \
      do { \
        int i = 0; \
        while (i < parallel_qemu_num) { \
            if(ReadArray[allQemus[i].pid]) \
                i++; \
            else{ \
                i = 0; \
                sleep(0.001); \
            } \
        } \
      }while(0)

// Every control pipe
/*
 * FIXME: Each qemu wants to have a unique control pipe, so PIPE fd should have relationship with qemu's pid.
 * While as pid can be (0, 65536), so we have to modify the file descriptor limit from default(1024) to 65536.
 */
#define CTRLPIPE(_x) (_x + 226)

/*
 * Defines for qemu instance
 * HACK: As producer is much faster than consumer, so stop time can be obtained
 * from afl-side approximately.
 */
typedef struct qemuInstance{
    u32         pid;            /* Pid of current qemu instance         */
    u8*         trace_bits;     /* Trace bits                           */
    u32         ctrl_pipe;      /* Control pipe for qemu                */
    u8*         testcaseDir;    /* Directory for testcase               */
    u64         start_us;       /* start time of a test (us)            */
    u64         stop_us;        /* Stop time of a test (us)             */
    u8*         out_file;       /* Out file in memory (free it in time) */
    u32         len;            /* Length of out file                   */
    u8          handled;        /* Whether has been handled manually    */
    void*       cur_queue;      /* Current queue file in all queues     */
    u8          cur_stage;      /* Which stage we are in                */
    u8          fault;          /* Fault type                           */
}QemuInstance;


// Set up ready share memory for qemu and afl.
void PARAL_QEMU(SetupSHM4Ready)(void);

// Initial QEMU ready FIFO (NOTE: use FIFO to speed up efficiency).
void PARAL_QEMU(InitQemuQueue) (void);

// Set up trace-bits bitmap for each qemu instance.
void PARAL_QEMU(setupTracebits) (void);


#endif /* AFL_PARREL_QEMU_H_ */
