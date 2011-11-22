/*
 * Synchronization primitives.
 * See synch.h for specifications of the functions.
 */
#include "opt-A1.h"
#include <types.h>
#include <lib.h>
#include <synch.h>
#include <thread.h>
#include <curthread.h>
#include <machine/spl.h>


////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *namearg, int initial_count)
{
	struct semaphore *sem;

	assert(initial_count >= 0);

	sem = kmalloc(sizeof(struct semaphore));
	if (sem == NULL) {
		return NULL;
	}

	sem->name = kstrdup(namearg);
	if (sem->name == NULL) {
		kfree(sem);
		return NULL;
	}

	sem->count = initial_count;
	return sem;
}

void
sem_destroy(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	spl = splhigh();
	assert(thread_hassleepers(sem)==0);
	splx(spl);

	/*
	 * Note: while someone could theoretically start sleeping on
	 * the semaphore after the above test but before we free it,
	 * if they're going to do that, they can just as easily wait
	 * a bit and start sleeping on the semaphore after it's been
	 * freed. Consequently, there's not a whole lot of point in 
	 * including the kfrees in the splhigh block, so we don't.
	 */

	kfree(sem->name);
	kfree(sem);
}

void 
P(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	assert(in_interrupt==0);

	spl = splhigh();
	while (sem->count==0) {
		thread_sleep(sem);
	}
	assert(sem->count>0);
	sem->count--;
	splx(spl);
}

void
V(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);
	spl = splhigh();
	sem->count++;
	assert(sem->count>0);
	thread_wakeup(sem);
	splx(spl);
}

////////////////////////////////////////////////////////////
//
// Lock.

struct lock *lock_create(const char *name)
{


	struct lock *lock;

	lock = kmalloc(sizeof(struct lock));
	if (lock == NULL) {
		return NULL;
	}

	lock->name = kstrdup(name);
	if (lock->name == NULL) {
		kfree(lock);
		return NULL;
	}
	lock->lock_thread==NULL;
	//assert(lock->lock_thread==NULL);

	
// add stuff here as needed
#if OPT_A1
	lock->lock_thread=NULL; 	//no thread holding it
#endif
	
	
	return lock;
}

void
lock_destroy(struct lock *lock)
{
	assert(lock != NULL);

	// add stuff here as needed
#if OPT_A1
	assert(lock->lock_thread==NULL);		//ensure no one is holding the lock
	
#endif
	
	kfree(lock->name);
	kfree(lock);
}

void
lock_acquire(struct lock *lock)
{
// Write this
#if OPT_A1
	int spl;
	assert(lock != NULL);  //ensure lock is created
	assert(in_interrupt==0);  //just like p() of semaphore
	spl=splhigh(); 		//disable interrupts
	while (lock->lock_thread != NULL) {	//loop while lock_thread is taken (locked)
		thread_sleep(lock);			//sleep
	}			//exit on lock_thread null
	assert(lock->lock_thread == NULL); //assert, ensure that it is indeed unlocked
	lock->lock_thread=curthread; 	//current thread acquired it
	splx(spl);
#endif

	(void)lock;  // suppress warning until code gets written
}


 
void
lock_release(struct lock *lock)
{
// Write this
#if OPT_A1
	int spl;
	assert (lock!=NULL); 	//assert lock is not null
	spl = splhigh();		//disable interrupts
	assert (lock->lock_thread == curthread); 	//ensure only holding thread is releasing
	lock->lock_thread=NULL;		//make it "unlocked"
	assert(lock->lock_thread==NULL);	//confirm
	thread_wakeup(lock); 		//call thread_wakeup, and allow other threads to enter
	splx(spl);		//return interrupt state to previous
#endif

	(void)lock;  // suppress warning until code gets written
}

int
lock_do_i_hold(struct lock *lock)
{

// Write this
#if OPT_A1
	int spl;
	spl = splhigh(); 	//disable interrupts
	assert (lock!=NULL);
	splx(spl);		//reenable interrupts
	return (lock->lock_thread == curthread);  //check if curthread holding the lock
	
#else
	return 1;    // dummy until code gets written
	
#endif

	(void)lock;  // suppress warning until code gets written

}

////////////////////////////////////////////////////////////
//
// CV


struct cv *
cv_create(const char *name)
{
	struct cv *cv;

	cv = kmalloc(sizeof(struct cv));
	if (cv == NULL) {
		return NULL;
	}

	cv->name = kstrdup(name);
	if (cv->name==NULL) {
		kfree(cv);
		return NULL;
	}
	
	// add stuff here as needed
	//nothing added for a1
	return cv;
}

void
cv_destroy(struct cv *cv)
{
	assert(cv != NULL);

	// add stuff here as needed
#if OPT_A1

	int spl;
	assert(cv != NULL);		//ensure not empty / null

	spl = splhigh();		//disable interrupts	
	assert(thread_hassleepers(cv)==0);		//assume / assert no threads sleeping on cv
	splx(spl);		//reenable interrupts

#endif
	
	kfree(cv->name);
	kfree(cv);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{

// Write this
#if OPT_A1
	assert (lock->lock_thread == curthread); 		//ensure current thread holds lock
	int spl;		
	spl=splhigh();			//disable interrupts
	lock_release(lock);		//release lock
	thread_sleep(cv);		//thread sleeps on address cv
	lock_acquire(lock);		//re-acquire lock
	splx(spl);
#endif


	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
	// Write this
#if OPT_A1
	int spl;
	spl=splhigh();		//disable interrupts
	thread_wakeupone(cv);		//wake up one thread
	splx(spl);		//reenable interrupts
#endif

	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	// Write this
#if OPT_A1
	int spl;
	spl=splhigh();		//disable interrupts
	thread_wakeup(cv);		//wake up all threads on address cv
	splx(spl);		//reenable interrupts
#endif

	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}
