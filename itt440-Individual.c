#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// Signal handler function for graceful shutdown on Ctrl+C (SIGINT)
void sigint_handler(int sig) 
{
    const char msg[] = "\n\n+-----------------------------------------------------+ \n"
                       "| [INTERRUPT DETECTED] Ctrl+C pressed!                | \n"
                       "| IT Command Centre: Shutting down systems safely.    | \n"
                       "+-----------------------------------------------------+ \n";
    write(STDOUT_FILENO, msg, strlen(msg));
    exit(0);
}

int main() 
{
    int pipes[4][2]; // Array of 4 pipes for parent-to-child communication
    pid_t pids[4];   // Array to store Process IDs for the 4 worker processes
    char user_input[256];

    // Register signal handler for Ctrl+C
    if (signal(SIGINT, sigint_handler) == SIG_ERR) 
    {
        perror("Failed to register signal handler");
        return 1;
    }

    // Display the main Command Centre dashboard menu
    printf("+-----------------------------------------------------+ \n");
    printf("|       IT DEPARTMENT DASHBOARD MENU                  | \n");
    printf("+-----------------------------------------------------+ \n");
    printf("Available Commands:                                   \n");
    printf("  1. AUTH   -> Authentication                         \n");
    printf("  2. DB     -> Database Administration Unit           \n");
    printf("  3. PAY    -> Financial Systems / Billing Unit       \n");
    printf("  4. NOTIF  -> IT Support                             \n");
    printf("  5. EXIT   -> Shutdown Command Centre                \n");
    printf("+-----------------------------------------------------+ \n");
    fflush(stdout);

    // Create 4 child worker processes connected via pipes
    for (int i = 0; i < 4; i++) 
    {
        // Initialize pipe for worker i
        if (pipe(pipes[i]) == -1) 
        {
            perror("Pipe creation failed");
            return 1;
        }

        // Fork a new process
        pids[i] = fork();

        if (pids[i] < 0) 
        {
            perror("Fork failed");
            return 1;
        }

        // === CHILD PROCESS LOGIC ===
        if (pids[i] == 0) 
        {
            close(pipes[i][1]); // Children only read, close unused write end
            char read_buffer[256];
            
            while (1) 
            {
                // Read commands sent by the parent through the pipe
                ssize_t bytes_read = read(pipes[i][0], read_buffer, sizeof(read_buffer) - 1);
                
                if (bytes_read <= 0) 
                {
                    break;
                }

                read_buffer[bytes_read] = '\0';

                // Break loop if shutdown signal received from parent
                if (strcmp(read_buffer, "EXIT") == 0) 
                {
                    break;
                }

                // Render dynamic, perfectly aligned ASCII status box
                char line_buf[100];
                printf("\n+-----------------------------------------------------+ \n");
                
                snprintf(line_buf, sizeof(line_buf), "WORKER PROCESS %d (PID: %d)", i + 1, getpid());
                printf("| %-51s |\n", line_buf);

                // Assign unit roles based on worker index
                if (i == 0) 
                {
                    printf("| %-51s |\n", "Unit: Authentication & Access Control");
                    printf("| %-51s |\n", "Status: Identity verification");
                } 
                else if (i == 1) 
                {
                    printf("| %-51s |\n", "Unit: Database Administration Unit");
                    printf("| %-51s |\n", "Status: Up servers and databases");
                } 
                else if (i == 2) 
                {
                    printf("| %-51s |\n", "Unit: Financial Systems & Billing Unit");
                    printf("| %-51s |\n", "Status: Verified billing gateway online");
                } 
                else if (i == 3) 
                {
                    printf("| %-51s |\n", "Unit: IT Support");
                    printf("| %-51s |\n", "Status: Ticket queue clear");
                }
                printf("+-----------------------------------------------------+ \n");
                fflush(stdout); // Force immediate rendering to terminal
            }

            close(pipes[i][0]);
            exit(0);
        } 
        // === PARENT PROCESS LOGIC ===
        else 
        {
            close(pipes[i][0]); // Parent only writes, close unused read end
        }
    }

    // Main interaction loop for parent Command Centre
    while (1) 
    {
        printf("\n[Command Centre] Enter command (AUTH, DB, PAY, NOTIF, EXIT): ");
        fflush(stdout);
        
        // Read user input safely using fgets
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) 
        {
            break;
        }

        // Remove trailing newline character from input
        user_input[strcspn(user_input, "\n")] = '\0';

        // Ignore empty inputs
        if (strlen(user_input) == 0) 
        {
            continue;
        }

        // Handle normal system shutdown request
        if (strcmp(user_input, "EXIT") == 0) 
        {
            // Broadcast "EXIT" message to all 4 worker processes
            for (int i = 0; i < 4; i++) 
            {
                write(pipes[i][1], "EXIT", 5);
            }
            printf("[Command Centre] Shutting down infrastructure links.\n");
            fflush(stdout);
            break; 
        }

        // Route matching commands to the appropriate worker pipe
        if (strcmp(user_input, "AUTH") == 0) 
        {
            write(pipes[0][1], user_input, strlen(user_input) + 1); // Send to Worker 1
        } 
        else if (strcmp(user_input, "DB") == 0) 
        {
            write(pipes[1][1], user_input, strlen(user_input) + 1); // Send to Worker 2
        } 
        else if (strcmp(user_input, "PAY") == 0) 
        {
            write(pipes[2][1], user_input, strlen(user_input) + 1); // Send to Worker 3
        } 
        else if (strcmp(user_input, "NOTIF") == 0) 
        {
            write(pipes[3][1], user_input, strlen(user_input) + 1); // Send to Worker 4
        } 
        else 
        {
            // Handle invalid commands
            printf("[Command Centre] Invalid command. Please use AUTH, DB, PAY, NOTIF, or EXIT.\n");
            fflush(stdout);
            continue;
        }

        // Brief delay to allow the child process time to print cleanly before displaying the next prompt
        usleep(300000); 
    }
    
    // Close all parent pipe write ends so children detect EOF and unblock
    for (int i = 0; i < 4; i++) 
    {
        close(pipes[i][1]);
    }

    // Reap all 4 child worker processes to prevent zombies
    for (int i = 0; i < 4; i++) 
    {
        wait(NULL);
    }

    printf("\n[Command Centre] All systems shut down cleanly.\n");
    fflush(stdout);
    return 0;
}
