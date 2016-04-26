/*
 * afl-parrel-qemu.c
 *
 *  Created on: 2016年4月25日
 *      Author: epeius
 */
/*
 * afl-parallel-qemu communicates with multiple qemu instance at the same time
 * to accelerate the efficiency when testing full-system software.
 */

/*************************************************************** \

 _______
|       |
| QEMU1 |
|_______|

 _______
|       |
| QEMU2 |
|_______|

 _______
|       |
| QEMU3 |
|_______|

 */

#include "config.h"
#include "afl-parrel-qemu.h"
#include "types.h"
#include "debug.h"
#include "alloc-inl.h"
#include "hash.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <dlfcn.h>
#include <assert.h>

#include <sys/wait.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/file.h>

#if defined(__APPLE__) || defined(__FreeBSD__) || defined (__OpenBSD__)
#  include <sys/sysctl.h>
#endif /* __APPLE__ || __FreeBSD__ || __OpenBSD__ */

extern u8 parallel_qemu_num;
extern QemuInstance * allQemus;
extern u32 qemu_quene_fd;

#define QEMUEXECUTABLE "/home/epeius/work/DSlab.EPFL/FinalSubmitV2/s2ebuild/qemu-release/i386-s2e-softmmu/qemu-system-i386"
char *const qemu_argv[] ={"qemu-system-i386",
        "-m", "128",
        "-net", "none",
        "-usbdevice", "tablet",
        "-monitor", "stdio",
        "-hda", "/home/epeius/work/DSlab.EPFL/FinalTest/s2ebuild/images/debian.raw.s2e",
        "-loadvm", "forkstate",
        "-s2e-config-file", "/home/epeius/work/DSlab.EPFL/FinalSubmitV2/testplace/forkstate.lua",
        NULL};

/*
 * We don't create bitmap here because we cannot synchonize well with qemu, so give this chance to qemus.
 * While control pipes could be initialed at both sides.
 */
void PARAL_QEMU(InitQemuQueue)(void)
{
    qemu_quene_fd = open(QEMUQUEUE, O_RDONLY); // we only need one queue here and set mode as read-only
    if (qemu_quene_fd == -1)
        PFATAL("Create qemu queue fifo failed.");

    if (access("/tmp/afltestcase", F_OK)) // for all testcases
        if (mkdir("/tmp/afltestcase", 0777))
            PFATAL("mkdir() failed");
    if (access("/tmp/afltracebits", F_OK)) // for all trace-bits bitmaps
        if (mkdir("/tmp/afltracebits", 0777))
            PFATAL("mkdir() failed");

    u8 i = 0;
    allQemus = (QemuInstance*) malloc(parallel_qemu_num * sizeof(QemuInstance));
    while (i < parallel_qemu_num) {
        pid_t pid = fork();
        if (pid < 0)
            PFATAL("fork() failed");
        if (!pid) {
            execv(QEMUEXECUTABLE, qemu_argv);
        } else {
            allQemus[i].pid = pid;
            u8* _tcDir = (u8*) malloc(128);
            sprintf(_tcDir, "/tmp/afltestcase/%d/", pid);
            mkdir(_tcDir, 0777);
            allQemus[i].testcaseDir = _tcDir;
            // set up control pipe
            int fd[2];
            if (pipe(fd) != 0)
                PFATAL("pipe() failed");
            if (dup2(fd[1], CTRLPIPE(pid) + 1) < 0
                    || dup2(fd[0], CTRLPIPE(pid)) < 0)
                PFATAL("dup2() failed");
            allQemus[i].ctrl_pipe = CTRLPIPE(pid) + 1;
            close(fd[0]); // afl doesn't need read control pipe anymore.
        }
        i++;
        sleep(2); // why not sleep for a while.
    }
}

void PARAL_QEMU(setupTracebits) (void)
{
    u8 i = 0;
    assert(allQemus);
    while (i < parallel_qemu_num) {
        key_t shmkey;
        u8* _shmfile = (u8*) malloc(128);
        sprintf(_shmfile, "/tmp/afltracebits/trace_%d", allQemus[i].pid);
        if ((shmkey = ftok(_shmfile, 1)) < 0) {
            free(_shmfile);
            printf("ftok error:%s\n", strerror(errno));
            PFATAL("ftok() failed");
        }
        free(_shmfile);
        int shm_id = shmget(shmkey, MAP_SIZE, IPC_CREAT | 0600);
        if (shm_id < 0)
            PFATAL("shmget() failed");

        void * __tracebits = shmat(shm_id, NULL, 0);
        if (__tracebits == (void*) -1)
            PFATAL("shmat() failed");

        allQemus[i].trace_bits = (u8*) __tracebits;
        i++;
    }
}

