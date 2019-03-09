#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double ior, bool glossy)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_ior(ior)
	, glossy(glossy)
{}

PhongMaterial::~PhongMaterial()
{}
