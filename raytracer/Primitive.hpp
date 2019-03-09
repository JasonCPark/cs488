#pragma once

#include <cmath>
#include <list>
#include <iostream>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
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
  Intersection intersect(Ray ray);
};

class MeshPrim : public Primitive {
  public:
  MeshPrim(Mesh* mesh)
    : m_mesh(mesh)
    {
      vec3 vertTotal = vec3(0,0,0);
      for (vec3 vert : m_mesh->m_vertices) {
        vertTotal += vert;
      }
      boundOrig = vertTotal/m_mesh->m_vertices.size();
      float farthestDist = 0;
      for (vec3 vert : m_mesh->m_vertices) {
        float dist = glm::distance(boundOrig, vert);
        if (dist > farthestDist) {
          farthestDist = dist;
        }
      }
      boundRad = farthestDist;
    }
	virtual ~MeshPrim();
  Intersection intersect(Ray ray);

  private:
    Mesh *m_mesh;
    vec3 boundOrig;
    float boundRad;
};
