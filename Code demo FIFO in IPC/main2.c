#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h> // dung de open file fifo 

int main (int argc, char* argv[]) { 
    int arr[5];
    int fd = open("sum", O_RDWR);
    
    if (fd == -1) {
        return 1;
    }
    
    int i;
    for (i = 0; i < 5; i++) {
        if (read(fd, &arr[i], sizeof(int)) == -1) {
            return 2;
        }
        //sum  += arr[i];
        printf("Received %d\n", arr[i]);
    } 
    //write(fd, &sum, sizeof(int));
    close(fd);
    
    int sum = 0;
    for (i = 0; i < 5; i++) {
        sum  += arr[i];
    } 
    printf("sum: %d\n", sum);
    //int fd1 = open("sum", O_WRONLY);
    //write(fd1, &sum, sizeof(int));
    //close(fd1);
    
    return 0;
}
