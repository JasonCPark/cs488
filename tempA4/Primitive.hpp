#pragma once

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
  virtual void applyTrans(mat4 trans);
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  void applyTrans(mat4 trans);
  Intersection intersect(Ray ray);
  mat4 invTrans = mat4();
  mat4 myTrans = mat4();
};

class Cube : public Primitive {
public:
  Cube(Mesh* mesh)
  : m_mesh(mesh)
  {
  }
  
  virtual ~Cube();
  void applyTrans(mat4 trans);
  Intersection intersect(Ray ray);
private:
	Mesh *m_mesh;
};

class MeshPrim : public Primitive {
  public:
  MeshPrim(Mesh* mesh)
    : m_mesh(mesh)
    {
    }
	virtual ~MeshPrim();
  Intersection intersect(Ray ray);
  void applyTrans(mat4 trans);

  private:
    Mesh *m_mesh;
};

class NonhierSphere : public Primitive {
public:
  virtual ~NonhierSphere();
	Intersection intersect(Ray ray);
  void applyTrans(mat4 trans);
  mat4 invTrans = mat4();
  mat4 myTrans = mat4();
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
  {
    /*
    glm::vec4 posVec4 = glm::vec4(pos,1);
    glm::mat4 posMat = mat4();
    posMat[3] = posVec4;
    */
    mat4 posMat = translate(pos);
    applyTrans(posMat);
    vec3 sizeVec = vec3(size, size, size);
    mat4 sizeMat = scale(sizeVec);
    applyTrans(sizeMat);
  }
  
  virtual ~NonhierBox();
private:
	Mesh *m_mesh;
};

class NonhierMeshPrim : public Primitive {
  public:
  NonhierMeshPrim(const glm::vec3& pos, float size, Mesh* mesh)
    : m_mesh(mesh)
    {
      vec3 sizeVec = vec3(size, size, size);
      mat4 sizeMat = scale(sizeVec);
      applyTrans(sizeMat);
      glm::vec4 posVec4 = glm::vec4(pos,1);
      glm::mat4 posMat = mat4();
      posMat[3] = posVec4;
      applyTrans(posMat);
    }
	virtual ~NonhierMeshPrim();
	Intersection intersect(Ray ray);
  void applyTrans(mat4 trans);

  private:
    Mesh *m_mesh;
};
