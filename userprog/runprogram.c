/*
 * Sample/test code for running a user program.  You can use this for
 * reference when implementing the execv() system call. Remember though
 * that execv() needs to do more than this function does.
 */

#include <types.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <lib.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <vm.h>
#include <vfs.h>
#include <test.h>
#include "opt-A2.h"

/*
 * Load program "progname" and start running it in usermode.
 * Does not return except on error.
 *
 * Calls vfs_open on progname and thus may destroy it.
 */
 
#if OPT_A2
int
runprogram(char *progname, int argc, char **argv)
#else
int
runprogram(char *progname)
#endif
{
	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result;

	/* Open the file. */
	result = vfs_open(progname, O_RDONLY, &v);
	if (result) {
		return result;
	}

	/* We should be a new thread. */
	assert(curthread->t_vmspace == NULL);

	/* Create a new address space. */
	curthread->t_vmspace = as_create();
	if (curthread->t_vmspace==NULL) {
		vfs_close(v);
		return ENOMEM;
	}

	/* Activate it. */
	as_activate(curthread->t_vmspace);

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		vfs_close(v);
		return result;
	}

	/* Done with the file now. */
	vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(curthread->t_vmspace, &stackptr);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		return result;
	}
#if OPT_A2
	 //stackptr at 0x80000000
	 int counter = 0, length; //length of each ar
	 int *copied; //to know how much copied for copyoutstr
	 while (counter<argc) {
		length = strlen(argv[counter]) + 1;

		stackptr = stackptr - length; //move stackptr back
		copyoutstr(argv[counter], stackptr, length, copied); //copy string into pos
		
		counter++;
	 }
	 //at this point, image of user stack:
	 /*
	 stackptr: 
	 argx arg(x-1) arg(x-2)...arg1 arg0 where x = argc - 1
	 */
	 vaddr_t track = stackptr;  //record where it is (to iterate)
	 
	 stackptr = stackptr - (stackptr % 4);
	 
	 stackptr = stackptr - 4;
	 copyout (NULL, stackptr, 4);  //store NULL in the end
	 
	
		 
	 //now make pointers
	 counter = 0;
	 while (counter < argc) {
	 stackptr = stackptr - 4;
	 copyout (&track, stackptr, 4);
	 track = track + strlen(argv[argc - counter - 1]) + 1;
	 counter++;
	 }
	 vaddr_t * user_stack = stackptr;
	stackptr = stackptr - (stackptr % 8);
	 /*
	 kprintf("arg3: %s", stackptr);
	 kprintf("arg2: %s", stackptr + strlen(argv[2]) + 1);
	 kprintf("arg1: %s", stackptr + strlen(argv[2]) + 1 + strlen(argv[1]) + 1);
	 */
	// md_usermode(argc, user_stack, stackptr, entrypoint);
	 md_usermode(argc, user_stack, stackptr, entrypoint);
#else
	/* Warp to user mode. */
	md_usermode(0 /*argc*/, NULL /*userspace addr of argv*/,
		    stackptr, entrypoint);
#endif
	
	/* md_usermode does not return */
	panic("md_usermode returned\n");
	return EINVAL;
}

