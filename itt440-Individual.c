#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// Signal handler function
void sigint_handler(int sig) 
{
    const char msg[] = "\n\n+---------------------------------------------------+ \n"
                       "| [INTERRUPT DETECTED] Ctrl+C pressed!              | \n"
                       "| IT Command Centre: Shutting down systems safely.  | \n"
                       "+---------------------------------------------------+ \n";
    write(STDOUT_FILENO, msg, strlen(msg));
    exit(0);
}

int main() 
{
    // Array of 4 pipes
    int pipes[4][2];
    
    // Array to store the Process IDs (PIDs)
    pid_t pids[4];
    
    char user_input[256];

    // Register signal handler
    if (signal(SIGINT, sigint_handler) == SIG_ERR) 
    {
        perror("Failed to register signal handler");
        return 1;
    }

    // Dashboard Menu
    printf("+---------------------------------------------------+ \n");
    printf("|     IT DEPARTMENT DASHBOARD MENU                  | \n");
    printf("+---------------------------------------------------+ \n");
    printf("Available Commands:                                 \n");
    printf("  1. AUTH   -> Authentication                       \n");
    printf("  2. DB     -> Database Administration Unit         \n");
    printf("  3. PAY    -> Financial Systems / Billing Unit     \n");
    printf("  4. NOTIF  -> IT Support                           \n");
    printf("  5. EXIT   -> Shutdown Command Centre              \n");
    printf("+---------------------------------------------------+ \n");

    // PIPES and FORK creation (child)
    for (int i = 0; i < 4; i++) 
    {
        // Communication between parent and child i
        if (pipe(pipes[i]) == -1) 
        {
            perror("Pipe creation failed");
            return 1;
        }

        // Fork the process
        pids[i] = fork();

        if (pids[i] < 0) 
        {
            perror("Fork failed");
            return 1;
        }

        // CHILD PROCESS
        if (pids[i] == 0) 
        {
            // Close the unused WRITE end
            close(pipes[i][1]); 

            char read_buffer[256];
            
            while (1) 
            {
                // Read data from the parent
                ssize_t bytes_read = read(pipes[i][0], read_buffer, sizeof(read_buffer) - 1);
                
                if (bytes_read <= 0) 
                {
                    break;
                }

                read_buffer[bytes_read] = '\0';

                // If parent sends "EXIT", break the loop and shut down child
                if (strcmp(read_buffer, "EXIT") == 0) 
                {
                    break;
                }

                // Child menu / worker
                printf("\n+---------------------------------------------------+ \n");
                printf("| WORKER PROCESS %d (PID: %d)                       | \n", i + 1, getpid());
                if (i == 0) 
                {
                    printf("| Unit: Authentication & Access Control             | \n");
                    printf("| Status: Identity verification                     | \n");
                } else if (i == 1) 
                {
                    printf("| Unit: Database Administration Unit                | \n");
                    printf("| Status: Up servers and databases                  | \n");
                } else if (i == 2) 
                {
                    printf("| Unit: Financial Systems & Billing Unit            | \n");
                    printf("| Status: Verified billing gateway online   | \n");
                } else if (i == 3) 
                {
                    printf("| Unit: IT Support                                  | \n");
                    printf("| Status: Ticket queue clear                        | \n");
                }
                printf("+---------------------------------------------------+ \n");
            }

            close(pipes[i][0]);
            exit(0);
        } 

        else 
        {
 
            close(pipes[i][0]); 
        }
    }

    // Parent/User Input for commands
    while (1) 
    {
        printf("\n[Command Centre] Enter command (AUTH, DB, PAY, NOTIF, EXIT): ");
        
        // Read user input
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) 
        {
            break;
        }

        user_input[strcspn(user_input, "\n")] = '\0';

        // Ignore empty inputs 
        if (strlen(user_input) == 0) 
        {
            continue;
        }

        // Check if user wants to exit normally
        if (strcmp(user_input, "EXIT") == 0) 
        {
            // Broadcast system shutdown (all child)
            for (int i = 0; i < 4; i++) 
            {
                write(pipes[i][1], "EXIT", 5);
            }
            printf("[Command Centre] Shutting down infrastructure links.\n");
            break; // Exit the parent loop
        }

        // Routing commands to specific child using pipes
        if (strcmp(user_input, "AUTH") == 0) 
        {
            write(pipes[0][1], user_input, strlen(user_input) + 1); // Child 1
        } else if (strcmp(user_input, "DB") == 0) 
        {
            write(pipes[1][1], user_input, strlen(user_input) + 1); // Child 2
        } else if (strcmp(user_input, "PAY") == 0) 
        {
            write(pipes[2][1], user_input, strlen(user_input) + 1); // Child 3
        } else if (strcmp(user_input, "NOTIF") == 0) 
        {
            write(pipes[3][1], user_input, strlen(user_input) + 1); // Child 4
        } else 
        {
            // Handle invalid user input
            printf("[Command Centre] Invalid command. Please use AUTH, DB, PAY, NOTIF, or EXIT.\n");
            continue;
        }
    }
    
    // Close all parent pipe write ends so children unblock and notice EOF
    for (int i = 0; i < 4; i++) 
    {
        close(pipes[i][1]);
    }

    // Wait for all 4 child worker processes to terminate completely
    for (int i = 0; i < 4; i++) 
    {
        wait(NULL);
    }

    printf("\n[Command Centre] All systems shut down cleanly.\n");
    return 0;
}
