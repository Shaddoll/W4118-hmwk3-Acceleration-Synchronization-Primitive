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
int main(int argc, char**argv){
	int N, pid, temp, temp1, temp2, i;

	if (argc == 2) {
		N = atoi(argv[1]);
	} else {
		fprintf(stderr, "parameters incorrect");
		return 1;
	}

	struct acc_motion event1, event2, event3;
	initialize_event(50, 0, 0, 5, &event1);
	initialize_event(0, 50, 0, 5, &event2);
	initialize_event(0, 0, 0, 5, &event3);


	int event_id1 = syscall(250, &event1);//create
	int event_id2 = syscall(250, &event2);//create
	int event_id3 = syscall(250, &event3);//create
	if (event_id1 < 0 || event_id2 < 0 || event_id3 < 0) {
		fprintf(stderr, "create failed\n");
		return 1;
	}
	for(i = 0; i < N; i++) {
		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "fork failed\n");
			return 1;
		}
		else if (pid > 0)
			continue;
		else {
			int w;
			if (i%3 == 0)
				w = syscall(251, event_id1);//wait
			else if (i%3 == 1)
                        	w = syscall(251, event_id2);//wait
			else
                        	w = syscall(251, event_id3);//wait
			if (w != 0) {
				printf("wait error\n");
				return 1;
			}
			if (i%3 == 0)
				printf("%d detected a horizontal shake\n", getpid());
			else if (i%3 == 1)
				printf("%d detected a vertical shake\n", getpid());
			else
				printf("%d detected a shake\n", getpid());
			break;
		}
	}

	if (pid > 0)
		sleep(60);
	else
		return 0;
	printf("destroy begin\n");
	temp = syscall(253, event_id1);//destroy
	printf("destroy end\n");
	temp1 = syscall(253, event_id2);
	printf("destroy end\n");
	temp2 = syscall(253, event_id3);
	printf("destroy end\n");
	if (temp != 0 || temp1 != 0 || temp2 != 0) {
		fprintf(stderr, "destroy error\n");
		return 1;
	}
	for(i = 0; i < N; i++)
		wait(0);
	return 0;
}
