#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#define FIFO_NAME "myfifo"
#define FIFO_NAME2 "myfifo2"
#define CHILD_COUNT 4
void signal_handler(int signum);
float sum1, sum2;               //saving important values as global
int signal_count = 0;
int t1Score = 0, t2Score = 0;
pid_t child_pids[5] = {0};  // array to store child PIDs

int main(int argc, char *argv[]) {
    if (mkfifo(FIFO_NAME, 0777) == -1) {                //parent procChild FIFO
        if (errno != EEXIST) {
            perror("Couldn't create file fifo");
            exit(1);
        }
    }
    if (mkfifo(FIFO_NAME2, 0777) == -1) {           //parent Display FIFO
        if (errno != EEXIST) {
            perror("Couldn't create file fifo");
            exit(1);
        }
    }
    pid_t pid;
    int child_count = 0;
    int rounds;
    if (argc < 2)
        rounds = 5;             //reading argument
    else
        rounds = atoi(argv[1]);

    struct sigaction sa = {0};
    sa.sa_handler = &signal_handler;
    sigaction(SIGUSR1, &sa, NULL); // set up signal handler for SIGUSR1
    for (int i = 0; i < CHILD_COUNT + 2; i++) {
        pid = fork();
        sleep(1);
        if (pid < 0) {
            perror("Fork failed\n");
            exit(1);
        } else if (pid == 0) {
            if (i < 4) {
                // child process
                pid_t currentProc = getpid(); // Get the PID
                printf("Child\n");
                usleep(300000);
                char filename[15];
                sprintf(filename, "%d.txt", currentProc); // Generate the filename after PID
                char *i1 = (char *) malloc(sizeof(int));
                sprintf(i1, "%d", i + 1);
                if (execl("./child", "./child", filename, i1, (char *) NULL) == -1) {       //4 competitors
                    perror("Exec Error");
                    exit(-1);
                }
            } else if (i > 4) {
                if (execl("./Display", "./Display", (char *) NULL) == -1) {                        // display process
                    perror("Exec Error");
                    exit(-1);
                }
            } else {
                char temp[10];
                sprintf(temp, "%d", rounds);
                if (execl("./procChild", temp, (char *) NULL) == -1) {          //process child
                    perror("Exec Error fifth child");
                    exit(-1);
                }
            }
        }
        child_pids[child_count] = pid;              //saving processes pid for later use
        child_count++;
    }
    //Building a simple UI
    for (int round_num = 0; round_num < rounds; round_num++) {
        char sumMsg[20] = {0};
        //Create FIFO file
        int fd;
        // generate two random integers between 0 and 99
        sleep(1);
        srand(time(NULL));              //generating randoms

        int min = rand() % 100;
        int max = rand() % 100;
        while (min==max){                   //if randomly min was equal to max (unlikely)
            srand(time(NULL));
            min = rand() % 100;
            max = rand() % 100;
        }
        // swap min and max if min was more than max
        if (min > max) {
            int temp = min;
            min = max;
            max = temp;
        }
        // open file for writing
        printf("Writing to File\n");
        FILE *f2 = fopen("range.txt", "w");
        // write comma-separated values to file
        fprintf(f2, "%d,%d", min, max);
        // close file
        fclose(f2);
        printf("Done Writing\n");
        sleep(1);
        for (int i = 0; i < CHILD_COUNT; i++) {             //signaling 4 children that the min and max are ready
            kill(child_pids[i], SIGUSR1);
        }
        // wait for all children to finish generating numbers
        printf("waiting for signal\n");
        while (signal_count < CHILD_COUNT) {
        }

        //reading files
        char msg[60] = {0};
        for (int j = 0; j < CHILD_COUNT; j++) {
            char childFp[10] = {0};
            char childFiles[15] = {0};
            FILE *temp;
            sprintf(childFiles, "%d.txt", child_pids[j]);
            temp = fopen(childFiles, "r");
            fscanf(temp, "%s", childFp);
            if (j < 3) {
                strcat(childFp, ",");
            }
            strcat(msg, childFp);               //getting the generated numbers by 4 children
        }
        printf("%s\n", msg);
        //Write on FIFO
        fd = open(FIFO_NAME, O_WRONLY);                 //open a fifo to communicate with the procChild and write the generated values
        if (fd == -1) {
            perror("Open Error");
            exit(1);
        }
        ssize_t bytes_written=write(fd, msg, strlen(msg) + 1);
        if (bytes_written == -1) {
            perror("Write Error");
            exit(1);
        }
        close(fd);

        //Read from FIFO
        fd = open(FIFO_NAME, O_RDONLY);             //opening fifo to read the procchild output
        if (fd == -1) {
            perror("Open Error");
            exit(1);
        }
        if (read(fd, sumMsg, sizeof(sumMsg)) == -1)
            perror("Error reading FIFO file\n");

        close(fd);
        printf("Received sum from co-processor: %s\n", sumMsg);

        //Doing comparison...
        sscanf(sumMsg, "%f,%f", &sum1, &sum2);
        int winTemp = 1;
        if (sum1 > sum2)
            t1Score++;
        else {
            t2Score++;
            winTemp = 2;
        }
        char temp[35];
        sprintf(temp, ",%d,%d,%d,%d,%d,%d,%d,%d", t1Score, t2Score, child_pids[0], child_pids[1], child_pids[2],
                child_pids[3], winTemp, rounds);
        strcat(msg, temp);

        // reset signal count for next round
        signal_count = 0;
        printf("Round %d finished.\n", round_num + 1);
        int fd2;

        //Write on FIFO2
        fd2 = open(FIFO_NAME2, O_WRONLY);
        write(fd2, msg, strlen(msg) + 1);
        close(fd2);
        printf("----------------------------------------------------------------------\n");             //display fifo to keep updating the screan
    }
    printf("Team 1 score: %d\nTeam 2 score: %d\n", t1Score, t2Score);
    // kill child processes
    for (int i = 0; i < CHILD_COUNT + 1; i++) {
        kill(child_pids[i], SIGKILL);                   //killing the 4 children after finishing rounds
    }
    unlink(FIFO_NAME);              //removing fifos
    unlink(FIFO_NAME2);
    printf("Parent process finished.\n");
    sleep(2);
    if (t1Score > t2Score)
        printf("The winner is team one with score %d-%d\n", t1Score, t2Score);
    else if (t1Score < t2Score)
        printf("The winner is team two with score %d-%d\n", t2Score, t1Score);
    else
        printf("Tie with score %d-%d\n", t1Score, t2Score);             //declaring winners
    for (int j = 0; j < CHILD_COUNT; j++) {
        char str[15];                                                           //removing files
        sprintf(str, "%d.txt", child_pids[j]);
        remove(str);
    }
    remove("range.txt");                                            //removing range.txt
    sleep(10);
    kill(child_pids[5], SIGKILL);                           //killing the screan
    return 0;
}
void signal_handler(int signum) {
    signal_count++;
    printf("Signal Count = %d\n", signal_count);
}
