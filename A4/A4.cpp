#include <glm/ext.hpp>

#include "A4.hpp"

using namespace glm;
using namespace std;

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


	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			if (rand() % 1000 == 1) {
				image(x, y, 0) = 0.4;
				image(x, y, 1) = 0.4;
				image(x, y, 2) = 0.7;
			} else {
				image(x, y, 0) = 0;
				image(x, y, 1) = 0;
				image(x, y, 2) = (float)y/(h*4);
			}
			
			Ray primeRay;
			primeRay.dir = norm_view + (x / double(w) * 2 - 1) * (double(w)/double(h)) * tan(radius_fov) * view_x_axis
				+ (y / double(h) * 2 - 1) * tan(radius_fov * -norm_up);
			primeRay.pos = eye;
			primeRay.dir = normalize(primeRay.dir);
			Intersection myIntersect = root->intersect(primeRay);
			if (myIntersect.did_hit) {

				
				PhongMaterial* materia = myIntersect.materia;
				vec3 clr;

				for (Light* light : lights) {
					vec3 liDir = light->position - myIntersect.pos;
					liDir = normalize(liDir);

					float specAmt = pow(dot(-primeRay.dir, 2*dot(myIntersect.norm, liDir)*myIntersect.norm - liDir), materia->m_shininess);
					clr[0] += materia->m_ks[0] * specAmt * light->colour.r;
					clr[1] += materia->m_ks[1] * specAmt * light->colour.g;
					clr[2] += materia->m_ks[2] * specAmt * light->colour.b;

					float diffuseAmt = dot(liDir, myIntersect.norm);
					clr[0] += materia->m_kd[0] * diffuseAmt * light->colour.r;
					clr[1] += materia->m_kd[1] * diffuseAmt * light->colour.g;
					clr[2] += materia->m_kd[2] * diffuseAmt * light->colour.b;


				}
				image(x,y,0) = clr[0];
				image(x,y,1) = clr[1];
				image(x,y,2) = clr[2];
			}

			/*
			// Red: increasing from top to bottom
			image(x, y, 0) = (double)y / h;
			// Green: increasing from left to right
			image(x, y, 1) = (double)x / w;
			// Blue: in lower-left and upper-right corners
			image(x, y, 2) = ((y < h/2 && x < w/2)
						  || (y >= h/2 && x >= w/2)) ? 1.0 : 0.0;
			*/
			
		}
	}

}
