#ifndef __LINUX_ACCELERATION_H
#define __LINUX_ACCELERATION_H
#include <linux/list.h>

struct dev_acceleration {
	int x; /* acceleration along X-axis */
	int y; /* acceleration along Y-axis */
	int z; /* acceleration along Z-axis */
};

struct acceleration_list {
	struct dev_acceleration acc;
	struct list_head list;
};

/*Define the noise*/
#define NOISE 10


/*Define the window*/
#define WINDOW 20


struct motion_event {
	int event_id;
	struct list_head list;
	wait_queue_head_t waitq;
	spin_lock_t event_lock;
	int waitq_n;
	bool happened;
	struct acc_motion *baseline;
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



#endif /* __LINUX_ACCELERATION_H */
