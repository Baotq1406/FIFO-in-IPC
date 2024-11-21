#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>  // For open
#include <errno.h>  // For error handling

int main(int argc, char* argv[]) {
    // Open the FIFO named "sum" for reading
    int fd = open("sum", O_RDONLY);
    if (fd == -1) {
        perror("Error opening FIFO");
        return 1;
    }

    // Read the count of numbers from the FIFO
    int count;
    if (read(fd, &count, sizeof(int)) == -1) {
        perror("Error reading count from FIFO");
        close(fd);
        return 2;
    }

    // Dynamically allocate memory to store the numbers
    int* arr = (int*)malloc(count * sizeof(int));
    if (!arr) {
        perror("Error allocating memory");
        close(fd);
        return 3;
    }

    // Read the array of numbers from the FIFO
    if (read(fd, arr, count * sizeof(int)) == -1) {
        perror("Error reading numbers from FIFO");
        free(arr);
        close(fd);
        return 4;
    }
    close(fd);

    // Calculate the sum
    int sum = 0;
    printf("Received numbers:\n");
    for (int i = 0; i < count; i++) {
        printf("%d\n", arr[i]);
        sum += arr[i];
    }
    printf("Sum: %d\n", sum);

    // Save the result to output.txt
    FILE* outputFile = fopen("output.txt", "w");
    if (!outputFile) {
        perror("Error opening output file");
        free(arr);
        return 5;
    }

    // Write all received numbers and the sum to the file
    fprintf(outputFile, "Received numbers:\n");
    for (int i = 0; i < count; i++) {
        fprintf(outputFile, "%d\n", arr[i]);
    }
    fprintf(outputFile, "Sum: %d\n", sum);
    fclose(outputFile);

    // Clean up
    free(arr);
    return 0;
}

