#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/wait.h>
struct acc_motion {

     unsigned int dlt_x; /* +/- around X-axis */
     unsigned int dlt_y; /* +/- around Y-axis */
     unsigned int dlt_z; /* +/- around Z-axis */
     
     unsigned int frq;   /* Number of samples that satisfies:
                          sum_each_sample(dlt_x + dlt_y + dlt_z) > NOISE */
};
void initialize_event(int x, int y, int z, int f, struct acc_motion *a) {
	a->dlt_x = x;
	a->dlt_y = y;
	a->dlt_z = z;
	a->frq = f;
}
int main(){
	int N, pid, temp, i;

	N = 5;

	struct acc_motion event1;
	initialize_event(1, 1, 1, 3, &event1);


	int event_id1 = syscall(250, &event1);//create
	if (event_id1 < 0) {
		perror("create failed");
		return 1;
	}
	for(i = 0; i < N; i++) {
		pid = fork();
		if (pid < 0) {
			perror("fork failed");
			return 1;
		}
		else if (pid > 0)
			continue;
		else {
			int w = syscall(251, event_id1);//wait
			if (w != 0) {
				perror("wait error");
				return 1;
			}
			printf("%d detected a shake", getpid());
			break;
		}
	}

	if (pid > 0)
		sleep(60);
	temp = syscall(252, event_id1);//destroy
	if (temp != 0) {
		perror("destroy error");
		return 1;
	}
	for(i = 0; i < N; i++)
		wait(0);
	return 0;
}
