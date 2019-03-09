#pragma once

#include "SceneNode.hpp"
#include "PhongMaterial.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	GeometryNode( const std::string & name, Mesh* mesh,
		Material *mat = nullptr );
	void setMaterial( Material *material );
	Intersection intersect(Ray ray);
	void applyTrans(glm::mat4 parentTrans);
	
	~GeometryNode();

	Material *m_material;
	Primitive *m_primitive;
};


