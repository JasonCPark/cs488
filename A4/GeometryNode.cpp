#include "GeometryNode.hpp"
#include <glm/glm.hpp>
using namespace glm;
using namespace std;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	cout << "GEO CREATED: " << name << endl;
	m_nodeType = NodeType::GeometryNode;
}

GeometryNode::~GeometryNode()
{
	delete m_material;
	delete m_primitive;
}

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Mesh* mesh, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( new MeshPrim(mesh))
{
	cout << "GEO CREATED: " << name << endl;
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

Intersection GeometryNode::intersect(Ray ray) {
	
	Intersection myIntersect = this->m_primitive->intersect(ray);
	Intersection frontIntersect;
	float lowestT;
	if (myIntersect.did_hit && myIntersect.t > 0.01) {
		myIntersect.materia = dynamic_cast<PhongMaterial*>(this->m_material);
		frontIntersect = myIntersect;
		lowestT = myIntersect.t;
	} else {
		frontIntersect = Intersection();
		lowestT = numeric_limits<float>::max();
	}

	for (SceneNode * node : this->children) {
		Intersection nodeIntersect = node->intersect(ray);
		if (nodeIntersect.did_hit && nodeIntersect.t > 0.01) {
			if (nodeIntersect.t < lowestT) {
				frontIntersect = nodeIntersect;
				lowestT = nodeIntersect.t;
			}
		}
	}
	return frontIntersect;
}

void GeometryNode::applyTrans(mat4 parentTrans) {
	parentTrans = parentTrans * trans;
	/*
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			cout << trans[j][i] << ",";
		}
		cout << endl;
	}
	*/
	this->m_primitive->applyTrans(parentTrans);
	for (SceneNode * child : this->children) {
		child->applyTrans(parentTrans);
	}
}
