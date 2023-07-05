/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_CYLINDER
#define H_CYLINDER
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple cylinder located at 'baseCenter'
 * with the specified radius and height
 */
class Cylinder : public SceneObject
{

private:
    glm::vec3 baseCenter = glm::vec3(0);
    float radius = 1;
    float height = 1;

public:
	Cylinder() {};  //Default constructor creates a unit sphere

	Cylinder(glm::vec3 c, float r, float h) : baseCenter(c), radius(r), height(h) {}

	float intersect(glm::vec3 p0, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 p);

};

#endif //!H_CYLINDER
