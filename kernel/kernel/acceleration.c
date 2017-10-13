#include <linux/syscalls.h>
#include <asm-generic/errno-base.h>
#include <linux/acceleration.h>
#include <linux/slab.h>

static DEFINE_SPINLOCK(acc_lock);
static struct dev_acceleration acc;

int do_set_acceleration(struct dev_acceleration __user *acceleration)
{
	int ret = 0;
	struct dev_acceleration temp_acc;

	if (current_euid() != 0 && current_uid() != 0)
		return -EACCES;
	if (acceleration == NULL)
		return -EINVAL;
	ret = copy_from_user(&temp_acc, acceleration, sizeof(struct dev_acceleration));
	if (ret != 0) {
		return -EFAULT;
	}
	spin_lock(&acc_lock);
	memcpy(&acc, &temp_acc, sizeof(struct dev_acceleration));
	spin_unlock(&acc_lock);
	return 0;
}

SYSCALL_DEFINE1(set_acceleration, struct dev_acceleration __user *, accel)
{
	return do_set_acceleration(accel);
}

int do_accevt_create(struct acc_motion __user *acceleration)
{
	int ret = 0;
	struct dev_acceleration temp;
	ret = copy_from_user(&temp, acceleration, sizeof(struct dev_acceleration));

}

SYSCALL_DEFINE1(accevt_create, struct acc_motion __user *, acceleration)
{
	return do_accevt_create(acceleration);
}
