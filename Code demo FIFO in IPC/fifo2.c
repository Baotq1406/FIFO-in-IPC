//Process 2

#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>

int main()
{
    FILE *fp;
    int fd1, fd2, l=1, w=1, c=0;
    char *myfifo1="file1", str1[100], ch;
    char *myfifo2="file2", str2[100];
    
    // Read sentences from the first myfifo1 file
    fd1 = open(myfifo1, O_RDONLY);
    read(fd1, str1, sizeof(str1));
    printf("\n Pipe1: %s", str1);

    // Count the number of lines, words, and characters in the sentence
    int i = 0;
    while (str1[i] != '\0') {
        if (str1[i] == '.' || str1[i] == '\n') {
            l++;
        } else if (str1[i] == ' ') {
            w++;
        } else {
            c++;
        }
        i++;
    }

    // Check the result
    printf("\nNo of lines: %d", l);
    printf("\nNo of words: %d", w);
    printf("\nNo of characters: %d\n", c);
    
 // Write the counted line, word, and character data in a text file
    fp = fopen("kp.txt", "w");
    fprintf(fp, "Number of lines: %d\n", l);
    fprintf(fp, "Number of words: %d\n", w);
    fprintf(fp, "Number of characters: %d\n", c);
    fclose(fp);

    // Read the content of the text file into str2
    fp = fopen("kp.txt", "r");
    i = 0;
    while ((ch = fgetc(fp)) != EOF) {
        str2[i] = ch;
        i++;
    }
    str2[i] = '\0';
    fclose(fp);
    close(fd1);

    // Write data of the text file on the second FIFO pipe
    fd2 = open(myfifo2, O_WRONLY);
    write(fd2, str2, strlen(str2) + 1);
    close(fd2);

    return 0;
}
