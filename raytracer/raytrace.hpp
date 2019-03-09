#include <glm/glm.hpp>
#include "PhongMaterial.hpp"
using namespace glm;

class Ray {
	public:
		Ray() {}
		Ray(vec3 p, vec3 d) : pos(p), dir(d) {}

		glm::vec3 pos = vec3();
		glm::vec3 dir = vec3();
};

class Intersection {
	public:
		vec3 pos = vec3();
		bool did_hit = false;
		float t = 0;
		vec3 norm = vec3();
		PhongMaterial* materia;
		vec3 clr = vec3(-1,-1,-1);
		bool isMotion = false;
};