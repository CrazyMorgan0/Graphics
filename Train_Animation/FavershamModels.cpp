//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  RailModels.cpp
//  A collection of functions for generating the models for a railway scene
//  ========================================================================

#include <cmath>
#include <GL/freeglut.h>
#include "FavershamModels.h"
#include "loadBMP.h"
#include "loadTGA.h"

GLUquadric* q;
GLuint txId[7];
float toRad = 3.14159265 / 180.0;  //Conversion from degrees to radians

//------Function to load a texture in bmp format  ------------------------
void loadTexture()				
{
	glGenTextures(7, txId); 				// Create a Texture object

	glBindTexture(GL_TEXTURE_2D, txId[0]);
    loadBMP("WagonTexture.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	glBindTexture(GL_TEXTURE_2D, txId[1]);
	loadTGA("station_brick.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	glBindTexture(GL_TEXTURE_2D, txId[2]);
	loadTGA("station_concrete.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	glBindTexture(GL_TEXTURE_2D, txId[3]);
	loadTGA("asphalt.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, txId[4]);
	loadBMP("Road.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, txId[5]);
	loadTGA("station_door.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, txId[6]);
	loadTGA("station_wood.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

//--------------- GROUND PLANE ------------------------------------
// This is a square shaped region on the xz-plane of size 400x400 units
// centered at the origin.  This region is constructed using small quads
// of unit size, to facilitate rendering of spotlights
//-----------------------------------------------------------------
void floor()
{
	float white[4] = {1., 1., 1., 1.};
	float black[4] = {0};
	glColor4f(0.3, 0.7, 0.3, 1.0);  //The floor is green in colour
	glNormal3f(0.0, 1.0, 0.0);

	//Change the floor colour to black to hopefully surpress some of the specular shading on the floor
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);

	//The floor is made up of several tiny squares on a 1000 x 1000 grid. Each square has a unit size.
	glBegin(GL_QUADS);
	for(int i = -500; i < 500; i++)
	{
		for(int j = -500;  j < 500; j++)
		{
			glVertex3f(i, 0, j);
			glVertex3f(i, 0, j+1);
			glVertex3f(i+1, 0, j+1);
			glVertex3f(i+1, 0, j);
		}
	}
	glEnd();

	//Resets the floor colour to white
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
}

//--------------- RAILWAY TRACK ------------------------------------
// A circular railway track of specified median radius and width
// The tracks are modelled using small quadrilateral segments each covering 5 deg arc
// The tracks have a height of 1 unit (refer to lab notes)
//-----------------------------------------------------------------
void tracks(float medRadius, float width)  
{
	float inRad  = medRadius - width * 0.5;
	float outRad = medRadius + width * 0.5;
	float angle1,angle2, ca1,sa1, ca2,sa2;
	float x1,z1, x2,z2, x3,z3, x4,z4, x7,z7, x8,z8, x9,z9, x10,z10;  //four points of a quad

	glColor4f(0.3, 0.3, 0.3, 1.0);
    glBegin(GL_QUADS);
	float radius = inRad;
	for (int i = 0; i < 2; i++)   //Two parallel tracks (radius = inRad, outRad)
	{
		for (int i = 0; i < 360; i += 5)    //5 deg intervals
		{
			angle1 = i * toRad;       //Computation of angles, cos, sin etc
			angle2 = (i + 5) * toRad;
			ca1 = cos(angle1); ca2 = cos(angle2);
			sa1 = sin(angle1); sa2 = sin(angle2);
			x1 = (radius - 0.5)*sa1; z1 = (radius - 0.5)*ca1;
			x2 = (radius + 0.5)*sa1; z2 = (radius + 0.5)*ca1;
			x3 = (radius + 0.5)*sa2; z3 = (radius + 0.5)*ca2;
			x4 = (radius - 0.5)*sa2; z4 = (radius - 0.5)*ca2;

			glNormal3f(0., 1., 0.);       //Quad 1 facing up
			glVertex3f(x1, 1.0, z1);
			glVertex3f(x2, 1.0, z2);
			glVertex3f(x3, 1.0, z3);
			glVertex3f(x4, 1.0, z4);

			glNormal3f(-sa1, 0.0, -ca1);   //Quad 2 facing inward
			glVertex3f(x1, 0.0, z1);
			glVertex3f(x1, 1.0, z1);
			glNormal3f(-sa2, 0.0, -ca2);
			glVertex3f(x4, 1.0, z4);
			glVertex3f(x4, 0.0, z4);

			glNormal3f(sa1, 0.0, ca1);   //Quad 3 facing outward
			glVertex3f(x2, 1.0, z2);
			glVertex3f(x2, 0.0, z2);
			glNormal3f(sa2, 0.0, ca2);
			glVertex3f(x3, 0.0, z3);
			glVertex3f(x3, 1.0, z3);
		}
		radius = outRad;
	}
	glEnd();
}


//--------------- MODEL BASE --------------------------------------
// This is a common base for the locomotive and wagons
// The base is of rectangular shape (20 length x 10 width x 2 height)
// and has wheels and connectors attached.
//-----------------------------------------------------------------
void base()
{
    glColor4f(0.2, 0.2, 0.2, 1.0);   //Base color
    glPushMatrix();
      glTranslatef(0.0, 4.0, 0.0);
      glScalef(20.0, 2.0, 10.0);     //Size 20x10 units, thickness 2 units.
      glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();					//Connector between wagons
      glTranslatef(11.0, 4.0, 0.0);
      glutSolidCube(2.0);
    glPopMatrix();

    //4 Wheels (radius = 2 units)
	//x, z positions of wheels:
	float wx[4] = {  -8,   8,   -8,    8 }; 
	float wz[4] = { 5.1, 5.1, -5.1, -5.1 };
    glColor4f(0.5, 0., 0., 1.0);    //Wheel color
	GLUquadric *q = gluNewQuadric();   //Disc

	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glTranslatef(wx[i], 2.0, wz[i]);
		gluDisk(q, 0.0, 2.0, 20, 2);
		glPopMatrix();
	}
}

//--------------- LOCOMOTIVE --------------------------------
// This simple model of a locomotive consists of the base,
// cabin and boiler
//-----------------------------------------------------------
void engine()
{
    base();

    //Cab
    glColor4f(0.8, 0.5, 0.2, 1);
    glPushMatrix();
      glTranslatef(7.0, 8.5, 0.0);
      glScalef(6.0, 7.0, 10.0);
      glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
      glTranslatef(6.0, 14.0, 0.0);
      glScalef(4.0, 4.0, 8.0);
      glutSolidCube(1.0);
    glPopMatrix();

    //Boiler
	GLUquadric *q = gluNewQuadric();   //Cylinder
    glPushMatrix();
      glColor4f(0.5, 0., 0., 1.0);
      glTranslatef(4.0, 10.0, 0.0);
      glRotatef(-90.0, 0., 1., 0.);
      gluCylinder(q, 5.0, 5.0, 14.0, 20, 5);
      glTranslatef(0.0, 0.0, 14.0);
      gluDisk(q, 0.0, 5.0, 20, 3);
      glColor4f(1.0, 1.0, 0.0, 1.0);
      glTranslatef(0.0, 4.0, 0.2);
      gluDisk(q, 0.0, 1.0, 20,2);  //headlight!
    glPopMatrix();

}

//--------------- WAGON ----------------------------------
// This extremely super complex model of a rail wagon consists of the base,
// and a textured quad
//--------------------------------------------------------
void wagon()
{
	base();

	glColor4f(1.0, 1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[0]);

	//3 large polygons (front, back, top)
	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0);   //Facing +z (Front side)
		
		//Bottom left
		glTexCoord2f(0., 0.);
		glVertex3f(-9.0, 5.0, 5.0);
		
		//Bottom right
		glTexCoord2f(1., 0.);
		glVertex3f(9.0, 5.0, 5.0);
		
		//Top right
		glTexCoord2f(1., 0.34);
		glVertex3f(9.0, 15.0, 5.0);
		
		//Top left
		glTexCoord2f(0., 0.34);
		glVertex3f(-9.0, 15.0, 5.0);

		glNormal3f(0.0, 0.0, -1.0);   //Facing -z (Back side)

		glTexCoord2f(0., 0.);
		glVertex3f(9.0, 5.0, -5.0);

		glTexCoord2f(1., 0.);
		glVertex3f(-9.0, 5.0,-5.0);

		glTexCoord2f(1., 0.34);
		glVertex3f(-9.0, 15.0,-5.0);

		glTexCoord2f(0., 0.34);
		glVertex3f(9.0, 15.0, -5.0);
		
		glNormal3f(0.0, 1.0, 0.0);   //Facing +y (Top side)

		glTexCoord2f(0., 0.45);
		glVertex3f(-9.0, 15.0, 5.0);

		glTexCoord2f(1., 0.45);
		glVertex3f(9.0, 15.0,  5.0);

		glTexCoord2f(1., 0.82);
		glVertex3f(9.0, 15.0, -5.0);

		glTexCoord2f(0., 0.82);
		glVertex3f(-9.0, 15.0, -5.0);
	glEnd();

	//2 small side polygons (left, right)
	glColor4f(0.5, 0.5, 0.0, 1.0);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	  glNormal3f(-1.0, 0.0, 0.0);   //Facing -x (Left side)
	  glVertex3f(-9.0, 5.0, -5.0);
	  glVertex3f(-9.0, 5.0, 5.0);
	  glVertex3f(-9.0, 15.0, 5.0);
	  glVertex3f(-9.0, 15.0, -5.0);

	  glNormal3f(1.0, 0.0, 0.0);   //Facing +x (Right side)
	  glVertex3f(9.0, 5.0, 5.0);
	  glVertex3f(9.0, 5.0, -5.0);
	  glVertex3f(9.0, 15.0, -5.0);
	  glVertex3f(9.0, 15.0, 5.0);
	glEnd();
}

void station(float medRadius)
{
	int width = 30;
	int height = 30;
	int bottom = 0;
	float trackRad = medRadius - (width * 2);
	float inRad = medRadius - width * 0.5;
	float outRad = medRadius + width * 0.5;
	float angle1, angle2, ca1, sa1, ca2, sa2;
	float x1, z1, x2, z2, x3, z3, x4, z4, x5, z5, x6, z6;  //Four points of a quad + 2 points points for the roof

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	glBegin(GL_QUADS);
	float radius = outRad;
	for (int j = 0; j < 3; j++)   //Two parallel walls (radius = outRad, inRad)
	{
		for (int i = 240; i < 300; i += 5)    //5 deg intervals
		{
			angle1 = i * toRad;       //Computation of angles, cos, sin etc
			angle2 = (i + 5) * toRad;
			ca1 = cos(angle1); ca2 = cos(angle2);
			sa1 = sin(angle1); sa2 = sin(angle2);
			x2 = (radius + 0.5) * sa1; z2 = (radius + 0.5) * ca1;
			x3 = (radius + 0.5) * sa2; z3 = (radius + 0.5) * ca2;

			glNormal3f(sa1, 0.0, ca1);   //Quad facing outward
			glTexCoord2f(0.0, 0.0);
			glVertex3f(x2, height, z2); //Bottom left
			glTexCoord2f(0.0, 1.0);
			glVertex3f(x2, 0.0, z2); //Top left
			glNormal3f(sa2, 0.0, ca2); 
			glTexCoord2f(1.0, 1.0);
			glVertex3f(x3, 0.0, z3); //Top right
			glTexCoord2f(1.0, 0.0);
			glVertex3f(x3, height, z3); //Bottom right
			if (i == 275 && j == 1) {
				glEnd();
				glBindTexture(GL_TEXTURE_2D, txId[5]);
				glColor4f(0.7, 0.7, 0.7, 1.0);
				glBegin(GL_QUADS);
					glNormal3f(sa1, 10, ca1);   //Quad facing outward
					glTexCoord2f(0.25, 0.0);
					glVertex3f(x2 + 1, height, z2); //Bottom left
					glTexCoord2f(0.25, 1.0);
					glVertex3f(x2 + 1, 10, z2); //Top left
					glNormal3f(sa2, 10, ca2);
					glTexCoord2f(0.75, 1.0);
					glVertex3f(x3 + 1, 10, z3); //Top right
					glTexCoord2f(0.75, 0.0);
					glVertex3f(x3 + 1, height, z3); //Bottom right
				glEnd();
				glColor4f(1.0, 1.0, 1.0, 1.0);
				glBindTexture(GL_TEXTURE_2D, txId[1]);
				glBegin(GL_QUADS);
			}
		}
		if(j == 0) {
			radius = inRad;
		}
		if(j == 1) {
			glEnd();
			glBindTexture(GL_TEXTURE_2D, txId[2]);
			glBegin(GL_QUADS);
			radius = trackRad;
			height = height / 3;
		}
	}
	height = 30;
	glEnd();
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	glBegin(GL_QUADS);
	for(int i = 0; i < 3 ; i++) {
		for (int i = 240; i < 300; i += 5) {   //Roof
			angle1 = i * toRad;
			angle2 = (i + 5) * toRad;
			ca1 = cos(angle1); ca2 = cos(angle2);
			sa1 = sin(angle1); sa2 = sin(angle2);
			x2 = (inRad + 0.5) * sa1; z2 = (inRad + 0.5) * ca1;
			x3 = (inRad + 0.5) * sa2; z3 = (inRad + 0.5) * ca2;

			x5 = (outRad + 0.5) * sa1; z5 = (outRad + 0.5) * ca1;
			x6 = (outRad + 0.5) * sa2; z6 = (outRad + 0.5) * ca2;

			glNormal3f(0., height, 0.); //Quad facing up
			glTexCoord2f(1.0, 0.0);
			glVertex3f(x2, height, z2);	//From a birds eye view, bottom inner
			glTexCoord2f(1.0, 1.0);
			glVertex3f(x3, height, z3);	//Top inner
			glTexCoord2f(0.0, 1.0);
			glVertex3f(x6, height, z6); //Top outer
			glTexCoord2f(0.0, 0.0);
			glVertex3f(x5, height, z5);	//Bottom outer

			if(i == 295) { //Close panel
				glNormal3f(sa1, bottom, ca1);   //Quad facing outward
				glTexCoord2f(1.0, 0.0);
				glVertex3f(x3, bottom, z3);	//Bottom inner
				glTexCoord2f(1.0, 1.0);
				glVertex3f(x3, height, z3);	//Top inner
				glTexCoord2f(0.0, 1.0);
				glVertex3f(x6, height, z6); //Top outer
				glTexCoord2f(0.0, 0.0);
				glVertex3f(x6, bottom, z6); //Bottom outer
			}
			if(i == 240) { //Far panel
				glNormal3f(sa1, bottom, ca1);   //Quad facing outward
				glTexCoord2f(1.0, 0.0);
				glVertex3f(x2, bottom, z2);	//Bottom inner
				glTexCoord2f(1.0, 1.0);
				glVertex3f(x2, height, z2);	//Top inner
				glTexCoord2f(0.0, 1.0);
				glVertex3f(x5, height, z5); //Top outer
				glTexCoord2f(0.0, 0.0);
				glVertex3f(x5, bottom, z5); //Bottom outer
			}
		}
		//Easily creates the platform using similar parameters
		if (i == 0) {
			height = height / 3;
			outRad = inRad;
			inRad = trackRad;
			glEnd();
			glBindTexture(GL_TEXTURE_2D, txId[2]);
			glBegin(GL_QUADS);
		}
		else {
			bottom = 25;
			height = 30;
			glEnd();
			glBindTexture(GL_TEXTURE_2D, txId[6]);
			glBegin(GL_QUADS);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void tunnel(float medRadius) {
	int width = 30;
	int length = 100;
	float x = 0;
	float y = 0;

	glColor4f(0.7, 0.7, 0.7, 0.5);
	glBegin(GL_QUAD_STRIP);
	for(float angle = 0; angle < (1.1 * 3.14159265); angle += 0.1) {
		x = width * cos(angle);
		y = width * sin(angle);
		glNormal3f(x, y, length);
		glVertex3f(x, y, length);
		glVertex3f(x, y, 0.0);
	}
	glEnd();
}

void asphalt(float medRadius, float width)
{
	float inRad = medRadius - width;
	float outRad = medRadius + width;
	float angle1, angle2, ca1, sa1, ca2, sa2;
	float x1, z1, x2, z2, x3, z3, x4, z4, x5, z5, x6, z6;  //four points of a quad

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[3]);
	glBegin(GL_QUADS);
	for (int i = 0; i < 360; i += 5) {   //Roof
		angle1 = i * toRad;
		angle2 = (i + 5) * toRad;
		ca1 = cos(angle1); ca2 = cos(angle2);
		sa1 = sin(angle1); sa2 = sin(angle2);
		x2 = (inRad + 0.5) * sa1; z2 = (inRad + 0.5) * ca1;
		x3 = (inRad + 0.5) * sa2; z3 = (inRad + 0.5) * ca2;

		x5 = (outRad + 0.5) * sa1; z5 = (outRad + 0.5) * ca1;
		x6 = (outRad + 0.5) * sa2; z6 = (outRad + 0.5) * ca2;

		glNormal3f(0., 0.1, 0.); //Quad facing up
		glTexCoord2f(1.0, 0.0);
		glVertex3f(x2, 0.1, z2);	//From a birds eye view, bottom inner
		glTexCoord2f(1.0, 1.0);
		glVertex3f(x3, 0.1, z3);	//Top inner
		glTexCoord2f(0.0, 1.0);
		glVertex3f(x6, 0.1, z6); //Top outer
		glTexCoord2f(0.0, 0.0);
		glVertex3f(x5, 0.1, z5);	//Bottom outer
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void trafficLight(bool trainMove) {
	float redLight;
	float greenLight;
	if(trainMove) {
		redLight = 0.3;
		greenLight = 1.0;
	} else {
		redLight = 1.0;
		greenLight = 0.3;
	}
	q = gluNewQuadric();
	glColor4f(0.4, 0.4, 0.5, 1.0);
	gluCylinder(q, 1, 1, 15, 10, 10);
	gluQuadricDrawStyle(q, GLU_FILL);
	glPushMatrix();
		glTranslatef(0, 0, 15);
		glutSolidCube(6);
		glTranslatef(0, 3, 1.5);
		glColor4f(redLight, 0.0, 0.0, 0.0);
		glutSolidSphere(1.3, 36, 10);
		glTranslatef(0, 0, -3);
		glColor4f(0.0, greenLight, 0.0, 0.0);
		glutSolidSphere(1.3, 36, 10);
	glPopMatrix();
}

void barrier(float barrierAngle) {
	glColor4f(0.5, 0.5, 0.5, 1.0);
	glutSolidCube(8);
	glPushMatrix();
		glPushMatrix();
			glTranslatef(0, 5, 0);
			glutSolidSphere(4.5, 10, 4);
		glPopMatrix();
		glColor4f(0.7, 0.3, 0.3, 1.0);
		glRotatef(barrierAngle, 0, 0, 1);
			glBegin(GL_QUADS);
				glVertex3f(-4, 2, 0); //Bottom right
				glVertex3f(-4, 5, 0); //Top right
				glVertex3f(-50, 6, 0); //Top left
				glVertex3f(-50, 2, 0); //Bottom left
			glEnd();
	glPopMatrix();
}

void road() {
	glColor4f(1, 1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[4]);

	glBegin(GL_QUAD_STRIP);                  //Draw a polygonal line using GL_LINE_STRIP
	for (int i = -50; i <= 50; i++)             //Was originally i < NPTS
	{
		glNormal3f(0., 0.1, 0.); //Quad facing up
		glTexCoord2f(i, 0);
		glVertex3f(-25, 0.01, i*10);    //Bottom left
		glTexCoord2f(i, 1);
		glVertex3f(25, 0.01, i*10);    //Top right
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}