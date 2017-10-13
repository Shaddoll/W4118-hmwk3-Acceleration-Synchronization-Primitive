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



int do_accevt_wait(int event_id) {
	struct motion_events *evt = NULL;
	
	evt = find_event(event_id);
	if (evt == NULL)
		return -ENODATA;
	
	DEFINE_WAIT(wait);
	
	while (1) {
		spin_lock(&evt->waitq_lock);
		
		prepare_to_wait(&evt->waitq, &wait, TASK_INTERRUPTIBLE);
		if (evt->happened) {
			if (--evt->waitq_n == 0)
				evt->happend = false;
			spin_unlock(&evt->waitq_lock);
			break;
		}
		
		spin_unlock($evt->waitq_lock);
		
		if (signal_pending(current))
			break;
		
		schedule();
	}
	
	finish_wait(&evt->waitq, &wait);
	
	return 0;
}


SYSCALL_DEFINE1(accevt_wait, int event_id)
{
	return do_accevt_wait(event_id);
}


