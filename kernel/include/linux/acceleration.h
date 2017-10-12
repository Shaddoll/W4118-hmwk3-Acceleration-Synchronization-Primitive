#ifndef __LINUX_ACCELERATION_H
#define __LINUX_ACCELERATION_H

struct dev_acceleration {
	int x; /* acceleration along X-axis */
	int y; /* acceleration along Y-axis */
	int z; /* acceleration along Z-axis */
};

#endif /* __LINUX_ACCELERATION_H */
