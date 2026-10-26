#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void childTask(int id)
{
	char name[50];
	printf("Child %d (PID: %d) - Please enter your name: ", id, getpid());
	scanf("%49s", name);
	printf("Hello %s! Child %d finished its task.\n", name, getpid());
}

int main(void)
{
	// Loop to create 4 child processes
	for(int i = 1; i <= 4; i++)
	{
		pid_t pid = fork();
		if(pid < 0)
		{
			perror("fork");
			exit(1);
		}
		else if(pid == 0)
		{
			//Child process executes this function
			childTask(i);
			exit(0);
		}
		else
		{
			// Parent process waits for all 4 child processes to complete
			wait(NULL);
		}
	}

	printf("Job is done\n");
	return EXIT_SUCCESS;
}
