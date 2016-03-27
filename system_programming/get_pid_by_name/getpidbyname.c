#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>

#define PID_MAX "/proc/sys/kernel/pid_max"

unsigned int get_sys_max_pid(void)
{
	FILE *filp = fopen(PID_MAX, "r");

	if (!filp) {
		fprintf(stderr, "open %s failed, %s\n", PID_MAX, strerror(errno));
		return 0;
	}

	char max[10] = {0};
	unsigned int pid_max = 0;

	if (fgets(max, 9, filp)) {
		pid_max = atoi(max);
		fclose(filp);
#ifdef DEBUG
		printf("current system max pid is : %u\n", pid_max);
#endif
	}

	return pid_max;
}

unsigned int get_pid_by_name(const char *pro_name)
{
	/*
	 * search /proc/pid/status to find program's name 
	 * and then we can find the pid
	 */
	unsigned int pid = 1; // search start
	FILE *filp = NULL;
	char proc_status[20] = {0};
	char status_info[100] = {0}; //file status information store in this
	char cur_name[100] = {0}; //program's name's max lenght

	unsigned int pid_max = get_sys_max_pid(); //max pid

	while (1) {
		/*
		 * open 1 program's status file, then read data
		 */
		sprintf(proc_status, "/proc/%u/status", pid);
#ifdef DEBUG
		printf("proc_status: %s, led: %d\n", proc_status, strlen(proc_status));
#endif

		filp = fopen(proc_status, "r");
		if (!filp) {
#ifdef DEBUG
			perror("fopen");
#endif
			if (pid >= pid_max) {
				fprintf(stderr, "Can not find pid of program %s\n", pro_name);
				break;
			}
			/*
			 * current pid is no exist, go to search next
			 */
			pid++;
			continue;
		} 
		/*
		 * get one line to sparse by loop
		 */
		while ((!feof(filp)) && (fgets(status_info, 99, filp))) {
			if (sscanf(status_info, "Name: %s", cur_name) == 1) {
				/*
				 * Run here means that we got current program's name,
				 * next what we should do is to compare.
				 */
				if (!strcmp(cur_name, pro_name)) {
					fclose(filp);
					return pid;
				}
				/*
				 * Not match, search next pid.
				 */
				bzero(cur_name, sizeof(cur_name));
				break;

			}
			/*
			 * Didn't scan target, scan next line.
			 */
			bzero(status_info, sizeof(status_info));
		} /* end of one pid */
		pid++;
		fclose(filp);
		bzero(status_info, sizeof(status_info));
	}
	
	return 0; /* Haven't find the pid, return 0 */
}


int main(int argc, char **argv)
{
	unsigned int target_pid;
	
	if (argc != 2) {
		fprintf(stderr, "argument error, i.e. %s test_pro\n", argv[0]);	
		return -1;
	}

	target_pid = get_pid_by_name(argv[1]);
	if (target_pid)
		printf("program %s's pid: %u\n", argv[1], target_pid);

	return 0;
}
