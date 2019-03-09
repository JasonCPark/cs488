#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double ior = -1, bool glossy = false);
  virtual ~PhongMaterial();

  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_ior;
  bool glossy = false;
};
