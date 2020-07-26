#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define PI 3.1415926
#define P2 PI/2
#define P3 3*PI/2
#define DEG 0.0174533

#define WINDOW_HEIGHT 512
#define WINDOW_WIDTH 1024

#define MAP_SIZE 150//should be a multiple of MAP_ARRAY
#define MAP_ARRAY 15//the size of our map in cells, maps always square
#define MAP_CELL_SIZE   MAP_SIZE/MAP_ARRAY
#define PLAYER_SIZE    MAP_CELL_SIZE/3
#define FOV 60
#define VIEW_STRIP    WINDOW_WIDTH/FOV
#define TURNING_ANGLE 0.07
#define SPEED    PLAYER_SIZE/4




//puts them in the middle of cell 1,1 facing right
float playerX = MAP_CELL_SIZE*1.5;
float playerY = MAP_CELL_SIZE*1.5;
float playerAngle = 0.0; 



int map[] = 
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
    ,1,0,0,0,1,0,0,0,0,0,3,0,0,0,1
    ,1,0,0,0,1,0,0,0,0,0,3,0,0,0,1
    ,1,0,0,0,1,0,0,0,0,2,2,2,0,0,1
    ,1,1,0,1,1,1,1,0,0,0,0,0,0,0,1
    ,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1
    ,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1
    ,1,2,2,2,0,0,0,0,1,0,0,0,0,0,1
    ,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1
    ,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1
    ,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1
    ,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1
    ,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1
    ,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    ,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};


struct Ray{
    float distance;
    bool vertical;
    int wallType;
};

Ray rays[FOV];








void drawMinimap(){
    //to put the map in the bottm right
    float translateY = WINDOW_HEIGHT - MAP_SIZE;

    //draw the maps border
    glColor3f(0.2,0.2,0.2);
    glBegin(GL_QUADS);
        glVertex2f(0, translateY);
        glVertex2f(MAP_SIZE, translateY);
        glVertex2f(MAP_SIZE, translateY + MAP_SIZE);
        glVertex2f(0, translateY + MAP_SIZE);
    glEnd();

    //loop through map
    for(int i=0 ; i<MAP_ARRAY ; i++){
        for(int j=0 ; j<MAP_ARRAY; j++){

            //pick colour based on wall type
            switch(map[i*MAP_ARRAY+j]){
                case 0: 
                    glColor3f(0,0,0);
                    break;
                case 1: 
                    glColor3f(255,0,0);
                    break;
                case 2: 
                    glColor3f(0,255,0);
                    break;
                case 3: 
                    glColor3f(0,0,255);
                    break;

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

    glBegin(GL_POINTS);
        glVertex2f(playerX, translateY + playerY);
    glEnd();

    glLineWidth(1);
    glBegin(GL_LINES);
        glVertex2f(playerX,translateY + playerY);
        glVertex2f(playerX + cos(playerAngle)*PLAYER_SIZE*2, translateY +playerY + sin(playerAngle)*PLAYER_SIZE*2);
    glEnd();
}



//TODO
void drawView(){

    //draw the floor
    glColor3f(0,0,0);
    glBegin(GL_QUADS);
        glVertex2f(0, WINDOW_HEIGHT/2);
        glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT/2);
        glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
        glVertex2f(0,WINDOW_HEIGHT);
    glEnd();
}





//helper function for casting rays
float distance(float ax, float bx, float ay, float by){
    return sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay));
}




//casts a bunch o rays
void castRays(){
    float rayY, rayX, xOffset, yOffset;
    int mapX, mapY,mapIndex, hWallType, vWallType;
    int depth, maxDepth=MAP_ARRAY; //how many times we loop before giving up

    float rayAngle = playerAngle-FOV/2*DEG;
    if(rayAngle<0){//constrain angle
            rayAngle += 2*PI;
    }else if(rayAngle > 2*PI){
        rayAngle -= 2*PI;
    }
    

    for(int i=0  ;i<FOV ; i++){        
        //HORIZONTAL RAY CHECK
        float hX, hY, hDistance=9999999999;
        depth=0;
        float aTan = -1/tan(rayAngle);

        //initial maths to find the ray cords and offsets
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

        //figure out when we hit a wall
        while(depth < maxDepth){
            mapX = ((int)rayX)/((int)MAP_CELL_SIZE);
            mapY = rayY/((int)MAP_CELL_SIZE);
            mapIndex = mapY*MAP_ARRAY+mapX;
            if(mapIndex >= 0 && mapIndex < MAP_ARRAY*MAP_ARRAY && map[mapIndex]!=0){//hit a wall
                depth = maxDepth;
                hWallType =  map[mapIndex];
                hX = rayX;
                hY = rayY;
                hDistance = distance(playerX, hX, playerY, hY);
            }else{
                rayX += xOffset;
                rayY += yOffset;
                depth++;
            }
        }

        


        //VERTICAL RAY CHECK
        //basically identidentical to horizontal check
        float vX, vY, vDistance=9999999999;
        depth=0;
        float nTan = -tan(rayAngle);


        //initial maths to find the ray cords and offsets
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

        //figure out when we hit a wall
        while(depth < maxDepth){
            mapX = ((int)rayX)/((int)MAP_CELL_SIZE);
            mapY = rayY/((int)MAP_CELL_SIZE);
            mapIndex = mapY*MAP_ARRAY+mapX;
            if(mapIndex >= 0 && mapIndex < MAP_ARRAY*MAP_ARRAY && map[mapIndex]!=0){//hit a wall
                depth = maxDepth;
                vWallType = map[mapIndex];
                vX = rayX;
                vY = rayY;
                vDistance = distance(playerX, vX, playerY, vY);
            }else{
                rayX += xOffset;
                rayY += yOffset;
                depth++;
            }
        }

        //pick the smallest of the two
        if(hDistance < vDistance){
            rayX = hX;
            rayY = hY;
            rays[i].distance = hDistance;
            rays[i].vertical = false;
            rays[i].wallType = hWallType;
        }else{
            rays[i].distance = vDistance;
            rays[i].vertical = true;
            rays[i].wallType = vWallType;
        }

        //draw it, just for testing
        glColor3f(0,255,0);
        glLineWidth(1);
        glBegin(GL_LINES);
            glVertex2f(playerX,playerY+WINDOW_HEIGHT - MAP_SIZE);
            glVertex2f(rayX,rayY+WINDOW_HEIGHT - MAP_SIZE);
        glEnd();

        //increase angle for next ray
        rayAngle +=DEG;
        if(rayAngle<0){//constrain angle
            rayAngle += 2*PI;
        }else if(rayAngle > 2*PI){
            rayAngle -= 2*PI;
        }
    }
}




//tank controls
//not sure why we need int x and y as args
void buttons(unsigned char key, int x, int y){
    switch(key){
        case 'w':
            playerX += cos(playerAngle)*SPEED;
            playerY += sin(playerAngle)*SPEED;
            break;
        case 's':
            playerX -= cos(playerAngle)*SPEED;
            playerY -= sin(playerAngle)*SPEED;
            break;
        case 'd':
            playerAngle += TURNING_ANGLE;
            if(playerAngle >= 2*PI){playerAngle = 0;}
            break;
        case 'a':
            playerAngle -= TURNING_ANGLE;
            if(playerAngle <= 0){playerAngle = 2*PI;}
            break;
    }
    glutPostRedisplay();//force redraw
}



void display(){
    glClear(GL_COLOR_BUFFER_BIT);//not sure
    glutSwapBuffers();//not sure
    
    //cast rays first is more accurate, cast rays last to see them
    castRays();
    drawView();
    drawMinimap();
    //castRays();
    
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