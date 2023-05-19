#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define FIFO_NAME "myfifo"
int main(int argc, char *argv[]) {
    while (1) {
        int fd;
        char msg[60] = {0};
        char resMsg[15] = {0};
        float v1, v2, v3, v4, sum1, sum2;
        while ((fd = open("myfifo", O_RDONLY)) == -1) {
            // fifo still not created
            sleep(1);
        }
        if (read(fd, msg, sizeof(msg)) == -1)
            perror("Error: couldn't read FIFO file\n");
        close(fd);
        printf("Received message from parent process: %s\n", msg);              //recieving the 4 numbers from parent using fifo
        sscanf(msg, "%f,%f,%f,%f", &v1, &v2, &v3, &v4);
        sum1 = v1 + v2;
        sum2 = v3 + v4;
        sprintf(resMsg, "%.2f,%.2f", sum1, sum2);
        fd = open(FIFO_NAME, O_WRONLY);
        if (write(fd, resMsg, strlen(resMsg) + 1) == -1)
            perror("Error: couldn't write on FIFO file\n");
        close(fd);
        printf("The following message has been written of FIFO file: %s\n", resMsg);                //calculating resutl ad sending it again to parent
        sleep(1);
    }
}