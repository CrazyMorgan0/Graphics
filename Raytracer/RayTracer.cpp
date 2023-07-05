/*==================================================================================
* COSC 363  Computer Graphics (2022)
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
* See Lab06.pdf   for details.
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Cylinder.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/freeglut.h>
#include "TextureBMP.h"
#include "Plane.h"

using namespace std;

const float EDIST = 40.0;
const int NUMDIV = 600;
const int MAX_STEPS = 5;
const float XMIN = -10.0;
const float XMAX = 10.0;
const float YMIN = -10.0;
const float YMAX = 10.0;

int length = 5;
int height = 4;
int y_offset = -16;
float x_offset = 12.5;
int z_offset = 70;

glm::vec3 sunCenter = glm::vec3(-5, 5, -EDIST);
glm::vec3 earthCenter = glm::vec3(5, 5, -EDIST - 10);

TextureBMP texture;
TextureBMP sunTexture;
TextureBMP earthTexture;

vector<SceneObject*> sceneObjects;


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0, 0.878, 0.960);						//Background colour = Orange
	glm::vec3 lightPos = glm::vec3(sunCenter.x, sunCenter.y, sunCenter.z + 10);				//Light's position
	glm::vec3 color(0);
	SceneObject* obj;

    ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found

	if (ray.index == 0) {
		//Stripe pattern
		int stripeWidth = 5;
			int iz = (ray.hit.z) / stripeWidth;
			int ix = (ray.hit.x + 50) / stripeWidth;
			int k = (iz + ix) % 2; //2 colours
			if (k == 0) color = glm::vec3(0, 0, 1);
			else color = glm::vec3(1, 0, 1);
			obj->setColor(color);
	}

	if(ray.index == 1) {
		float x1 = -20; float x2 = 20;
		float y1 = -20; float y2 = 20;
		float texcoords = (ray.hit.x - x1) / (x2 - x1);
		float texcoordt = (ray.hit.y - y1) / (y2 - y1);
		if (texcoords > 0 && texcoords < 1 && texcoordt > 0 && texcoordt < 1) {
			glm::vec3 color = texture.getColorAt(texcoords, texcoordt);
			obj->setColor(color);
		}
	}

	if(ray.index == 2) {
		glm::vec3 direction = sunCenter - ray.hit;
		glm::vec3 unitDirection = glm::normalize(direction);
		float texcoords = 0.5f + (atan2(-unitDirection.x, -unitDirection.z) / (2 * 3.14159265f));
		float texcoordt = 0.5f + (asin(-unitDirection.y) / 3.14159265f);
		if(texcoords > 0 && texcoords < 1 && texcoordt > 0 && texcoordt < 1) {
			color = sunTexture.getColorAt(texcoords, texcoordt);
			obj->setColor(color);
		}
	}

	if (ray.index == 3) {
		glm::vec3 direction = earthCenter - ray.hit;
		glm::vec3 unitDirection = glm::normalize(direction);
		float texcoords = 0.5f + (atan2(-unitDirection.x, -unitDirection.z) / (2 * 3.14159265f));
		float texcoordt = 0.5f + (asin(-unitDirection.y) / 3.14159265f);
		if (texcoords > 0 && texcoords < 1 && texcoordt > 0 && texcoordt < 1) {
			color = earthTexture.getColorAt(texcoords, texcoordt);
			obj->setColor(color);
		}
	}

	color = obj->lighting(lightPos, -ray.dir, ray.hit); //Object's colour calculated using Phong-Blinn

	glm::vec3 lightVec = lightPos - ray.hit;
	Ray shadowRay(ray.hit, lightVec);

	shadowRay.closestPt(sceneObjects);
	if (shadowRay.index > -1) { //Hit an object
		SceneObject* hitObj = sceneObjects[shadowRay.index]; //See what object the shadow ray hit
		float lightDist = glm::length(lightPos);
		if (shadowRay.dist < lightDist) {
			if (hitObj->isTransparent() || hitObj->isRefractive()) {
				color = 0.4f * obj->getColor(); //Cast a lighter shadow
			}
			else {
				color = 0.2f * obj->getColor(); //0.2 ambient lighting
			}
		}
	}

	if (obj->isReflective() && step < MAX_STEPS) {
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
		Ray reflectedRay(ray.hit, reflectedDir);
		glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
		color = color + (rho * reflectedColor);
	}

	if (obj->isRefractive() && step < MAX_STEPS) {
		float eta = 1.0f / obj->getRefractiveIndex();
		float rho = obj->getRefractionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 refractedDir = glm::refract(ray.dir, normalVec, eta);
		Ray refractedRay(ray.hit, refractedDir);
		refractedRay.closestPt(sceneObjects);
		glm::vec3 mNormalVec = obj->normal(refractedRay.hit);
		glm::vec3 h = glm::refract(refractedDir, -mNormalVec, 1.0f / eta);
		Ray hRefractedRay(refractedRay.hit, h);
		glm::vec3 refractedColor = trace(hRefractedRay, step + 1);
		color = color + (rho * refractedColor);
	}

	if (obj->isTransparent() && step < MAX_STEPS) {
		float rho = obj->getTransparencyCoeff();
		Ray imposedRay(ray.hit, ray.dir);
		glm::vec3 imposedColor = trace(imposedRay, step + 1);
		color = color + (rho * imposedColor);
	}
	return color;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for (int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
		xp = XMIN + i * cellX;
		for (int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j * cellY;

			glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);	//direction of the primary ray

			Ray ray = Ray(eye, dir);

			glm::vec3 col = trace(ray, 1); //Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
		}
	}

    glEnd();
    glFlush();
}



//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

	glClearColor(0, 0, 0, 1);

	sunTexture = TextureBMP("Sun.bmp");
	earthTexture = TextureBMP("Earth.bmp");
	texture = TextureBMP("nice_tree.bmp");
	

	Plane* plane = new Plane(
		glm::vec3(-20., -20, -40), //Point A
		glm::vec3(20., -20, -40), //Point B
		glm::vec3(20., -20, -200), //Point C
		glm::vec3(-20., -20, -200)); //Point D
	plane->setColor(glm::vec3(0.8, 0.8, 0));
	plane->setSpecularity(false);
	sceneObjects.push_back(plane);

	Plane* backWall = new Plane(
		glm::vec3(-20, -20, -200), //Point A
		glm::vec3(20, -20, -200), //Point B
		glm::vec3(20, 10, -200), //Point C
		glm::vec3(-20, 10, -200)); //Point D
	backWall->setSpecularity(false);
	backWall->setColor(glm::vec3(0));
	sceneObjects.push_back(backWall);

	Sphere *sun = new Sphere(sunCenter, 2.0);
	sun->setColor(glm::vec3(0));
	sun->setSpecularity(false);
	sceneObjects.push_back(sun);

	Sphere* earth = new Sphere(earthCenter, 1.3);
	earth->setColor(glm::vec3(0));
	earth->setSpecularity(false);
	sceneObjects.push_back(earth);

	glm::vec3 boxColor = glm::vec3(0);

	glm::vec3 topA = glm::vec3(0, height + y_offset, -EDIST - z_offset); //Point A
	glm::vec3 topB = glm::vec3(length, height + y_offset, -EDIST - length - z_offset); //Point B
	glm::vec3 topC = glm::vec3(0, height + y_offset, -EDIST - (2 * length) - z_offset); //Point C
	glm::vec3 topD = glm::vec3(-length, height + y_offset, -EDIST - length - z_offset); //Point D
	Plane* boxTop = new Plane(topA, topB, topC, topD);
	boxTop->setColor(boxColor);
	boxTop->setReflectivity(true, 1);
	sceneObjects.push_back(boxTop);

	glm::vec3 bottomA = glm::vec3(0, -height + y_offset, -EDIST - z_offset); //Point A

	glm::vec3 rightA = bottomA;
	glm::vec3 rightB = glm::vec3(length, -height + y_offset, -EDIST - length - z_offset); //Bottom B
	glm::vec3 rightC = topB;
	glm::vec3 rightD = topA;
	Plane* boxRight = new Plane(rightA, rightB, rightC, rightD);
	boxRight->setColor(boxColor);
	boxRight->setReflectivity(true, 1);
	sceneObjects.push_back(boxRight);

	glm::vec3 leftA = glm::vec3(-length, -height + y_offset, -EDIST - length - z_offset); //Bottom D
	glm::vec3 leftB = bottomA;
	glm::vec3 leftC = topA;
	glm::vec3 leftD = topD;
	Plane* boxLeft = new Plane(leftA, leftB, leftC, leftD);
	boxLeft->setColor(boxColor);
	boxLeft->setReflectivity(true, 1);
	sceneObjects.push_back(boxLeft);

	Sphere* refractiveSphere = new Sphere(glm::vec3(0, y_offset, -EDIST - 38), 3);
	refractiveSphere->setColor(glm::vec3(0));
	refractiveSphere->setRefractivity(true, 1, 1.5);
	sceneObjects.push_back(refractiveSphere);

	Sphere* transparentSphere = new Sphere(glm::vec3(-x_offset, y_offset, -EDIST - z_offset - 2), 4);
	transparentSphere->setColor(glm::vec3(1, 0, 0));
	transparentSphere->setTransparency(true, 1);
	sceneObjects.push_back(transparentSphere);

	Cylinder *cylinder = new Cylinder(glm::vec3(x_offset, y_offset - 6, -EDIST - z_offset -3), 3, 8);
	cylinder->setColor(glm::vec3(1, 0.26, 0.2));
	sceneObjects.push_back(cylinder);
}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Toby Morgan - Cosc363 Assignment 2");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
