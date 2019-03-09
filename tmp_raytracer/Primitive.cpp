#include "Primitive.hpp"
#include <iostream>
#include <list>
using namespace std;

Intersection sphereIntersect(Ray ray, vec3 pos, float radius) {
	Intersection myIntersect = Intersection();
	vec3 L = ray.pos - pos;
	float a = dot(ray.dir,ray.dir);
	float b = 2*dot(ray.dir,L);
	float c = dot(L,L)-pow(radius,2);
	double roots[2];
	size_t numRoots = quadraticRoots(a,b,c,roots);
	float t = (float)glm::min(roots[0], roots[1]);
	if (numRoots > 0) {
		myIntersect.t = t;
		myIntersect.did_hit = true;
		myIntersect.pos = ray.pos + ray.dir * t;
		myIntersect.norm = myIntersect.pos - pos;
		myIntersect.norm = normalize(myIntersect.norm);
	}
	return myIntersect;
}

Primitive::~Primitive()
{
}

Intersection Primitive::intersect(Ray ray) {
	Intersection myIntersect = Intersection();
	return myIntersect;
}

Sphere::~Sphere()
{
}

Intersection Sphere::intersect(Ray ray) {
	return sphereIntersect(ray, vec3(0,0,0), 1);
}

MeshPrim::~MeshPrim()
{
	delete m_mesh;
}

Intersection MeshPrim::intersect(Ray ray)
{
	Intersection myIntersect = Intersection();
	if (sphereIntersect(ray, boundOrig, boundRad).did_hit) {
		std::vector<glm::vec3> vertices = m_mesh->m_vertices;
		std::vector<Triangle> faces = m_mesh->m_faces;
		vec3 a = ray.pos;
		vec3 b_a = ray.dir;
		float lowestT = std::numeric_limits<float>::max();
		//debug
		if (faces[0].hasTexture) {
			faces[0].myTexture.read(0,0);
		}
		for (Triangle tri : faces) {
			vec3 P0 = vertices[tri.v1];
			vec3 P1 = vertices[tri.v2];
			vec3 P2 = vertices[tri.v3];
			mat3 M = {P1-P0, P2-P0, -b_a};
			mat3 M1 = {a-P0, P2-P0, -b_a};
			mat3 M2 = {P1-P0, a-P0, -b_a};
			mat3 M3 = {P1-P0, P2-P0, a-P0};
			float D = determinant(M);
			float D1 = determinant(M1);
			float D2 = determinant(M2);
			float D3 = determinant(M3);

			float beta = D1/D;
			float gamma = D2/D;
			float t = D3/D;

			if (beta >= 0 && gamma >= 0 && beta + gamma <= 1) {
				if (t < lowestT) {
					lowestT = t;
					myIntersect.t = t;
					myIntersect.did_hit = true;
					myIntersect.pos = ray.pos + ray.dir * t;
					myIntersect.norm = cross(P1-P0,P2-P1);
					myIntersect.norm = normalize(myIntersect.norm);
					if (tri.hasTexture) {
						myIntersect.clr = tri.myTexture.read(beta, gamma);
					}
				}
			}
		}
	}
	return myIntersect;
}
