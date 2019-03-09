#include <glm/ext.hpp>

#include "A4.hpp"

using namespace glm;
using namespace std;

Ray reflection(vec3 dir, vec3 norm, vec3 pos) {
	Ray reflRay;
	reflRay.dir = dir - 2 * norm * dot(dir,norm);
	reflRay.pos = pos + norm*0.01;
	return reflRay;
}

vec3 rayCast(Ray ray, int depth, SceneNode* root, std::list<Light *> lights, vec3 backClr, vec3 ambient) {
	Intersection myIntersect = root->intersect(ray);
	PhongMaterial* materia = myIntersect.materia;
    vec3 clr;

	if (myIntersect.did_hit) {
		clr[0] = materia->m_kd[0] * ambient.r;
		clr[1] = materia->m_kd[1] * ambient.g;
		clr[2] = materia->m_kd[2] * ambient.b;
		for (Light* light : lights) {
			vec3 liDir = light->position - myIntersect.pos;
			liDir = normalize(liDir);
			Ray shadowRay;
			shadowRay.dir = liDir;
			shadowRay.pos = myIntersect.pos;
			Intersection shadowIntersect = root->intersect(shadowRay);
			if (!(shadowIntersect.did_hit)) {
				float diffuseAmt = dot(liDir, myIntersect.norm);
				clr[0] += materia->m_kd[0] * diffuseAmt * light->colour.r;
				clr[1] += materia->m_kd[1] * diffuseAmt * light->colour.g;
				clr[2] += materia->m_kd[2] * diffuseAmt * light->colour.b;

				float specAmt = pow(dot(-ray.dir, 2*dot(myIntersect.norm, liDir)*myIntersect.norm - liDir), materia->m_shininess);
				clr[0] += materia->m_ks[0] * specAmt * light->colour.r;
				clr[1] += materia->m_ks[1] * specAmt * light->colour.g;
				clr[2] += materia->m_ks[2] * specAmt * light->colour.b;
			}
		}
		if (depth > 0) {
			Ray reflRay = reflection(ray.dir, myIntersect.norm, myIntersect.pos);
			clr += 1.0 * materia->m_ks * rayCast(reflRay, depth-1, root, lights, backClr, ambient);
		}
        return clr;
	} else {
		return backClr;
	}
}

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here...

  std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();
	vec3 view_x_axis = cross(view, up);
	view_x_axis = normalize(view_x_axis);
	vec3 norm_up = normalize(up);
	vec3 norm_view = normalize(view);
	double radius_fov = fovy * PI/360;

	root->applyTrans(mat4());

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			vec3 backClr;
			if (rand() % 1000 == 1) {
				backClr[0] = 0.4;
				backClr[1] = 0.4;
				backClr[2] = 0.7;
			} else {
				backClr[0] = 0;
				backClr[1] = 0;
				backClr[2] = (float)y/(h*4);
			}
			
			vec3 clr = {0,0,0};
			/* Supersampling
			for (int i=-1; i<2; i++) {
				for (int j=-1; j<2; j++) {
					*/
					Ray primeRay;
					primeRay.dir = norm_view + ((x) / double(w) * 2 - 1) * (double(w)/double(h)) * tan(radius_fov) * view_x_axis
						+ ((y) / double(h) * 2 - 1) * tan(radius_fov * -norm_up);
					primeRay.pos = eye;
					primeRay.dir = normalize(primeRay.dir);
					clr += rayCast(primeRay, 2, root, lights, backClr, ambient); //* (1.0f/9.0f);
					/*
				}
			}
			*/

			image(x,y,0) = clr[0];
			image(x,y,1) = clr[1];
			image(x,y,2) = clr[2];
		}
	}

}
