#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

//const float PI = 3.1415926535898;

Ray reflection(vec3 direction, vec3 norm, vec3 pos);

Ray refraction(vec3 direction, vec3 norm, vec3 pos, float indOfRefr);

Ray perturbedRay(Ray mainRay, float variance);

vec3 rayCast(Ray ray, int depth, SceneNode* root, std::list<Light *> lights, vec3 backClr, bool depthOfField = false);

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		glm::vec3 & eye,
		glm::vec3 & view,
		glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
);
