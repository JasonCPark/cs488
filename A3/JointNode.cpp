#include "JointNode.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;
using namespace std;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}


//---------------------------------------------------------------------------------------
void JointNode::applyRot(char axis, float angle, glm::mat4 parentTrans) {
	vec3 rot_axis;
	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	localTrans = transl * rot_matrix * invTransl * oldLocalTrans;
	if (parentTrans != mat4()) {
		curParentTrans = parentTrans;
	}
	trans = curParentTrans * localTrans * originalTrans;
	mat4 newParentTrans = curParentTrans * localTrans;
	for (SceneNode * node : this->children) {
		node->applyRot(axis, 0, newParentTrans);
	}
}

//---------------------------------------------------------------------------------------
void JointNode::applyRot2(float angleX, float angleY, glm::mat4 parentTrans) {
	vec3 rot_axis;
	mat4 rot_matrixX = glm::rotate(degreesToRadians(angleX), vec3(1,0,0));
	mat4 rot_matrixY = glm::rotate(degreesToRadians(angleY), vec3(0,1,0));
	localTrans = transl * rot_matrixX * rot_matrixY * invTransl * oldLocalTrans;
	if (parentTrans != mat4()) {
		curParentTrans = parentTrans;
	}
	trans = curParentTrans * localTrans * originalTrans;
	mat4 newParentTrans = curParentTrans * localTrans;
	for (SceneNode * node : this->children) {
		node->applyRot2(0, 0, newParentTrans);
	}
}
