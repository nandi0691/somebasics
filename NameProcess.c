/*
	gcc NameProcess.c --->Compile
	./a.out & ps ---> see the result
*/
#include <stdio.h>      /* perror() */
#include <stdlib.h>     /* NULL */
#include <sys/prctl.h>  /* prctl(), PR_SET_NAME */
#include <sys/types.h>  /* wait() */
#include <sys/wait.h>   /* wait() */
#include <unistd.h>     /* fork(), sleep() */

int main()
{
	if (prctl(PR_SET_NAME, (unsigned long) "parent") < 0)
	return 0;
	
  switch (fork())
    {
    case 0:
      if (prctl(PR_SET_NAME, (unsigned long) "child") < 0)
        perror("prctl()");
      sleep(10);
      break;
    case -1:
      perror("fork()");
      break;
    default:
      if (wait(NULL) < 0)
        perror("wait()");
    }
  return 0;
}
