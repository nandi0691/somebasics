#include <time.h>
#include <iostream>
#include <sys/time.h>

int main()
{
	time_t clk = time(NULL);
	struct timeval tv;

	gettimeofday(&tv, NULL);

	unsigned long long millisecondsSinceEpoch =
		    (unsigned long long)(tv.tv_sec) * 1000 +
		        (unsigned long long)(tv.tv_usec) / 1000;

	printf("%llu\n", millisecondsSinceEpoch);
	return 0;
}
