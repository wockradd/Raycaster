#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define PI 3.1415926
#define MAP_SIZE 60
#define PLAYER_SIZE MAP_SIZE/3
#define MAP_X 8
#define MAP_Y 8


float playerX = MAP_SIZE*1.5, playerY = 1.5*MAP_SIZE;
float playerAngle = 0.0;  //the angle the player is facing
float deltaX, deltaY;
int map[] = 
    {1,1,1,1,1,1,1,1
    ,1,0,0,0,1,0,0,1
    ,1,0,0,1,1,0,0,1
    ,1,0,0,0,0,0,0,1
    ,1,0,0,0,0,0,0,1
    ,1,0,1,0,0,0,0,1
    ,1,0,0,0,0,0,0,1
    ,1,1,1,1,1,1,1,1};










void drawMap(){
    //loop through map
    for(int i=0 ; i<MAP_Y ; i++){
        for(int j=0 ; j<MAP_X; j++){

            //decide on colour
            if(map[i*MAP_Y+j]==1){
                glColor3f(255,0,0);
            }else{
                glColor3f(0,0,0);
            }

            //draw
            glBegin(GL_QUADS);
                    glVertex2f(MAP_SIZE*j+1, MAP_SIZE*i+1);
                    glVertex2f(MAP_SIZE*j+MAP_SIZE-1, MAP_SIZE*i+1);
                    glVertex2f(MAP_SIZE*j+MAP_SIZE-1, MAP_SIZE*i+MAP_SIZE-1);
                    glVertex2f(MAP_SIZE*j+1, MAP_SIZE*i+MAP_SIZE-1);
                glEnd();
        }
    }
    
}


void drawPlayer(){
    glPointSize(PLAYER_SIZE);
    glColor3f(0,255,0);
    glBegin(GL_POINTS);
        glVertex2f(playerX, playerY);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
        glVertex2f(playerX, playerY);
        glVertex2f(playerX + cos(playerAngle)*PLAYER_SIZE, playerY + sin(playerAngle)*PLAYER_SIZE);
    glEnd();

}





void display()
{
    glClear(GL_COLOR_BUFFER_BIT);//not sure
    glutSwapBuffers();//not sure
    
    
    drawMap();
    drawPlayer();

    glFlush();//not sure
}



//not sure why we need int x and y as args
void buttons(unsigned char key, int x, int y){
    switch(key){
        case 'w':
            playerX += cos(playerAngle)*PLAYER_SIZE/4;
            playerY += sin(playerAngle)*PLAYER_SIZE/4;
            break;
        case 'd':
            playerAngle += 0.1;
            if(playerAngle >= 2*PI){playerAngle = 0;}
            break;
        case 's':
            playerX -= cos(playerAngle)*PLAYER_SIZE/4;
            playerY -= sin(playerAngle)*PLAYER_SIZE/4;
            break;
        case 'a':
            playerAngle -= 0.1;
            if(playerAngle <= 0){playerAngle = 2*PI;}
            break;
    }
    glutPostRedisplay();//force redraw
}

void init(){
    glutInitDisplayMode(GLUT_SINGLE);//not sure
    glutInitWindowSize(1024, 512);
    glutCreateWindow("Babys first game");
    glClearColor(0.3,0.3,0.3,0);//set bg colour to grey
    gluOrtho2D(0,1024,512,0);//sets x, y co-ords
}



int main(int argc, char** argv)
{	
    glutInit(&argc, argv);
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();
    return 0;
}