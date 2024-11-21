#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>

int main(int argc, char *argv[])
{
    int fd1, fd2;
    char *myfifo1 = "file1", str1[1000], str2[1000];
    char *myfifo2 = "file2";
    FILE *input_file;

    // Check if input file is provided as an argument
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Create FIFO pipes
    mkfifo(myfifo1, 0666);
    mkfifo(myfifo2, 0666);

    // Open the input file (argv[1] is the file path)
    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("Failed to open input file");
        return 1;
    }

    // Read the entire content of the input file into str1
    size_t len = 0;
    while (fgets(str1 + len, sizeof(str1) - len, input_file)) {
        len = strlen(str1);  // Update length after each read
    }
    fclose(input_file);  // Close the file after reading

    // Write the entire content to the first FIFO pipe
    fd1 = open(myfifo1, O_WRONLY);
    write(fd1, str1, strlen(str1) + 1);  // Include the null terminator
    close(fd1);

    // Read data from the second FIFO pipe
    fd2 = open(myfifo2, O_RDONLY);
    read(fd2, str2, 1000);

    // Print the message received from FIFO2
    printf("\nMessage from FIFO2 process:\n%s", str2);
    close(fd2);

    return 0;
}

