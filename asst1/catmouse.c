/*
 * catmouse.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 * 26-11-2007: KMS : Modified to use cat_eat and mouse_eat
 * 21-04-2009: KMS : modified to use cat_sleep and mouse_sleep
 * 21-04-2009: KMS : added sem_destroy of CatMouseWait
 *
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>
#include "opt-A1.h"

/*
 * 
 * cat,mouse,bowl simulation functions defined in bowls.c
 *
 * For Assignment 1, you should use these functions to
 *  make your cats and mice eat from the bowls.
 * 
 * You may *not* modify these functions in any way.
 * They are implemented in a separate file (bowls.c) to remind
 * you that you should not change them.
 *
 * For information about the behaviour and return values
 *  of these functions, see bowls.c
 *
 */

/* this must be called before any calls to cat_eat or mouse_eat */
extern int initialize_bowls(unsigned int bowlcount);

extern void cat_eat(unsigned int bowlnumber);
extern void mouse_eat(unsigned int bowlnumber);
extern void cat_sleep(void);
extern void mouse_sleep(void);

#if OPT_A1

/*array of bowls */
static char *bowl;		//O if occupied, E if empty

/* how many cats are currently eating? */
static int eating_c_count;

/* how many mice are currently eating? */
static int eating_m_count;

/*how many mice are waiting */
static int waiting_mice_count;

/*how many cats are waiting */
static int waiting_cats_count;

static struct lock *mainlock;		//lock 

static struct cv *c_sleep;		//cv to make cat wait / signal
static struct cv *m_sleep;		//cv to make mouse wait / signal 

#endif


/*
 *
 * Problem parameters
 *
 * Values for these parameters are set by the main driver
 *  function, catmouse(), based on the problem parameters
 *  that are passed in from the kernel menu command or
 *  kernel command line.
 *
 * Once they have been set, you probably shouldn't be
 *  changing them.
 *
 *
 */
int NumBowls;  // number of food bowls
int NumCats;   // number of cats
int NumMice;   // number of mice
int NumLoops;  // number of times each cat and mouse should eat

/*
 * Once the main driver function (catmouse()) has created the cat and mouse
 * simulation threads, it uses this semaphore to block until all of the
 * cat and mouse simulations are finished.
 */
struct semaphore *CatMouseWait;

/*
 * 
 * Function Definitions
 * 
 */


/*
 * cat_simulation()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds cat identifier from 0 to NumCats-1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Each cat simulation thread runs this function.
 *
 *      You need to finish implementing this function using 
 *      the OS161 synchronization primitives as indicated
 *      in the assignment description
 */
 
 


#if OPT_A1

int initialize(unsigned int bowlcount)
{
  unsigned int i;

  if (bowlcount == 0) {
    kprintf("initialize_bowls: invalid bowl count %d\n",bowlcount);
    return 1;
  }  //if no bowls

  bowl = kmalloc(bowlcount*sizeof(char));	//allocate bowls array
  
  if (bowl == NULL) {		//if failed to allocate
    panic("initialize_bowls: unable to allocate space for %d bowls\n",bowlcount);
  }
  /* initialize bowls */
  
  for(i=0;i<bowlcount;i++) {		//initialize bowl array
    bowl[i] = 'E';
  }
  
  eating_c_count = eating_m_count = waiting_mice_count = waiting_cats_count = 0;	//initialize all four globals to 0
    
  mainlock=lock_create("MainLock");		//create main lock
  
  c_sleep= cv_create("CatSleep");		//condition variable, puts cat to sleep when cat can't eat
  m_sleep= cv_create("MouseSleep");		//condition variable, puts mouse to sleep when mouse can't eat
  
  return 0;	//no error
}
 


void helper (int *a, struct cv *cv, struct lock *lock)	//helper function
{
*a = (*a) + 1;			//increase count of cats waiting list
		cv_wait(cv, lock);		//call cv_wait on cv -> cat_sleep
		*a = (*a) - 1;		//decrement again, not waiting anymore


}

int search (char *x)		//searches an array of bowls to find first empty ('E')
{
	int i=0;
	while (i < NumBowls)		//search through all bowls
	{
		if (x[i]=='E')		//found empty bowl at X
			return i;
	i++;
	}

}

#endif

#if OPT_A1

static void cat_simulation(void * unusedpointer, unsigned long catnumber)
{
  
	int i;
	
  /* avoid unused variable warnings. */
  (void) unusedpointer;
  (void) catnumber;


  /* your simulated cat must iterate NumLoops times,
   *  sleeping (by calling cat_sleep() and eating
   *  (by calling cat_eat()) on each iteration */
   
  for(i=0;i<NumLoops;i++) {

	//cat_sleep();
	

	lock_acquire(mainlock);		//acquire main lock
	
	while (eating_m_count > 0 || eating_c_count == NumBowls) {		//if mouse present or if all bowls taken
		helper (&waiting_cats_count, c_sleep, mainlock);  //increase cat wait count, call cv, decrease cat wait count
		
	}
	int x = search(bowl);		//find empty spot in bowl
	
	bowl[x]='O';		//mark as occupied
	eating_c_count++;		//going to eat
	
	lock_release(mainlock);		//release main lock
	x++;
	cat_eat(x);		//cat_eat, x+1 cuz starts from 1 (not 0)
	x--;
	lock_acquire(mainlock);		//acquire main lock again
	eating_c_count--;			//reduce c_count by 1, since cat just came back
	bowl[x]='E';		//mark as empty
	
	
		if (eating_c_count==0 && waiting_mice_count > 0)	//if all bowls empty and mice waiting
		{
			cv_broadcast(m_sleep, mainlock);		//broadcast to mice
		}
		
		if (waiting_cats_count > waiting_mice_count) {		//if more cats waiting
			cv_broadcast(c_sleep, mainlock);			//wake up cats
		}
		else if (waiting_mice_count > waiting_cats_count) {		//if more mice waiting
			cv_broadcast(m_sleep, mainlock);		//wake up mice
		}
		else {				//if equal	
			unsigned int k=  (unsigned int)random() % 2;		//chose random int, 0 or 1
			if (k==0) {		//if 0, then broadcast to mice
				if (waiting_mice_count != 0)
					cv_broadcast(m_sleep, mainlock);
			}
			else {
				if (waiting_cats_count != 0)
					cv_broadcast(c_sleep, mainlock);		//else broadcast to cats
			}
				
		}
		
		
			
		
	
	
	lock_release(mainlock);  //release mainlock
	
  
  
  
  
  }

  /* indicate that this cat simulation is finished */
  V(CatMouseWait); 
}

#else


   /* avoid unused variable warnings. */
   (void) unusedpointer;
   (void) catnumber;
 
 
   /* your simulated cat must iterate NumLoops times,
    *  sleeping (by calling cat_sleep() and eating
    *  (by calling cat_eat()) on each iteration */
   for(i=0;i<NumLoops;i++) {
 
     /* do not synchronize calls to cat_sleep().
        Any number of cats (and mice) should be able
        sleep at the same time. */
     cat_sleep();
 
     /* for now, this cat chooses a random bowl from
      * which to eat, and it is not synchronized with
      * other cats and mice.
      *
      * you will probably want to control which bowl this
      * cat eats from, and you will need to provide 
      * synchronization so that the cat does not violate
      * the rules when it eats */
 
     /* legal bowl numbers range from 1 to NumBowls */
     bowl = ((unsigned int)random() % NumBowls) + 1;
     cat_eat(bowl);
 
   }

#endif

	
/*
 * mouse_simulation()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long mousenumber: holds mouse identifier from 0 to NumMice-1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      each mouse simulation thread runs this function
 *
 *      You need to finish implementing this function using 
 *      the OS161 synchronization primitives as indicated
 *      in the assignment description
 *
 */
 


#if OPT_A1
static
void
mouse_simulation(void * unusedpointer,
          unsigned long mousenumber)
{
  int i;
  //x is used to find empty bowl
  //unsigned int bowl;

  /* Avoid unused variable warnings. */
  (void) unusedpointer;
  (void) mousenumber;


  /* your simulated mouse must iterate NumLoops times,
   *  sleeping (by calling mouse_sleep()) and eating
   *  (by calling mouse_eat()) on each iteration */
  for(i=0;i<NumLoops;i++) {

	//mouse_sleep();
	
	lock_acquire(mainlock);		//acquire main lock
	
	while (eating_c_count > 0 || eating_m_count == NumBowls) {		//if mouse present or if all bowls taken
		helper(&waiting_mice_count, m_sleep, mainlock); //increase mice wait count, call cv, decrease cat wait count
		
	}

	int x= search(bowl);	//search the array
	
	
		
	bowl[x]='O';		//mark as occupied
	eating_m_count++;		//going to eat
	
	lock_release(mainlock);		//release main lock
	x++;
	mouse_eat(x);		//eat at x+1 (starts at 1, isntead of 0)
	x--;
	lock_acquire(mainlock);		//get mainlock again
	eating_m_count--;		//lower mice count by 1
	bowl[x]='E';		//mark as empty 
	
	if (eating_m_count==0 && waiting_cats_count > 0)	//if all bowls empty and cats waiting
	{
		cv_broadcast(c_sleep, mainlock);		//broadcast to cats
	
	}
	
	
		if (waiting_cats_count > waiting_mice_count) {		//if more cats waiting
			cv_broadcast(c_sleep, mainlock);			//broadcast to cats
		}
		else if (waiting_mice_count > waiting_cats_count) {		//if more mice waiting
			cv_broadcast(m_sleep, mainlock);				//broadcast to mice
		}
		else {		//if equal amount waiting
			unsigned int k=  (unsigned int)random() % 2;	//random number - 0 or 1
			
			
			if (k==0) {			//if random is 0
				if (waiting_mice_count != 0)
					cv_broadcast(m_sleep, mainlock);	//broadcast to mice
			}
			else {		//else (if 1)
				if (waiting_cats_count != 0)
					cv_broadcast(c_sleep, mainlock);		//broadcast to cats
			}
				
		}
	
	lock_release(mainlock);		//release main lock


	}

  /* indicate that this mouse is finished */
  V(CatMouseWait); 
}

#else

 static
 void
 mouse_simulation(void * unusedpointer,
           unsigned long mousenumber)
 {
   int i;
   unsigned int bowl;
 
   /* Avoid unused variable warnings. */
   (void) unusedpointer;
   (void) mousenumber;
 
 
   /* your simulated mouse must iterate NumLoops times,
    *  sleeping (by calling mouse_sleep()) and eating
    *  (by calling mouse_eat()) on each iteration */
   for(i=0;i<NumLoops;i++) {
 
     /* do not synchronize calls to mouse_sleep().
        Any number of mice (and cats) should be able
        sleep at the same time. */
     mouse_sleep();
 
     /* for now, this mouse chooses a random bowl from
      * which to eat, and it is not synchronized with
      * other cats and mice.
      *
      * you will probably want to control which bowl this
      * mouse eats from, and you will need to provide 
      * synchronization so that the mouse does not violate
      * the rules when it eats */
 
     /* legal bowl numbers range from 1 to NumBowls */
     bowl = ((unsigned int)random() % NumBowls) + 1;
     mouse_eat(bowl);
 
   }
 
   /* indicate that this mouse is finished */
   V(CatMouseWait); 
 }



#endif



/*
 * catmouse()
 *
 * Arguments:
 *      int nargs: should be 5
 *      char ** args: args[1] = number of food bowls
 *                    args[2] = number of cats
 *                    args[3] = number of mice
 *                    args[4] = number of loops
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up cat_simulation() and
 *      mouse_simulation() threads.
 *      You may need to modify this function, e.g., to
 *      initialize synchronization primitives used
 *      by the cat and mouse threads.
 *      
 *      However, you should should ensure that this function
 *      continues to create the appropriate numbers of
 *      cat and mouse threads, to initialize the simulation,
 *      and to wait for all cats and mice to finish.
 */

int
catmouse(int nargs,
	 char ** args)
{

  int index, error;
  int i;

  /* check and process command line arguments */
  if (nargs != 5) {
    kprintf("Usage: <command> NUM_BOWLS NUM_CATS NUM_MICE NUM_LOOPS\n");
    return 1;  // return failure indication
  }

  /* check the problem parameters, and set the global variables */
  NumBowls = atoi(args[1]);
  if (NumBowls <= 0) {
    kprintf("catmouse: invalid number of bowls: %d\n",NumBowls);
    return 1;
  }
  NumCats = atoi(args[2]);
  if (NumCats < 0) {
    kprintf("catmouse: invalid number of cats: %d\n",NumCats);
    return 1;
  }
  NumMice = atoi(args[3]);
  if (NumMice < 0) {
    kprintf("catmouse: invalid number of mice: %d\n",NumMice);
    return 1;
  }
  NumLoops = atoi(args[4]);
  if (NumLoops <= 0) {
    kprintf("catmouse: invalid number of loops: %d\n",NumLoops);
    return 1;
  }
  kprintf("Using %d bowls, %d cats, and %d mice. Looping %d times.\n",
          NumBowls,NumCats,NumMice,NumLoops);

  /* create the semaphore that is used to make the main thread
     wait for all of the cats and mice to finish */
  CatMouseWait = sem_create("CatMouseWait",0);
  if (CatMouseWait == NULL) {
    panic("catmouse: could not create semaphore\n");
  }

  /* 
   * initialize the bowls
   */
  if (initialize_bowls(NumBowls)) {
    panic("catmouse: error initializing bowls.\n");
  }
  
#if OPT_A1
	if (initialize(NumBowls)) {		//initialize everything (global variables, bowl array)
		panic("catmouse: error initializing.\n");	//error
	}
#endif

  /*
   * Start NumCats cat_simulation() threads.
   */
  for (index = 0; index < NumCats; index++) {
    error = thread_fork("cat_simulation thread",NULL,index,cat_simulation,NULL);
    if (error) {
      panic("cat_simulation: thread_fork failed: %s\n", strerror(error));
    }
  }

  
  /*
   * Start NumMice mouse_simulation() threads.
   */
  for (index = 0; index < NumMice; index++) {
    error = thread_fork("mouse_simulation thread",NULL,index,mouse_simulation,NULL);
    if (error) {
      panic("mouse_simulation: thread_fork failed: %s\n",strerror(error));
    }
  }

  /* wait for all of the cats and mice to finish before
     terminating */  
  for(i=0;i<(NumCats+NumMice);i++) {
    P(CatMouseWait);
  }

  /* clean up the semaphore the we created */
  sem_destroy(CatMouseWait);
#if OPT_A1
	kfree (bowl);		//free memory of bowl array (which was dynamically allocated
#endif
  return 0;
}

/*
 * End of catmouse.c
 */
