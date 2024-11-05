#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> // dung de open file fifo 

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int main (int argc, char* argv[]) {
    //Tao file fifo
    //mkfifo("myfifo1", 0777); //0777 quyen truy cap (0777 co the dc doc hoac ghi boi bat ki ai)
    //Kiem tra trang thai code 
    if (mkfifo("myfifo1", 0777) == -1) {
        if (errno != EEXIST) {
            printf("Could not create fifo file\n"); 
            return 1;
        }
    } 
    printf("Opening...\n");
    //int fd = open("myfifo1", O_WRONLY); //mo file fifo
    int fd = open("myfifo1", O_RDWR);
    if (fd == -1) {
      return 3;
    }
    printf("Opened\n");
    //int x = 97;
    int x = 'Z';
    //File descriptor
    if (write(fd, &x, sizeof(x)) == -1) {
        return 2;
    }
    printf("Written\n");
    close(fd); 
    printf("Closed\n");
    return 0;
}


