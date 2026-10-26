#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void childTask()
{
	char name[50];
	printf("Child (PID: %d) - Please enter your name: ", getpid());
	scanf("%49s", name);
	printf("Hello %s! Process ID: %d finished its task.\n", name, getpid());
}

int main(void)
{
	// Loop to create 4 child processes
	for(int i = 0; i < 4; i++)
	{
		pid_t pid = fork();
	
		if(pid < 0)
		{
			perror("fork");
			exit(1);
		}
		else if(pid == 0)
		{
			// Child process executes this function
			childTask();
			exit(0);
		}
	}
	
	// Parent process waits for all 4 child processes to complete
	for(int i = 0; i < 4; i++)
	{
		wait(NULL);
	}
	
	printf("Job is done\n");
	return EXIT_SUCCESS;

}
