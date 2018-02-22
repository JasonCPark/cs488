#include "Primitive.hpp"
#include <iostream>

Primitive::~Primitive()
{
}

Intersection Primitive::intersect(Ray ray) {
	Intersection myIntersect = Intersection();
	return myIntersect;
}

Intersection meshIntersect(Ray ray, vec3 pos, float size, Mesh* mesh) {
	std::vector<glm::vec3> vertices = mesh->m_vertices;
	std::vector<Triangle> faces = mesh->m_faces;
	vec3 a = ray.pos;
	vec3 b_a = ray.dir;
	float lowestT = std::numeric_limits<float>::max();
	Intersection myIntersect = Intersection();
	for (Triangle tri : faces) {
		vec3 P0 = vertices[tri.v1]*size + pos;
		vec3 P1 = vertices[tri.v2]*size + pos;
		vec3 P2 = vertices[tri.v3]*size + pos;
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
			}
		}
	}
	return myIntersect;
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

MeshPrim::~MeshPrim()
{
}

Intersection MeshPrim::intersect(Ray ray)
{
	return meshIntersect(ray, vec3(0,0,0), 1, m_mesh);
}

NonhierSphere::~NonhierSphere()
{
}

Intersection NonhierSphere::intersect(Ray ray) {
	float radius = this->m_radius;
	vec3 pos = this->m_pos;

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

NonhierBox::~NonhierBox()
{
}

NonhierMeshPrim::~NonhierMeshPrim()
{
	delete m_mesh;
}

Intersection NonhierMeshPrim::intersect(Ray ray) {
	return meshIntersect(ray, m_pos, m_size, m_mesh);
}

