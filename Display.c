#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <GL/glut.h>

#define FIFO_NAME2 "myfifo2"

void updateScrean();
void display();
void renderText(float x, float y, void *font, char *string);
void display1();
char msg[60] = {0}, resMsg[15] = {0};
float v1, v2, v3, v4, sum1, sum2;
char sum1_str[50], sum2_str[50];
int child_pids[4];
int fd2, t1Score, t2Score, roundWinner, rounds, rCnt = 0;               //declaring data we want to display as global

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow(" Team Competition ");
    glutDisplayFunc(display);
    glutTimerFunc(3000, updateScrean, 0);
    glutMainLoop();
}
void updateScrean() {
    fd2 = open(FIFO_NAME2, O_RDONLY);
    sleep(1);
    read(fd2, msg, sizeof(msg));
    close(fd2);

    sscanf(msg, "%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d", &v1, &v2, &v3, &v4, &t1Score, &t2Score, &child_pids[0],
           &child_pids[1], &child_pids[2], &child_pids[3], &roundWinner, &rounds);
    sum1 = v1 + v2;
    sum2 = v3 + v4;

    sprintf(resMsg, "%.2f,%.2f", sum1, sum2);
    glutPostRedisplay(); // Trigger a screen update
    rCnt++;
    printf("......%d......\n", rCnt);
    if (rCnt == rounds)
        glutTimerFunc(3000, display1, 0); // Set the next timer callback                    displaying the final result screan
    else if (rCnt < rounds)
        glutTimerFunc(3000, updateScrean, 0); // Set the next timer callback               displaying the new data on screan
}

void renderText(float x, float y, void *font, char *string) {
    glRasterPos2f(x, y);

    int len = strlen(string);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]);
    }
}

void display() {
    char temp[12][15];

    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0f, 1.0f, 1.0f);

    float redTeam[3] = {1.0f, 0.0f, 0.0f};
    float blueTeam[3] = {0.0f, 0.0f, 1.0f};
    float white[3] = {1.0f, 1.0f, 1.0f};

    char tmp[10];
    sprintf(tmp, "Round %d", rCnt);
    renderText(-0.07f, 0.7f, GLUT_BITMAP_HELVETICA_18, tmp);


    // Draw Team1
    glColor3f(redTeam[0], redTeam[1], redTeam[2]);
    glColor3f(redTeam[0], redTeam[1], redTeam[2]);
    renderText(-0.54f, 0.7f, GLUT_BITMAP_HELVETICA_18, "Team 1");
    sprintf(temp[0], "%d", t1Score);
    renderText(-0.47f, 0.6f, GLUT_BITMAP_HELVETICA_18, temp[0]);
    sprintf(temp[1], "Player %d", child_pids[0]);
    renderText(-0.79f, 0.5f, GLUT_BITMAP_HELVETICA_12, temp[1]);
    sprintf(temp[2], "%.2f", v1);
    renderText(-0.72f, 0.4f, GLUT_BITMAP_HELVETICA_12, temp[2]);
    sprintf(temp[3], "Player %d", child_pids[1]);
    renderText(-0.3f, 0.5f, GLUT_BITMAP_HELVETICA_12, temp[3]);
    sprintf(temp[4], "%.2f", v2);
    renderText(-0.24f, 0.4f, GLUT_BITMAP_HELVETICA_12, temp[4]);


    // Draw Team2
    glColor3f(blueTeam[0], blueTeam[1], blueTeam[2]);
    renderText(0.43f, 0.7f, GLUT_BITMAP_HELVETICA_18, "Team 2");
    sprintf(temp[5], "%d", t2Score);
    renderText(0.5f, 0.6f, GLUT_BITMAP_HELVETICA_18, temp[5]);
    sprintf(temp[6], "Player %d", child_pids[2]);
    renderText(0.18f, 0.5f, GLUT_BITMAP_HELVETICA_12, temp[6]);
    sprintf(temp[7], "%.2f", v3);
    renderText(0.24f, 0.4f, GLUT_BITMAP_HELVETICA_12, temp[7]);
    sprintf(temp[8], "Player %d", child_pids[3]);
    renderText(0.67f, 0.5f, GLUT_BITMAP_HELVETICA_12, temp[8]);
    sprintf(temp[9], "%.2f", v4);
    renderText(0.72f, 0.4f, GLUT_BITMAP_HELVETICA_12, temp[9]);

    // Draw Sum
    glColor3f(white[0], white[1], white[2]);

    sprintf(sum1_str, "%0.2f", sum1);
    sprintf(sum2_str, "%0.2f", sum2);

    renderText(-0.625f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Team 1 result");
    renderText(-0.5f, -0.1f, GLUT_BITMAP_HELVETICA_18, sum1_str);
    renderText(0.345f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Team 2 result");
    renderText(0.45f, -0.1f, GLUT_BITMAP_HELVETICA_18, sum2_str);

    // Declare the winner
    glColor3f(0, 1, 0);
    if (roundWinner == 1)
        renderText(-0.375f, -0.5f, GLUT_BITMAP_HELVETICA_18, "The winner at this round Team 1");
    else if (roundWinner == 2)
        renderText(-0.375f, -0.5f, GLUT_BITMAP_HELVETICA_18, "The winner at this round Team 2");
    else
        renderText(-0.375f, -0.5f, GLUT_BITMAP_HELVETICA_18, "The winner at this round Team X");

    glutSwapBuffers();
    glFlush();
}

void display1() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Declare the winner
    glColor3f(0, 1, 0);
    if (t1Score > t2Score)
        renderText(-0.275f, 0, GLUT_BITMAP_HELVETICA_18, "The winner is Team 1");
    else if (t1Score < t2Score)
        renderText(-0.275f, 0, GLUT_BITMAP_HELVETICA_18, "The winner is Team 2");
    else
        renderText(-0.05F, 0, GLUT_BITMAP_HELVETICA_18, "TIE");

    glutSwapBuffers();
    glFlush();
}