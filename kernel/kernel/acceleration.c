#include <linux/syscalls.h>
#include <asm-generic/errno-base.h>
#include <linux/acceleration.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

static DEFINE_SPINLOCK(acc_lock);	/* spinlock for `acc` */
static struct dev_acceleration acc;	/* used by set_acceleration syscall */

static DEFINE SPINLOCK(acclist_lock);
static LIST_HEAD(acc_list);

static int number_of_events;
struct motion_events event_list;
static DEFINE_SPINLOCK(event_list_lock);
static DEFINE_SPINLOCK(event_counter_lock);

LIST_HEAD(&event_list);

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

	spin_lock(&event_list_lock);
	list_add(&temp->list, &event_list.list);
	spin_unlock(&event_list_lock);

	return 0;
}


int do_accevt_wait(int event_id) {
	struct motion_event *evt = NULL;
	
	spin_lock(&event_list_lock);
	evt = find_event(event_id);
	if (evt == NULL)
		return -ENODATA;
	spin_lock(&evt->waitq_lock);
	evt->waitq_n++;
	spin_unlock(&evt->waitq_lock);
	spin_unlock(&event_list_lock);
	
	DEFINE_WAIT(wait);
	
	
	while (1) {
		
		prepare_to_wait(&evt->waitq, &wait, TASK_INTERRUPTIBLE);
		
		spin_lock(&evt->event_lock);
		if (evt->happened) {
			if (--evt->waitq_n == 0)
				evt->happend = false;
			spin_unlock(&evt->event_lock);
			break;
		}
		if (evt->destroyed) {
			if (--evt->waitq_n == 0)
				evt->destroyed = false;
			spin_unlock(&evt->event_lock);
			break;
		}
		spin_unlock(&evt->event_lock);
		
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

static inline int satisfy_baseline(struct acceleration_list *prev,
				   struct acceleration_list *curr,
				   struct acc_motion *baseline)
{
	int delta_x = abs(prev->acc.x - curr->acc.x);
	int delta_y = abs(prev->acc.y - curr->acc.y);
	int delta_z = abs(prev->acc.z - curr->acc.z);
	int strength = delta_x + delta_y + delta_z;
	if (strength > NOISE && 
	    delta_x >= baseline->dlt_x &&
	    delta_y >= baseline->dlt_y &&
	    delta_z >= baseline->dlt_z) {
		return 1;
	}
	return 0;
}

static int verify_event(struct list_head *acc_list,
			struct acc_motion *baseline)
{
	struct acceleration_list *curr, *prev;
	int frq = 0;
	int first_loop = 1;

	list_for_each_entry(curr, acc_list, list) {
		if (first_loop) {
			first_loop = 0;
			prev = curr;
			continue;
		}
		if (satisfy_baseline(prev, curr, baseline))
			++frq;
		prev = curr;
		if (frq >= baseline->frq)
			return 1;
	}
	return 0;
}

int do_accevt_signal(struct dev_acceleration __user *acceleration)
{
	static int acc_list_length = 0;
	int ret;
	struct acceleration_list *new_data;
	struct acceleration_list *first;
	struct motion_events *motion;
	
	new_data = kmalloc(sizeof(struct acceleration_list), GFP_KERNEL);
	if (new_data == NULL) {
		return -ENOMEM;
	}
	ret = copy_from_user(&(new_data->acc),
				acceleration,
				sizeof(struct dev_acceleration));
	if (ret < 0) {
		kfree(new_data);
		return -EFAULT;
	}
	spinlock(&acclist_lock);
	list_add_tail(&(new_data->list), &acc_list);
	if (acc_list_length == WINDOW + 1) { /* window is full */
		first = list_first_entry(&acc_list,
					struct acceleration_list,
					list);
		list_del(&(first->list));
		kfree(first);
	}
	else {
		++acc_list_length;
	}
	spinlock(&event_list_lock);
	list_for_each_entry(motion, &event_list, list) {
		/* verify if the motion event's baseline is satisfied */
		if (verify_event(&acc_list, motion->baseline)) {
			motion->happened = true;
			wake_up_interruptible(&(motion->waitq));
		}
	}
	spinunlock(&event_list_lock);
	spinunlock(&acclist_lock);
	return 0;
}
