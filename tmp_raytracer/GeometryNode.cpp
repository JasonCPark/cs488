#include "GeometryNode.hpp"
#include <glm/glm.hpp>
using namespace glm;
using namespace std;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat, vec3 motionTransl )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
	, motionTransl(motionTransl)
{
	m_nodeType = NodeType::GeometryNode;
}

GeometryNode::~GeometryNode()
{
	delete m_material;
	delete m_primitive;
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
	Ray oldRay = Ray();
	oldRay.pos = ray.pos;
	oldRay.dir = ray.dir;

	float randFloat = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	mat4 adjustedTrans = glm::translate(motionTransl*randFloat) * trans;
	mat4 adjustedInvTrans = glm::inverse(adjustedTrans);

	ray.pos = vec3(adjustedInvTrans * vec4(ray.pos,1));
	ray.dir = mat3(adjustedInvTrans) * ray.dir;
	Intersection myIntersect = this->m_primitive->intersect(ray);

	Intersection frontIntersect;
	float lowestT;
	if (myIntersect.did_hit && myIntersect.t > 0.01) {
		myIntersect.materia = dynamic_cast<PhongMaterial*>(this->m_material);
		frontIntersect = myIntersect;
		lowestT = myIntersect.t;
		if (this->motionTransl != vec3(0,0,0)) {
			frontIntersect.isMotion = true;
		}
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
	frontIntersect.pos = vec3(adjustedTrans * vec4(frontIntersect.pos, 1));
	frontIntersect.norm = transpose(mat3(adjustedInvTrans)) * frontIntersect.norm;
	frontIntersect.norm = normalize(frontIntersect.norm);

	// Check if motion blur distributed rays are needed
	//adjustedTrans = glm::translate(motionTransl) * trans;
	adjustedTrans = trans;
	adjustedInvTrans = glm::inverse(adjustedTrans);
	ray.pos = vec3(adjustedInvTrans * vec4(oldRay.pos,1));
	ray.dir = mat3(adjustedInvTrans) * oldRay.dir;
	Intersection newIntersect = this->m_primitive->intersect(ray);
	if (newIntersect.did_hit && newIntersect.t > 0.01 && this->motionTransl != vec3(0,0,0)) {
		frontIntersect.isMotion = true;
	}

	return frontIntersect;
}
