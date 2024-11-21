#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<ctype.h>

int main()
{
    FILE *fp;
    int fd1, fd2, l = 0, w = 0, c = 0;
    char *myfifo1 = "file1", str1[1000], ch;
    char *myfifo2 = "file2", str2[1000];

    // Open the first FIFO pipe (file1) for reading
    fd1 = open(myfifo1, O_RDONLY);
    if (fd1 == -1) {
        perror("Error opening fifo1 for reading");
        return 1;
    }

    // Read the entire data from fifo1
    read(fd1, str1, sizeof(str1));
    printf("\nReceived from Pipe1:\n%s", str1);

    // Close fifo1 after reading
    close(fd1);

    // Count the number of sentences, words, and characters in the input data
    int i = 0;
    int in_word = 0;  // To handle word boundary detection
    while (str1[i] != '\0') {
        c++;  // Counting all characters including spaces and punctuation

        // Count sentences based on '.', '!', or '?'
        if (str1[i] == '.' || str1[i] == '!' || str1[i] == '?') {
            // Check if the next character is a space or newline (indicating the end of the sentence)
            if (str1[i + 1] == ' ' || str1[i + 1] == '\n' || str1[i + 1] == '\0') {
                l++;  // Increment sentence count
            }
        }

        // Counting words based on spaces and punctuation
        if (isspace(str1[i]) || str1[i] == '\n') {
            if (in_word) {
                w++;  // End of a word
                in_word = 0;
            }
        } else {
            in_word = 1;  // Inside a word
        }

        i++;
    }

    // If the last part is a word, count it
    if (in_word) {
        w++;
    }

    // Check the result
    printf("\nNumber of sentences: %d", l);
    printf("\nNumber of words: %d", w);
    printf("\nNumber of characters: %d\n", c);

    // Write the counted sentence, word, and character data into a text file
    fp = fopen("kp.txt", "w");
    if (fp == NULL) {
        perror("Error opening file kp.txt for writing");
        return 1;
    }
    fprintf(fp, "Number of sentences: %d\n", l);
    fprintf(fp, "Number of words: %d\n", w);
    fprintf(fp, "Number of characters: %d\n", c);
    fclose(fp);

    // Read the content of the text file into str2
    fp = fopen("kp.txt", "r");
    if (fp == NULL) {
        perror("Error opening file kp.txt for reading");
        return 1;
    }

    i = 0;
    while ((ch = fgetc(fp)) != EOF) {
        str2[i] = ch;
        i++;
    }
    str2[i] = '\0';
    fclose(fp);

    // Open the second FIFO pipe (file2) for writing
    fd2 = open(myfifo2, O_WRONLY);
    if (fd2 == -1) {
        perror("Error opening fifo2 for writing");
        return 1;
    }

    // Write the result (content from kp.txt) into fifo2
    write(fd2, str2, strlen(str2) + 1);
    close(fd2);

    return 0;
}

