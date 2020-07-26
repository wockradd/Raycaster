#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define PI 3.1415926
#define P2 PI/2
#define P3 3*PI/2
#define DEG 0.0174533

#define WINDOW_HEIGHT 512
#define WINDOW_WIDTH 1024

#define MAP_SIZE 500
#define MAP_CELL_SIZE   MAP_SIZE/8
#define PLAYER_SIZE    MAP_CELL_SIZE/3
#define MAP_X 8
#define MAP_Y 8
#define FOV 60




float playerX = MAP_CELL_SIZE*1.5;
float playerY = MAP_CELL_SIZE*1.5;
float playerAngle = 0.0;  //the angle the player is facing

int map[] = 
    {1,1,1,1,1,1,1,1
    ,1,0,0,0,1,0,0,1
    ,1,0,0,1,1,0,0,1
    ,1,0,0,0,0,0,0,1
    ,1,0,0,0,0,0,0,1
    ,1,0,1,0,0,0,0,1
    ,1,0,0,0,0,0,0,1
    ,1,1,1,1,1,1,1,1};







void drawMinimap(){
    //to put the map in the bottm right
    float translateY = WINDOW_HEIGHT - MAP_SIZE;

    //loop through map
    for(int i=0 ; i<MAP_Y ; i++){
        for(int j=0 ; j<MAP_X; j++){

            //pick colour based on if theres a wall there
            if(map[i*MAP_Y+j]==1){
                glColor3f(255,0,0);
            }else{
                glColor3f(0,0,0);
            }

            //draw
            glBegin(GL_QUADS);
                    glVertex2f(MAP_CELL_SIZE*j+1, translateY + MAP_CELL_SIZE*i+1);
                    glVertex2f(MAP_CELL_SIZE*j+MAP_CELL_SIZE-1,translateY + MAP_CELL_SIZE*i+1);
                    glVertex2f(MAP_CELL_SIZE*j+MAP_CELL_SIZE-1,translateY + MAP_CELL_SIZE*i+MAP_CELL_SIZE-1);
                    glVertex2f(MAP_CELL_SIZE*j+1,translateY + MAP_CELL_SIZE*i+MAP_CELL_SIZE-1);
                glEnd();
        }
    }

    //draw the player, just a point with a line showing where they're facing
    glPointSize(PLAYER_SIZE);
    glColor3f(0,255,0);
    glLineWidth(3);

    glBegin(GL_POINTS);
        glVertex2f(playerX, translateY + playerY);
    glEnd();

    
    glBegin(GL_LINES);
        glVertex2f(playerX,translateY + playerY);
        glVertex2f(playerX + cos(playerAngle)*PLAYER_SIZE, translateY +playerY + sin(playerAngle)*PLAYER_SIZE);
    glEnd();
}








//tank controls
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


float distance(float ax, float bx, float ay, float by){
    return sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay));
}


void castRays(){
    float rayY, rayX, xOffset, yOffset;
    int mapX, mapY,mapIndex;
    int depth, maxDepth=8; //how many times we loop before giving up
    float rayAngle = playerAngle-FOV/2*DEG;
    if(rayAngle<0){
        rayAngle += 2*PI;
    }else if(rayAngle > 2*PI){
        rayAngle -= 2*PI;
    }

    for(int i=0  ;i<FOV ; i++){        
        //horizontal collision
        float hX, hY, hDistance=9999999999;
        depth=0;
        float aTan = -1/tan(rayAngle);
        if(rayAngle==PI || rayAngle==0){//looking straight left or right, never gonna collide
            rayX = playerX;
            rayY = playerY;
            depth = 8;
        }else{
            if(rayAngle > PI){//looking up
                rayY = ((int)playerY)/((int)MAP_CELL_SIZE)*((int)MAP_CELL_SIZE)-0.0001;
                yOffset = -MAP_CELL_SIZE;
            }else{//looking down
                rayY = (((int)playerY)/((int)MAP_CELL_SIZE)*((int)MAP_CELL_SIZE))+MAP_CELL_SIZE;
                yOffset = MAP_CELL_SIZE;
                
            }
            rayX = (playerY-rayY)*aTan+playerX;
            xOffset = -yOffset*aTan;
        }
        while(depth < maxDepth){
            mapX = ((int)rayX)/((int)MAP_CELL_SIZE);
            mapY = rayY/((int)MAP_CELL_SIZE);
            mapIndex = mapY*MAP_Y+mapX;
            if(mapIndex >= 0 && mapIndex < MAP_Y*MAP_X && map[mapIndex]==1){//hit a wall
                depth = maxDepth;
                hX = rayX;
                hY = rayY;
                hDistance = distance(playerX, hX, playerY, hY);
            }else{
                rayX += xOffset;
                rayY += yOffset;
                depth++;
            }
        }

        


        //verticalLine check
        float vX, vY, vDistance=9999999999;
        depth=0;
        float nTan = -tan(rayAngle);

        if(rayAngle==P2 || rayAngle==P3){//looking straight up or down, never gonna collide
            rayX = playerX;
            rayY = playerY;
            depth = 8;
        }else{
            if(rayAngle > P2 && rayAngle < P3){//looking up
                rayX = ((int)playerX)/((int)MAP_CELL_SIZE)*((int)MAP_CELL_SIZE)-0.0001;
                xOffset = -MAP_CELL_SIZE;
            }else{//looking down
                rayX = (((int)playerX)/((int)MAP_CELL_SIZE)*((int)MAP_CELL_SIZE))+MAP_CELL_SIZE;
                xOffset = MAP_CELL_SIZE;
                
            }
            rayY = (playerX-rayX)*nTan+playerY;
            yOffset = -xOffset*nTan;
        }
        while(depth < maxDepth){
            mapX = ((int)rayX)/((int)MAP_CELL_SIZE);
            mapY = rayY/((int)MAP_CELL_SIZE);
            mapIndex = mapY*MAP_Y+mapX;
            if(mapIndex >= 0 && mapIndex < MAP_Y*MAP_X && map[mapIndex]==1){//hit a wall
                depth = maxDepth;
                vX = rayX;
                vY = rayY;
                vDistance = distance(playerX, vX, playerY, vY);
            }else{
                rayX += xOffset;
                rayY += yOffset;
                depth++;
            }
        }


        if(hDistance < vDistance){
            rayX = hX;
            rayY = hY;
        }

        glColor3f(0,255,0);
        glLineWidth(1);
        glBegin(GL_LINES);
            glVertex2f(playerX,playerY+WINDOW_HEIGHT - MAP_SIZE);
            glVertex2f(rayX,rayY+WINDOW_HEIGHT - MAP_SIZE);
        glEnd();


        rayAngle +=DEG;
        if(rayAngle<0){
            rayAngle += 2*PI;
        }else if(rayAngle > 2*PI){
            rayAngle -= 2*PI;
        }
    }
    

}




void display(){
    glClear(GL_COLOR_BUFFER_BIT);//not sure
    glutSwapBuffers();//not sure
    
    drawMinimap();
    castRays();

    glFlush();//not sure
}





void init(){
    glutInitDisplayMode(GLUT_SINGLE);//not sure
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Babys first game");
    glClearColor(0.3,0.3,0.3,0);//set bg colour to grey
    gluOrtho2D(0,WINDOW_WIDTH,WINDOW_HEIGHT,0);//sets x, y co-ords
}




int main(int argc, char** argv){	
    glutInit(&argc, argv);
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();
    return 0;
}