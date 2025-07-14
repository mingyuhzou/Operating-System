// producer.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FILENAME "numbers.txt"  // Define the filename to be used
#define COUNT 20  // Define the number of random integers to generate

int main() {
    int fd[2];  // Array to hold pipe file descriptors
    if (pipe(fd) == -1) {  // Create a pipe
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    // Create numbers.txt and write 20 random integers to it
    int file_fd = open(FILENAME, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (file_fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    srand(getpid());  // Seed the random number generator with the process ID
    int numbers[COUNT];  // Array to hold the random numbers
    for (int i = 0; i < COUNT; i++) {
        numbers[i] = rand() % 100;  // Generate a random number between 0 and 99
        dprintf(file_fd, "%d\n", numbers[i]);  // Write the number to the file
    }
    close(file_fd);  // Close the file after writing

    // Open numbers.txt for reading
    file_fd = open(FILENAME, O_RDONLY);
    if (file_fd == -1) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    // Output the file descriptor and contents of the file
    printf("File numbers.txt fd is: %d\n", file_fd);
    printf("Contents of file numbers.txt:\n");

    // Read the file contents and print them
    char buffer[128];
    int bytesRead;
    while ((bytesRead = read(file_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';  // Ensure the buffer is null-terminated
        printf("%s", buffer);
    }
    close(file_fd);  // Close the file after reading
    printf("\n");

    // Fork to create the consumer process
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) { 
        // Parent process (producer)
        printf("Parent Process: My pid = %d. I created child pid = %d.\n", getpid(), pid);
        close(fd[0]);  // Close the read end of the pipe

        // Reopen the file and send data through the pipe
        file_fd = open(FILENAME, O_RDONLY);
        if (file_fd == -1) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }

        while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0) {
            write(fd[1], buffer, bytesRead);  // Write data to the pipe
        }

        close(file_fd);  // Close the file
        close(fd[1]);  // Close the write end of the pipe
        wait(NULL);    // Wait for the child process to finish
    } else { 
        // Child process (consumer)
        printf("Child Process: My pid = %d. My parent pid = %d.\n", getpid(), getppid());
        close(fd[1]);  // Close the write end of the pipe
        dup2(fd[0], STDIN_FILENO);  // Redirect stdin to read from the pipe
        execlp("./consumer", "consumer", NULL);  // Execute the consumer program
        perror("execlp failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}