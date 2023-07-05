/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray. 
*/
float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir)
{
    float a = pow(dir.x, 2) + pow(dir.z, 2);
    float b = 2 * (dir.x * (p0.x - baseCenter.x) + dir.z * (p0.z - baseCenter.z));
    float c = pow((p0.x - baseCenter.x), 2) + pow((p0.z - baseCenter.z), 2) - pow(radius, 2);
    float delta = b*b - (4 * a * c);
   
	if(delta < 0.001) return -1.0;    //includes zero and negative values

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);

    glm::vec3 p1 = glm::vec3((p0 + t1 * dir));
    glm::vec3 p2 = glm::vec3((p0 + t2 * dir));

    if(p1.y >= (baseCenter.y) && p1.y <= (baseCenter.y + height)) {
        return t1;
    } else {
        if(p2.y >= (baseCenter.y) && p2.y <= (baseCenter.y + height)) {
            return t2;
        } else {
            return -1;
        }
    }
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3(((p.x - baseCenter.x) / radius), 0, ((p.z - baseCenter.z) / radius));
    return n;
}
