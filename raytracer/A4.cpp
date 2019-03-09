#include <glm/ext.hpp>
#include <omp.h>
#include <time.h>

#include "cs488-framework/MathUtils.hpp"
#include "A4.hpp"

using namespace glm;
using namespace std;

bool reflection_flag = true;
bool refraction_flag = true;
bool soft_shadow_flag = true;
bool regular_sampling_flag = false;
bool depth_of_field_flag = false;
bool motion_blur_flag = false;

Ray reflection(vec3 dir, vec3 norm, vec3 pos) {
	Ray reflRay;
	reflRay.dir = dir - 2 * norm * dot(dir,norm);
	reflRay.pos = pos + norm*0.01;
	return reflRay;
}

Ray refraction(vec3 dir, vec3 norm, vec3 pos, float indOfRefr) {
	vec3 n = norm;
	float ior1 = 1;
	float ior2 = indOfRefr;
	float c1 = clamp(dot(dir, norm),-1.0f, 1.0f);
	if (c1 < 0) {
		c1 = -c1;
	} else {
		swap(ior1, ior2);
		n = -norm;
	}
	float ior = ior1/ior2;
	float c2UnderRoot = 1 - ior * ior * (1-c1*c1);
	Ray refrRay = Ray();
	refrRay.pos = pos - n*0.01;
	if (c2UnderRoot < 0) {
		refrRay.dir = vec3(0,0,0);
	} else {
		refrRay.dir = ior * dir + (ior * c1 - sqrt(c2UnderRoot)) * n;
	}
	return refrRay;
}

Ray perturbedRay(Ray mainRay, float variance) {
	vec3 randVec = vec3(0,0,0);
	for (int i=0; i<3; i++) {
		randVec[i] = -1.0 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX/2);
	}
	vec3 mainDir = mainRay.dir;
	vec3 perturbation = cross(randVec, mainDir);
	perturbation = normalize(perturbation) * variance;
	return Ray(mainRay.pos, mainDir + perturbation);
}

vec3 phongIlluminate(Intersection myIntersect, Intersection shadowIntersect, vec3 liDir, PhongMaterial* materia, Ray ray, Light* light) {
	vec3 shadowClr = vec3(0,0,0);
	float diffuseAmt = dot(liDir, myIntersect.norm);
	shadowClr += myIntersect.clr * diffuseAmt * light->colour;

	float specAmt = pow(dot(-ray.dir, 2*dot(myIntersect.norm, liDir)*myIntersect.norm - liDir), materia->m_shininess);
	if (specAmt < 0) {
		specAmt = 0;
	}
	shadowClr += materia->m_ks * specAmt * light->colour;

	if (shadowIntersect.did_hit && shadowIntersect.materia->m_ior > 0) {
		shadowClr *= 0.5;
	}
	return shadowClr;
}

vec3 rayCast(Ray ray, int depth, SceneNode* root, std::list<Light *> lights, vec3 backClr, bool depthOfField) {
	vec3 clr;
	Intersection testIntersect;
	testIntersect = root->intersect(ray);
	int motionTrials = 1;
	if (motion_blur_flag && testIntersect.isMotion) {
		motionTrials = 10;
	}
	for (int motioni=0; motioni<motionTrials; motioni++) {
		vec3 motionClr = vec3(0,0,0);
		Intersection myIntersect;
		myIntersect = root->intersect(ray);
		
		PhongMaterial* materia = myIntersect.materia;

		// Object Intersection
		if (myIntersect.did_hit) {
			if (myIntersect.clr[0] <= 0) {
				myIntersect.clr = materia->m_kd;
			}

			// Reflection and Refraction
			if (depth > 0) {
				if (refraction_flag && materia->m_ior > 0) {
					Ray refrRay = refraction(ray.dir, myIntersect.norm, myIntersect.pos, materia->m_ior);
					vec3 refrClr = vec3(0,0,0);
					if (!materia->glossy) {
						refrClr = materia->m_ks * rayCast(refrRay, depth-1, root, lights, backClr);
					} else {
						// Mulithreading
						#pragma omp parallel for
						for (int i=0; i<8; i++) {
							Ray perturbed = perturbedRay(refrRay, 0.01);
							refrClr += materia->m_ks * rayCast(perturbed, depth-1, root, lights, backClr) / 8.0;
						}
					}
					motionClr += refrClr * 0.8;
				}
				if (reflection_flag && materia->m_ks != vec3(0,0,0)) {
					Ray reflRay = reflection(ray.dir, myIntersect.norm, myIntersect.pos);
					vec3 reflClr = vec3(0,0,0);
					if (!materia->glossy) {
						reflClr = materia->m_ks * rayCast(reflRay, depth-1, root, lights, backClr);
					} else {
						// Mulithreading
						#pragma omp parallel for
						for (int i=0; i<8; i++) {
							Ray perturbed = perturbedRay(reflRay, 0.2);
							reflClr += materia->m_ks * rayCast(perturbed, depth-1, root, lights, backClr) / 8.0;
						}
					}
					if (materia->m_ior > 0) {
						motionClr += reflClr * 0.1;
					} else {
						motionClr += reflClr * 0.4;
					}
				}
			}

			// Phong Illumination
			if (materia->m_ior <= 0) {
				for (Light* light : lights) {
					vec3 liDir = light->position - myIntersect.pos;
					liDir = normalize(liDir);
					Ray shadowRay;
					shadowRay.dir = liDir;
					shadowRay.pos = myIntersect.pos;
					Intersection initShadowIntersect;
					if (soft_shadow_flag) {
						Ray initPerturbed = perturbedRay(shadowRay, 0.1);
						initShadowIntersect = root->intersect(initPerturbed);
					} else {
						initShadowIntersect = root->intersect(shadowRay);
					}
					if (initShadowIntersect.did_hit) {
						if (soft_shadow_flag) {
							// Mulithreading
							#pragma omp parallel for
							// Loop for Soft Shadow
							for (int i=0; i<10; i++) {
								Ray perturbed = perturbedRay(shadowRay, 0.2);
								Intersection shadowIntersect = root->intersect(perturbed);
								// Direct path to light source
								if (!(shadowIntersect.did_hit) || shadowIntersect.materia->m_ior > 0) {
									vec3 shadowClr = phongIlluminate(myIntersect, shadowIntersect, liDir, materia, ray, light);
									motionClr += shadowClr/10.0;
								}
							}
						}
					} else {
						motionClr += phongIlluminate(myIntersect, initShadowIntersect, liDir, materia, ray, light);
					}
				}
			}
		} else {
			motionClr = backClr;
		}
		clr += motionClr / (float)motionTrials;
	}
	return clr;
}

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
) {
	double startTime = omp_get_wtime();

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

	mat4 translMat1 = translate(vec3(0,0,2));
	mat4 translMat2 = translate(vec3(0,0,-2));

	int dofi = 1;
	if (depth_of_field_flag) {
		dofi = 10;
	}
	for (int i=0; i<dofi; i++) {
		vec3 _eye;
		vec3 _view;
		vec3 _up;
		if (depth_of_field_flag) {
			float randAngX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/1.0f);
			float randAngY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/1.0f);
			mat4 rotMat = rotate(degreesToRadians(-0.5f + randAngX), vec3(1,0,0))
				* rotate(degreesToRadians(-0.5f + randAngY), vec3(0,1,0));
			_eye = vec3(translMat2 * rotMat * translMat1 * vec4(eye,1));
			_view = vec3(rotMat * vec4(view,1));
			_up = vec3(rotMat * vec4(up,1));
		} else {
			_eye = eye;
			_view = view;
			_up = up;
		}

		size_t h = image.height();
		size_t w = image.width();
		vec3 view_x_axis = cross(view, _up);
		view_x_axis = normalize(view_x_axis);
		vec3 norm_up = normalize(_up);
		vec3 norm_view = normalize(_view);
		double radius_fov = fovy * PI/360;

		for (uint y = 0; y < h; ++y) {
			// Mulithreading
			#pragma omp parallel for
			for (int x = 0; x < w; ++x) {
				// Set background colour
				vec3 backClr;
				/*
				if (rand() % 1000 == 1) {
					backClr[0] = 0.4;
					backClr[1] = 0.4;
					backClr[2] = 0.7;
				} else {
					backClr[0] = 0;
					backClr[1] = 0;
					backClr[2] = (float)y/(h*4);
				}
				*/
					backClr[0] = (float)y/(h*4);
					backClr[1] = (float)y/(h*4);
					backClr[2] = (float)y/(h*4);
				
				vec3 clr = {0,0,0};
				// Supersampling
				float sample_size = 1.0f;
				float divider = 1.0f;
				if (regular_sampling_flag) {
					sample_size = 0.2f;
					divider = 25;
				}
				for (float i=0; i<1; i+=sample_size) {
					for (float j=0; j<1; j+=sample_size) {
						// Initial Rays
						Ray primeRay;
						primeRay.dir = norm_view + ((x+i) / double(w) * 2 - 1) * (double(w)/double(h)) * tan(radius_fov) * view_x_axis
							+ ((y+j) / double(h) * 2 - 1) * tan(radius_fov * -norm_up);
						primeRay.pos = _eye;
						primeRay.dir = normalize(primeRay.dir);
						clr += rayCast(primeRay, 2, root, lights, backClr) / divider;
					}
				}

				image(x,y,0) += clr[0]/ (float)dofi;
				image(x,y,1) += clr[1]/ (float)dofi;
				image(x,y,2) += clr[2]/ (float)dofi;
			}
			if (y % 10 == 0) {
				cout << i << " - " << ((float)y/(float)h)*100 << "% complete" << endl;
			}
		}
	}
	double elapsedTime = omp_get_wtime() - startTime;
	cout << "Running Time: " << elapsedTime << " seconds" << endl;
}
