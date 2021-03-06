/*
 * Columbia University
 * COMS W4118 Fall 2017
 * Homework 3
 *
 * Adapted from Fall 2016 solution
 *
 */
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <hardware/hardware.h>
#include <hardware/sensors.h> /* <-- This is a good place to look! */

#include "accelerationd.h"

static int effective_linaccel_sensor = -1;
static int sensor_mode = 1;

/* helper functions which you should use */
static int open_sensors(struct sensors_module_t **hw_module,
			struct sensors_poll_device_t **poll_device);
static void enumerate_sensors(const struct sensors_module_t *sensors);
static struct dev_acceleration *poll_sensor_data_emulator(void);
static int poll_sensor_data(struct sensors_poll_device_t *sensors_device);

void daemon_mode(void)
{
	/* Fill in */
	pid_t pid;
	pid_t sid;
	int fd;

	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "fork\n");
		fprintf(stderr, "error: %s\n", strerror(errno));
		exit(1);
	} else if (pid > 0) {
		exit(0);
	}
	/* child process */
	sid = setsid();
	if (sid < 0) {
		fprintf(stderr, "setsid\n");
		fprintf(stderr, "error: %s\n", strerror(errno));
		exit(1);
	}
	umask(0);
	chdir("/");
	fd = open("/dev/null", O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "open\n");
		fprintf(stderr, "error: %s\n", strerror(errno));
		exit(1);
	}
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
	if (fd > 2)
		close(fd);
}

int main(int argc, char **argv)
{
	struct sensors_module_t *sensors_module = NULL;
	struct sensors_poll_device_t *sensors_device = NULL;
	/* int errsv = 0;  Error that we pull up from system call*/

	if (argc > 1 && strcmp(argv[1], "-o") == 0)
		sensor_mode = 0;
	if (argc > 1 && strcmp(argv[1], "-e") == 0)
		goto emulation;

	/*
	 * TODO: Implement your code to make this process a daemon in
	 * daemon_mode function
	 */
	daemon_mode();

	printf("Opening sensors...\n");
	if (open_sensors(&sensors_module,
			 &sensors_device) < 0) {
		printf("open_sensors failed\n");
		return EXIT_FAILURE;
	}
	enumerate_sensors(sensors_module);

	printf("turn me into a daemon!\n");
	while (1) {
emulation:
		poll_sensor_data(sensors_device);
		/* TODO: Define time interval and call usleep */
		usleep((useconds_t) 1000 * TIME_INTERVAL);
	}

	return EXIT_SUCCESS;
}


static int poll_sensor_data(struct sensors_poll_device_t *sensors_device)
{
	struct dev_acceleration *cur_acceleration;
	int err = 0; /* Return value of this function*/
	int i;

	if (effective_linaccel_sensor < 0) {
		/* emulation */
		cur_acceleration = poll_sensor_data_emulator();
		/*
		 * TODO: You have the acceleration here -
		 * scale it and send it to your kernel
		 */
		if (sensor_mode == 0) {
			err = syscall(__NR_set_acceleration,
				      cur_acceleration);
		} else {
			err = syscall(__NR_accevt_signal,
				     cur_acceleration);
		}
		if (err < 0)
			goto error;
	} else {


		sensors_event_t buffer[128];
		ssize_t buf_size = sizeof(buffer)/sizeof(buffer[0]);
		ssize_t count = sensors_device->poll(sensors_device,
						buffer,
						buf_size);
		/*
		 * TODO: You have the acceleration here - scale it and
		 * send it to kernel
		 */
		cur_acceleration = (struct dev_acceleration *)
			malloc(sizeof(struct dev_acceleration));
		for (i = 0; i < count; ++i) {
			if (buffer[i].sensor == effective_linaccel_sensor) {
				cur_acceleration->x =
					(int)(buffer[i].acceleration.x * 100);
				cur_acceleration->y =
					(int)(buffer[i].acceleration.y * 100);
				cur_acceleration->z =
					(int)(buffer[i].acceleration.z * 100);
				if (sensor_mode == 0) {
					err = syscall(__NR_set_acceleration,
						      cur_acceleration);
				} else {
					err = syscall(__NR_accevt_signal,
						     cur_acceleration);
				}
				if (err < 0)
					goto error;
			}
		}
	}
error:
	free(cur_acceleration);
	return err;
}



/*  DO NOT MODIFY BELOW THIS LINE  */
/*---------------------------------*/



static struct dev_acceleration *poll_sensor_data_emulator(void)
{
	int cur_x, cur_y, cur_z;
	struct dev_acceleration *ad;

	ad = (struct dev_acceleration *)
			malloc(sizeof(struct dev_acceleration));

	if (!ad) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		exit(1);
	}

	FILE *fp = fopen("/data/misc/acceleration", "r");

	if (!fp) {
		free(ad);
		return 0;
	}
	fscanf(fp, "%d, %d, %d", &cur_x, &cur_y, &cur_z);
	fclose(fp);
	ad->x = cur_x;
	ad->y = cur_y;
	ad->z = cur_z;

	return ad;
}




static int open_sensors(struct sensors_module_t **mSensorModule,
			struct sensors_poll_device_t **mSensorDevice)
{
	int err = hw_get_module(SENSORS_HARDWARE_MODULE_ID,
				(hw_module_t const **)mSensorModule);

	if (err) {
		printf("couldn't load %s module (%s)",
		       SENSORS_HARDWARE_MODULE_ID, strerror(-err));
	}

	if (!*mSensorModule)
		return -1;

	err = sensors_open(&((*mSensorModule)->common), mSensorDevice);

	if (err) {
		printf("couldn't open device for module %s (%s)",
		       SENSORS_HARDWARE_MODULE_ID, strerror(-err));
	}

	if (!*mSensorDevice)
		return -1;

	const struct sensor_t *list;
	ssize_t count =
		(*mSensorModule)->get_sensors_list(*mSensorModule, &list);
	size_t i;

	for (i = 0 ; i < (size_t)count ; i++)
		(*mSensorDevice)->activate(*mSensorDevice, list[i].handle, 1);
	return 0;
}

static void enumerate_sensors(const struct sensors_module_t *sensors)
{
	int nr, s;
	const struct sensor_t *slist = NULL;

	if (!sensors)
		printf("going to fail\n");

	nr = sensors->get_sensors_list((struct sensors_module_t *)sensors,
				       &slist);
	if (nr < 1 || slist == NULL) {
		printf("no sensors!\n");
		return;
	}

	for (s = 0; s < nr; s++) {
		printf("%s (%s) v%d\n\tHandle:%d, type:%d, max:%0.2f, "
			"resolution:%0.2f \n", slist[s].name, slist[s].vendor,
			slist[s].version, slist[s].handle, slist[s].type,
			slist[s].maxRange, slist[s].resolution);

		if (slist[s].type == SENSOR_TYPE_LINEAR_ACCELERATION)
			effective_linaccel_sensor = slist[s].handle;
	}
}
