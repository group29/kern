
#include <types.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <lib.h>
#include <vfs.h>
#include <vm.h>
#include <test.h>
#include <vnode.h>
#include <thread.h>
#include <array.h>
#include "userfile.h"
#include "opt-A2.h"
#if OPT_A2

int sys_write(int filehandle, const void *buf, size_t size) {
//dirty implementation
char *b = buf;
	putch(b[0]);
	
	return 0;
}

/*
void sys__exit(int exitcode) {

//set process info appropriately , "clean out"
thread_exit();

}



int sys_open(const char *filename, int flagnumber){
	//value may use in the function
	int i = 0;
	int opsit;
	char * remname;	
	//define our descriptor structure 
	struct filedescriptor *myfile = kmalloc(sizeof(struct filedescriptor));
	//vnode define
	sturct vnode* vn;
	//copy our filename to remname cuz vfs_open will destory our filename
	remname = kstrdup(filename);
	
	// open a file 
	opsit = vfs_open(filename, flagnumber , &vn);
	if(opsit){
		return opsit;
	}
	myfile->fn=remname;
	myfile->vp=vn;
	myfile->flag=flagbumber;		
	myfile->offset=0;
	
	//add ur struct to ur thread array which is thr filetable we pass in
	//return the ID which is the index i
	return addID(curthread->t_filetable,myfile);
			
	
}
*/

/*
int sys_fork(void) {

	int copyResult;
	struct thread *parent; // the parent process
	struct thread *child; // the child process
	vaddr_t stackptr; // the stackptr which points to the child's address space

	parent = curthread;
	child = thread_create(curthread->t_name); // create the child thread
	if (child == NULL){ 
		return (-1);  
	}
		
	// set memory in child address space
	child->t_stack[0]= 0xae;
	child->t_stack[1] = 0x11;
	child->t_stack[2]= 0xda;
	child->t_stack[3] = 0x33;
	
	// make stackptr point to child address space 
	stackptr = child->t_vmspace;
	// copy the parent address space to child's addresspace so there are
 	// seperate address spaces both parent and child
	copyResult = copyin(curthread->t_vmspace, &child->t_vmspace);
	
	// if copy is unsuccessful then return -1
	if (copyResult) {
		return (-1);
	}

} */
#endif
