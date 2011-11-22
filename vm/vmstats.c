#include "opt-A3.h"
#if OPT_A3

/* belongs in kern/vm/vmstats.c */

/* NOTE !!!!!! WARNING !!!!!
 * All of the functions whose names begin with '_'
 * assume that atomicity is ensured elsewhere
 * (i.e., outside of these routines).
 * All of the functions whose names do not begin
 * with '_' ensure atomicity locally.
 *
 * You may need to be careful in choosing which 
 * version to use and when.
 */

#include <types.h>
#include <lib.h>
#include <synch.h>
#include <machine/spl.h>
#include "vmstats.h"
#include <addrspace.h>

/* Counters for tracking statistics */
static unsigned int stats_counts[VMSTAT_COUNT];

static struct lock *stats_lock = 0;

/* Strings used in printing out the statistics */
static const char *stats_names[] = {
 /* 0 */ "TLB Faults", 
 /* 1 */ "TLB Faults with Free",
 /* 2 */ "TLB Faults with Replace",
 /* 3 */ "TLB Invalidations",
 /* 4 */ "TLB Reloads",
 /* 5 */ "Page Faults (Zeroed)",
 /* 6 */ "Page Faults (Disk)",
};


/* ---------------------------------------------------------------------- */
/* Assumes vmstat_init has already been called */
void
vmstats_inc(unsigned int index)
{
  /* If interrupts are already off we might be in an interrupt
   * handler and we don't need to and can not use a lock
   */
  if (curspl == SPL_HIGH) {
    _vmstats_inc(index);
  } else {
    /* simple check that vmstat_init has been called */
    assert(stats_lock);
    lock_acquire(stats_lock);
      _vmstats_inc(index);
    lock_release(stats_lock);
  }
}

/* ---------------------------------------------------------------------- */
void
vmstats_init()
{
  /* Ensure this only gets called once */
  assert(stats_lock == 0);
  stats_lock = lock_create("StatsLock");
  assert(stats_lock != 0);

  lock_acquire(stats_lock);
    _vmstats_init();
  lock_release(stats_lock);
}

/* ---------------------------------------------------------------------- */
/* Assumes vmstat_init has already been called */
void
vmstats_print()
{
  /* simple check that vmstat_init has been called */
  assert(stats_lock);
  lock_acquire(stats_lock);
    _vmstats_print();
  lock_release(stats_lock);
}

/* ---------------------------------------------------------------------- */
void
_vmstats_inc(unsigned int index)
{
  assert(index < VMSTAT_COUNT);
  stats_counts[index]++;
}

/* ---------------------------------------------------------------------- */
void
_vmstats_init()
{
  int i = 0;

  if (sizeof(stats_names) / sizeof(char *) != VMSTAT_COUNT) {
    kprintf("vmstats_init: number of stats_names = %d != VMSTAT_COUNT = %d\n",
      (sizeof(stats_names) / sizeof(char *)), VMSTAT_COUNT);
    panic("Should really fix this before proceeding\n");
  }

  for (i=0; i<VMSTAT_COUNT; i++) {
   
	if (i == 0) {
	 stats_counts[i] = tlbfault;
	}
	
	else if (i == 1) {
	  stats_counts[i] = tlbfree;
	}
	
	else if (i == 2) {
	  stats_counts[i] = tlbreplace;
	}
	
	else if (i == 3) {
	  stats_counts[i] = tlbinvalid; 
	}
	
	else {
	   stats_counts[i] = 0;
	}

  }

}

/* ---------------------------------------------------------------------- */
void
_vmstats_print()
{
  int i = 0;
  int free_plus_replace = 0;
  int disk_plus_zeroed_plus_reload = 0;
  int tlb_faults = 0;

  kprintf("VMSTATS:\n");
  for (i=0; i<VMSTAT_COUNT; i++) {
    kprintf("VMSTAT %25s = %10d\n", stats_names[i], stats_counts[i]);
  }

  tlb_faults = stats_counts[VMSTAT_TLB_FAULT];
  free_plus_replace = stats_counts[VMSTAT_TLB_FAULT_FREE] + stats_counts[VMSTAT_TLB_FAULT_REPLACE];
  disk_plus_zeroed_plus_reload = stats_counts[VMSTAT_PAGE_FAULT_DISK] +
    stats_counts[VMSTAT_PAGE_FAULT_ZERO] + stats_counts[VMSTAT_TLB_RELOAD];

  kprintf("VMSTAT TLB Faults with Free + TLB Faults with Replace = %d\n", free_plus_replace);
  if (tlb_faults != free_plus_replace) {
    kprintf("WARNING: TLB Faults (%d) != TLB Faults with Free + TLB Faults with Replace (%d)\n",
      tlb_faults, free_plus_replace); 
  }

  kprintf("VMSTAT TLB Reloads + Page Faults (Zeroed) + Page Faults (Disk) = %d\n", disk_plus_zeroed_plus_reload);
  if (tlb_faults != disk_plus_zeroed_plus_reload) {
    kprintf("WARNING: TLB Faults (%d) != TLB Reloads + Page Faul (Zeroed) + Page Faults (Disk) (%d)\n",
      tlb_faults, disk_plus_zeroed_plus_reload); 
  }

}
/* ---------------------------------------------------------------------- */

#endif /* OPT_A3 */
