#pragma once

#include "SceneNode.hpp"
#include "PhongMaterial.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr, vec3 motionTransl = vec3(0,0,0) );

	void setMaterial( Material *material );
	Intersection intersect(Ray ray);
	
	~GeometryNode();

	Material *m_material;
	Primitive *m_primitive;
	vec3 motionTransl = vec3(0,0,0);
};


