#pragma once

#include <glm/glm.hpp>
#include "polyroots.hpp"
#include "raytrace.hpp"
#include "Mesh.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual Intersection intersect(Ray ray);
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  virtual ~Cube();
};

class MeshPrim : public Primitive {
  public:
  MeshPrim(Mesh* mesh, mat4 trans)
    : m_mesh(mesh)
    {
    }
	virtual ~MeshPrim();
  Intersection intersect(Ray ray);

  private:
    Mesh *m_mesh;
    mat4 trans;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
	Intersection intersect(Ray ray);
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();
private:
  glm::vec3 m_pos;
  double m_size;
	Mesh *m_mesh;
};

class NonhierMeshPrim : public Primitive {
  public:
  NonhierMeshPrim(const glm::vec3& pos, float size, Mesh* mesh)
    : m_pos(pos), m_size(size), m_mesh(mesh)
    {
    }
	virtual ~NonhierMeshPrim();
	Intersection intersect(Ray ray);

  private:
    glm::vec3 m_pos;
    float m_size;
    Mesh *m_mesh;
};
