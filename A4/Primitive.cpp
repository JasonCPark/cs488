#include "Primitive.hpp"
#include <iostream>

Primitive::~Primitive()
{
}

void Primitive::applyTrans(glm::mat4 trans){
}

Intersection Primitive::intersect(Ray ray) {
	Intersection myIntersect = Intersection();
	return myIntersect;
}

Intersection meshIntersect(Ray ray, Mesh* mesh) {
	std::vector<glm::vec3> vertices = mesh->m_vertices;
	std::vector<Triangle> faces = mesh->m_faces;
	vec3 a = ray.pos;
	vec3 b_a = ray.dir;
	float lowestT = std::numeric_limits<float>::max();
	Intersection myIntersect = Intersection();
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
			}
		}
	}
	return myIntersect;
}

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

Sphere::~Sphere()
{
}

void Sphere::applyTrans(mat4 trans) {
	/*
	vec3 translVec = vec3(trans[3]);
	m_pos += translVec;
	*/
	invTrans = inverse(trans);
	myTrans = trans;
}

Intersection Sphere::intersect(Ray ray) {
	ray.pos = vec3(invTrans * vec4(ray.pos,1));
	ray.dir = mat3(invTrans) * ray.dir;
	Intersection myIntersect = sphereIntersect(ray, vec3(0,0,0), 1);
	myIntersect.pos = vec3(myTrans * vec4(myIntersect.pos, 1));
	myIntersect.norm = transpose(mat3(invTrans)) * myIntersect.norm;
	myIntersect.norm = normalize(myIntersect.norm);
	return myIntersect;
}

Cube::~Cube()
{
	delete m_mesh;
}

void Cube::applyTrans(mat4 trans) {
	m_mesh->applyTrans(trans);
}

Intersection Cube::intersect(Ray ray) {
	return meshIntersect(ray, m_mesh);
}

MeshPrim::~MeshPrim()
{
	delete m_mesh;
}

void MeshPrim::applyTrans(mat4 trans) {
	m_mesh->applyTrans(trans);
}

Intersection MeshPrim::intersect(Ray ray)
{
	return meshIntersect(ray, m_mesh);
}

NonhierSphere::~NonhierSphere()
{
}

Intersection NonhierSphere::intersect(Ray ray) {
	ray.pos = vec3(invTrans * vec4(ray.pos,1));
	ray.dir = mat3(invTrans) * ray.dir;
	Intersection myIntersect = sphereIntersect(ray, vec3(0,0,0), 1);
	myIntersect.pos = vec3(myTrans * vec4(myIntersect.pos, 1));
	myIntersect.norm = transpose(mat3(invTrans)) * myIntersect.norm;
	myIntersect.norm = normalize(myIntersect.norm);
	return myIntersect;
}

void NonhierSphere::applyTrans(mat4 trans) {
	invTrans = inverse(trans);
	myTrans = trans;
}

NonhierBox::~NonhierBox()
{
	delete m_mesh;
}

NonhierMeshPrim::~NonhierMeshPrim()
{
	delete m_mesh;
}

void NonhierMeshPrim::applyTrans(mat4 trans) {
	m_mesh->applyTrans(trans);
}

Intersection NonhierMeshPrim::intersect(Ray ray) {
	return meshIntersect(ray, m_mesh);
}

