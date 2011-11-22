#include "opt-A2.h"

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);

#if OPT_A2
int sys_write(int filehandle, const void *buf, size_t size);
void sys__exit (int exitcode);
int waitpid(pid_t pid, int *status, int options);
int sys_open(const char *filename, int flagnumber);
#endif /* opt_a2 */

#endif /* _SYSCALL_H_ */
