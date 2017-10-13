#include <linux/syscalls.h>
#include <asm-generic/errno-base.h>
#include <linux/acceleration.h>
#include <linux/spinlock.h>

struct dev_acceleration acc;
static DEFINE_SPINLOCK(motion_event_lock);
static int number_of_events;
struct motion_events event_list;
static DEFINE_SPINLOCK(list_lock);
static DEFINE_SPINLOCK(event_counter_lock);

LIST_HEAD(&event_list);

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

int do_accevt_create(struct acc_motion __user *acceleration) {
	int id;
	struct motion_events *temp;
	int v;

	temp = kmalloc(sizeof(struct motion_events), GFP_KERNEL);
	if (temp == NULL)
		return -ENOMEM;

	temp->baseline = kmalloc(sizeof(struct acc_motion), GFP_KERNEL);
	if (kacceleration == NULL)
		return -ENOMEM;

	v = copy_from_user(temp->baseline, acceleration, sizeof(struct acc_motion));
	if (v < 0)
		return -EFAULT;

	spin_lock(&event_counter_lock);
	temp->id = number_of_events;
	number_of_events = number_of_events + 1;
	spin_unlock(&event_counter_lock);

	init_waitqueue_head(&(temp->waitq));
	temp->waitq_n = 0;
	spin_lock_init(&temp->waitq_lock);

	spin_lock(&list_lock);
	list_add(&temp->list, &event_list.list);
	spin_unlock(&list_lock);

	return 0;
}


int do_accevt_wait(int event_id) {
	struct motion_events *evt = NULL;
	
	evt = find_event(event_id);
	if (evt == NULL)
		return -ENODATA;
	
	DEFINE_WAIT(wait);
	
	spin_lock(&evt->waitq_lock);
	evt->waitq_n++;
	spin_unlock(&evt->waitq_lock);
	
	while (1) {
		
		prepare_to_wait(&evt->waitq, &wait, TASK_INTERRUPTIBLE);
		
		spin_lock(&evt->waitq_lock);
		if (evt->happened) {
			if (--evt->waitq_n == 0)
				evt->happend = false;
			spin_unlock(&evt->waitq_lock);
			break;
		}
		spin_unlock(&evt->waitq_lock);
		
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


