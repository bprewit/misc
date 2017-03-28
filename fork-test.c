#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigchld_handler(int);

int n_jobs = 0;

int main(int argc, char *argv[])
{
	pid_t pid;
	signal(SIGCHLD, sigchld_handler);
	for(int i = 0; i < 5; ++i)
	{
		++n_jobs;
		pid = fork();
		switch(pid) 
		{
			case 0:								/* child */
				sleep(5);
				exit(EXIT_SUCCESS);
				break;
			case -1:							/* error */
				perror("fork failed:");
				exit(EXIT_FAILURE);
				break;
			default: 							/* parent */
				fprintf(stderr, "launched pid %d\n", pid);
				break;
		}
	}
	if(pid > 0) 								/* parent */
	{
		while(n_jobs > 0)
		{
			sleep(1);
		}
	}
}

void sigchld_handler (int signum)
{
	int pid, status, serrno;
	serrno = errno;
	while(1)
	{
		pid = waitpid(WAIT_ANY, &status, WNOHANG);
		if (pid < 0)
		{
			perror ("waitpid");
			break;
		}
		if (pid == 0)
		{
			break;
		}
		fprintf(stderr, "pid %d exited\n", pid);
		--n_jobs;
	}
	errno = serrno;
}
