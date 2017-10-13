#ifndef __LINUX_ACCELERATION_H
#define __LINUX_ACCELERATION_H
#include <linux/list.h>
struct dev_acceleration {
	int x; /* acceleration along X-axis */
	int y; /* acceleration along Y-axis */
	int z; /* acceleration along Z-axis */
};




/*Define the noise*/
#define NOISE 10


/*Define the window*/
#define WINDOW 20


struct motion_events {
	int event_id;
	struct list_head list;
	wait_queue_head_t wait_queue;
};


/*
* Define the motion.
* The motion give the baseline for an EVENT.
*/
struct acc_motion {

     unsigned int dlt_x; /* +/- around X-axis */
     unsigned int dlt_y; /* +/- around Y-axis */
     unsigned int dlt_z; /* +/- around Z-axis */
     
     unsigned int frq;   /* Number of samples that satisfies:
                          sum_each_sample(dlt_x + dlt_y + dlt_z) > NOISE */
};




/* Create an event based on motion.  
 * If frq exceeds WINDOW, cap frq at WINDOW.
 * Return an event_id on success and the appropriate error on failure.
 * system call number 250
 */
 
int accevt_create(struct acc_motion __user *acceleration);



/* Block a process on an event.
 * It takes the event_id as parameter. The event_id requires verification.
 * Return 0 on success and the appropriate error on failure.
 * system call number 251
 */

int accevt_wait(int event_id);



/* The acc_signal system call
 * takes sensor data from user, stores the data in the kernel,
 * generates a motion calculation, and notify all open events whose
 * baseline is surpassed.  All processes waiting on a given event 
 * are unblocked.
 * Return 0 success and the appropriate error on failure.
 * system call number 252
 */

int accevt_signal(struct dev_acceleration __user * acceleration);



/* Destroy an acceleration event using the event_id,
 * Return 0 on success and the appropriate error on failure.
 * system call number 253
 */

int accevt_destroy(int event_id);


#endif /* __LINUX_ACCELERATION_H */
