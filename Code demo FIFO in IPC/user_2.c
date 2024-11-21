#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <pwd.h>

#define FIFO1 "FIFO1" // FIFO for receiving messages from User1
#define FIFO2 "FIFO2" // FIFO for sending messages from User2 to User1
#define MAX_SIZE 1024
#define HISTORY_FILE "chat_history.txt"

int send_fd, recv_fd; // File descriptors for opening FIFO
int running = 1; // Flag to control the chat loop

// Signal handler to close FIFO before exiting
void handle_sigint(int sig) {
    printf("\nEnding chat session.\n");
    running = 0;
    close(send_fd);
    close(recv_fd);
    unlink(FIFO1);
    unlink(FIFO2);
    exit(0);
}

// Function to save message history to a file
void save_message_to_history(const char *message) {
    FILE *file = fopen(HISTORY_FILE, "a");
    if (file == NULL) {
        perror("Cannot open history file");
        return;
    }
    fprintf(file, "%s\n", message);
    fclose(file);
}

// Function to get the current time
void get_current_time(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "[%Y-%m-%d %H:%M:%S]", t);
}

// Function that continuously receives messages from User1
void *receive_messages(void *arg) {
    char message[MAX_SIZE];
    char formatted_message[MAX_SIZE + 50];
    char time_buffer[20];

    while (running) {
        ssize_t bytes_read = read(recv_fd, message, MAX_SIZE);
        if (bytes_read > 0) {
            message[bytes_read] = '\0'; // Null-terminate received message

            // Add timestamp and tag message as from "embeoxinchao" (User1)
            get_current_time(time_buffer, sizeof(time_buffer));
            snprintf(formatted_message, sizeof(formatted_message), "%s embeoxinchao: %s", time_buffer, message);

            // Display the received message and save it to history
            printf("%s\n", formatted_message);
            save_message_to_history(formatted_message); // Save received message to history
        } else if (bytes_read == -1 && errno != EAGAIN) {
            perror("Error reading from FIFO");
        }
        usleep(100000); // Sleep briefly to prevent CPU overuse
    }
    return NULL;
}
void chat_user2() {
    char message[MAX_SIZE];
    char formatted_message[MAX_SIZE + 50];
    char time_buffer[20];
    pthread_t receiver_thread;

    // Get the current username for User2 using the effective user ID
    const char *username = getpwuid(geteuid())->pw_name;

    if (username == NULL) {
        perror("Unable to retrieve username");
        exit(1);
    }

    // Open FIFO for sending and receiving messages
    recv_fd = open(FIFO1, O_RDONLY | O_NONBLOCK); // Open FIFO1 in non-blocking mode for receiving
    send_fd = open(FIFO2, O_WRONLY);

    if (send_fd < 0 || recv_fd < 0) {
        perror("Cannot open FIFO");
        exit(1);
    }

    // Create a thread to receive messages continuously
    pthread_create(&receiver_thread, NULL, receive_messages, NULL);

    printf("Chat started. Enter a message and press Enter to send.\n");
    printf("Type '/exit' to quit or '/clear' to clear the screen.\n");

    while (running) {
        // Prompt for user input
        printf("%s: ", username); // Use the username dynamically
        if (fgets(message, MAX_SIZE, stdin) == NULL) continue;

        // Handle special commands
        if (strncmp(message, "/exit", 5) == 0) {
            running = 0;
            break;
        } else if (strncmp(message, "/clear", 6) == 0) {
            system("clear");
            continue;
        }

        // Add timestamp to the outgoing message
        get_current_time(time_buffer, sizeof(time_buffer));
        snprintf(formatted_message, sizeof(formatted_message), "%s %s: %s", time_buffer, username, message);

        // Send the message and save to history
        write(send_fd, formatted_message, strlen(formatted_message) + 1);
        save_message_to_history(formatted_message);
    }

    // Clean up
    pthread_join(receiver_thread, NULL);
    close(send_fd);
    close(recv_fd);
    unlink(FIFO1);
    unlink(FIFO2);
}

int main() {
    // Set up signal handler for safe exit
    signal(SIGINT, handle_sigint);

    // Create FIFOs with read/write permissions for both processes
    if (mkfifo(FIFO1, 0666) == -1 && errno != EEXIST) {
        perror("Error creating FIFO1");
        exit(1);
    }
    if (mkfifo(FIFO2, 0666) == -1 && errno != EEXIST) {
        perror("Error creating FIFO2");
        exit(1);
    }

    // Start chat process for User2
    chat_user2();

    return 0;
}
