#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
char *filename;
int delay;
void signal_handler(int signum);
int main(int argc, char *argv[]) {
    printf("Child process %d started\n", getpid());
    filename = argv[1];                 //getting filename
    delay = atoi(argv[2]);
    struct sigaction sa = {0};
    sa.sa_handler = &signal_handler;
    sigaction(SIGUSR1, &sa, NULL); // set up signal handler for SIGUSR1
    while (1) {
    }
}

void signal_handler(int signum) {
    usleep(500);
    printf("Child process %d entered round\n", getpid());
    FILE *fp;
    float min, max;
    int randMax = 100;
    fp = fopen("range.txt", "r");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    fscanf(fp, "%f,%f", &min, &max);
    fclose(fp);
    printf("open file\n");
    srandom(time(NULL) + delay);
    float random_num = ((float) (random() % 100) / randMax) * (99 - 0) + 0;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("fopen");
        exit(2);
    }
    // Write the random number to the output file
    fprintf(fp, "%.2f\n", random_num);          //waiting for the parent to write the range then reading it and then generate random values
    fclose(fp);
    printf("Child process %d generated %.2f\n", getpid(), random_num);
    usleep(delay * 1000);
    kill(getppid(), SIGUSR1); // send signal to parent which will update the signalcounter
    sleep(1);
}