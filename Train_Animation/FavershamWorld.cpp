//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  FILE NAME: RailwayWorld.cpp
//  See Lab02.pdf for details
//  ========================================================================

#include <math.h>
#include <GL/freeglut.h>
#include "FavershamModels.h"

bool trainMove = true;
float train_y_rotation = 100;
float train_speed = 0.5;
int viewMode = 0;
int value = 0;
int medRadius = 200;
double wagonTheta = 22 / 120;
float barrierAngle = -90;
double trackAngle = atan(wagonTheta);
float cameraAngle = 0;
float look_x = 0;
float look_y = 0;
float look_z = 0;
float eye_x = -80;
float eye_y = 50;
float eye_z = 400; //Camera parameters


//---------------------------------------------------------------------
void initialize(void) 
{
    float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};

    loadTexture();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

    //Creates and defines a spotlight
    glEnable(GL_LIGHT1);

    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0.01);

//  Changes so that the lighting accepts the vertices definitions for the colour material
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 50);

//	Define light's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glClearColor (0.3, 0.3, 0.7, 1.0);  //Background colour is dark sky colour

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60., 1.0, 10.0, 1000.0);   //Perspective projection
}

//-------------------------------------------------------------------
void display(void)
{
    float lgt_pos[] = {0.0f, 100.0f, 0.0f, 1.0f};  //light0 position (directly above the origin)
    float spotlgt_dir[] = {-1.0f, -1.0f, 0.0f, 0.0f};
    float spotlgt_pos[] = {-10.0f, 14.0f, 0.0f, 1.0f}; //light1 position, headlight on the locomotive

    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eye_x, eye_y, eye_z, look_x, look_y, look_z, 0, 1, 0);
    
    glLightfv(GL_LIGHT0, GL_POSITION, lgt_pos);   //light position

    floor();
    station(medRadius + 70); //Middle of station is around 70 units away from the track
    
    glPushMatrix();
        glRotatef(train_y_rotation, 0, 1, 0);
        glTranslatef(0, 1, -medRadius);
        glLightfv(GL_LIGHT1, GL_POSITION, spotlgt_pos);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotlgt_dir);
        engine();		 //locomotive
    glPopMatrix();


    for(int i = 1; i < 4; i++) {
        glPushMatrix();
            //Negative angle corresponds to rotation in the clockwise direction
            glRotatef(-6.5 * i + train_y_rotation, 0, 1, 0);
            glTranslatef(0, 1, -medRadius);
            wagon();
        glPopMatrix();
    }

    glPushMatrix();
        glTranslatef(medRadius, 0, 0);
        glRotatef(-10, 0, 1, 0);
        tunnel(medRadius); //Tunnel is placed around the track
    glPopMatrix();

    tracks(medRadius, 10);  //mean radius 120 units, width 10 units
    asphalt(medRadius, 10); //Complimentary asphalt underneath tracks

    glPushMatrix();
        glRotatef(25, 0, 1, 0);
        glTranslatef(-(medRadius - 10), 0, 0);
        glRotatef(-90, 1, 0, 0);
        trafficLight(trainMove);
    glPopMatrix();

    glPushMatrix();
        glRotatef(187, 0, 1, 0);
        glTranslatef(0, 5, medRadius + 20);
        barrier(barrierAngle);
    glPopMatrix();

    glPushMatrix();
        glRotatef(7, 0, 1, 0);
        glTranslatef(0, 5, medRadius + 20);
        barrier(barrierAngle);
    glPopMatrix();

    road();

    glutSwapBuffers();   //Useful for animation
}

void controls(int key, int x, int y)
{
    if (key == GLUT_KEY_END) trainMove = !trainMove;
    if (key == GLUT_KEY_LEFT) cameraAngle -= 0.1;  //Change direction
    else if (key == GLUT_KEY_RIGHT) cameraAngle += 0.1;
    if (key == GLUT_KEY_UP)
    { //Move forward
        eye_x += 5 * sin(cameraAngle);
        eye_z -= 5 * cos(cameraAngle);
    }
    else if (key == GLUT_KEY_DOWN)
    {  //Move backward
        eye_x -= 5 * sin(cameraAngle);
        eye_z += 5 * cos(cameraAngle);
    }
    else if (key == GLUT_KEY_PAGE_UP)
    {  //Move upward
        eye_y++;
    }
    else if (key == GLUT_KEY_PAGE_DOWN)
    {
        //Move downward
        eye_y--;
    }

    if(trainMove) {
        train_speed = 0.5;
    } else {
        train_speed = 0;
    }
    look_x = eye_x + 100 * sin(cameraAngle);
    look_z = eye_z - 100 * cos(cameraAngle);

//    if(key == GLUT_KEY_RIGHT) train_y_rotation--;
//    else if(key == GLUT_KEY_LEFT) train_y_rotation++;
    glutPostRedisplay();
}

void timer(int value) {
    value++;
    if (value < 90) {
        barrierAngle++;
    } 
    if (value > 630 && value < 720) {
        barrierAngle--;
    }
    if (value > 1000) {
        value = 0;
        trainMove = true;
    }
    if(value < 720) {
        train_y_rotation += train_speed;
    } else {
        trainMove = false;
    }
    glutPostRedisplay();
    glutTimerFunc(10, timer, value);
}

//---------------------------------------------------------------------
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE|GLUT_DEPTH);
   glutInitWindowSize (600, 600); 
   glutInitWindowPosition (50, 50);
   glutCreateWindow ("Cosc363 Assignment 1 - Toby Morgan");
   initialize();

   glutDisplayFunc(display); 

   glutTimerFunc(10, timer, value);
   glutSpecialFunc(controls);
   glutMainLoop();
   return 0;
}
