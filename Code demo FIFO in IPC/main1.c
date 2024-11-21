#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>  // For open
#include <errno.h>  // For error handling
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>  // For checking if characters are digits

int main(int argc, char* argv[]) {
    // Check if the input file is provided as a command-line argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    // Open the input file
    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        perror("Error opening input file");
        return 1;
    }

    // Dynamically allocate memory for numbers read from the file
    int capacity = 10; // Initial capacity
    int* arr = (int*)malloc(capacity * sizeof(int));
    if (!arr) {
        perror("Error allocating memory");
        fclose(inputFile);
        return 2;
    }

    int num, count = 0;

    // Read all numbers from the file
    while (fscanf(inputFile, "%d", &num) == 1) {
        // Resize array if capacity is exceeded
        if (count == capacity) {
            capacity *= 2;
            int* temp = realloc(arr, capacity * sizeof(int));
            if (!temp) {
                perror("Error reallocating memory");
                free(arr);
                fclose(inputFile);
                return 3;
            }
            arr = temp;
        }
        arr[count++] = num;
    }

    // Check for invalid characters in the file
    char ch;
    while ((ch = fgetc(inputFile)) != EOF) {
        if (!isspace(ch)) {  // If we find a non-space character (e.g., a letter), it's invalid
            fprintf(stderr, "File must contain only numbers, found invalid character '%c'\n", ch);
            free(arr);
            fclose(inputFile);
            return 4;
        }
    }

    fclose(inputFile);

    // Check if no valid numbers were found in the file
    if (count == 0) {
        fprintf(stderr, "Error: No valid numbers found in the input file\n");
        free(arr);
        return 5;
    }

    // Open the FIFO named "sum" for writing
    int fd = open("sum", O_WRONLY);
    if (fd == -1) {
        perror("Error opening FIFO");
        free(arr);
        return 6;
    }

    // Write the count of numbers first
    if (write(fd, &count, sizeof(int)) == -1) {
        perror("Error writing to FIFO");
        close(fd);
        free(arr);
        return 7;
    }

    // Write the array of numbers to the FIFO
    if (write(fd, arr, count * sizeof(int)) == -1) {
        perror("Error writing to FIFO");
        close(fd);
        free(arr);
        return 8;
    }

    // Clean up
    close(fd);
    free(arr);
    return 0;
}

