#ifndef _ACCELERATIOND_H
#define _ACCELERATIOND_H


#define __NR_set_acceleration 249
#define __NR_accevt_create 250
#define __NR_accevt_wait 251
#define __NR_accevt_signal 252
#define __NR_accevt_destroy 253

#define TIME_INTERVAL  200


struct dev_acceleration {
	int x; /* acceleration along X-axis */
	int y; /* acceleration along Y-axis */
	int z; /* acceleration along Z-axis */
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
			     * sum_each_sample(dlt_x+dlt_y+dlt_z) > NOISE
			     */
};

#endif
