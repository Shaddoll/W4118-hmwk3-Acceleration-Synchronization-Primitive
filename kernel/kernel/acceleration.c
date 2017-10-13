#include <linux/syscalls.h>
#include <asm-generic/errno-base.h>
#include <linux/acceleration.h>

struct dev_acceleration acc;

int do_set_acceleration(struct dev_acceleration __user *acceleration)
{
	int ret = 0;

	if (acceleration == NULL)
		return -EINVAL;
	ret = copy_from_user(&acc, acceleration, sizeof(acceleration));
	return ret ? -EFAULT : 0;
}

SYSCALL_DEFINE1(set_acceleration, struct dev_acceleration __user *, accel)
{
	return do_set_acceleration(accel);
}