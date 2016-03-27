#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


int main(void)
{
	while (1) {
		sleep(3);
		printf("%s, %d\n", __FILE__, (int)getpid());
	}

	return 0;
}
