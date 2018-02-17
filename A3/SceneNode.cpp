#include "SceneNode.hpp"

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
	oldTrans(mat4()),
	localTrans(mat4()),
	oldLocalTrans(mat4()),
	originalTrans(mat4()),
	isSelected(false),
	parent(NULL),
	m_nodeId(nodeInstanceCount++)
{
}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans)
{
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::render(glm::mat4 view, ShaderProgram* shader, BatchInfoMap* batchInfoMap, bool do_picking) {
	this->renderChildren(view, shader, batchInfoMap, do_picking);
}

//---------------------------------------------------------------------------------------
void SceneNode::renderChildren(glm::mat4 view, ShaderProgram* shader, BatchInfoMap* batchInfoMap, bool do_picking) {
	for (SceneNode * node : this->children) {
		node->render(view, shader, batchInfoMap, do_picking);
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	child->parent = this;
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
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
	originalTrans = rot_matrix * originalTrans;
	trans = rot_matrix * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	originalTrans = glm::scale(amount) * originalTrans;
	trans = glm::scale(amount) * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	originalTrans = glm::translate(amount) * originalTrans;
	trans = glm::translate(amount) * trans;
	adjustTransl(amount);
	origTransl = transl;
	origInvTransl = invTransl;
}

//---------------------------------------------------------------------------------------
void SceneNode::applyTranslate(const glm::vec3& amount, glm::mat4 parentTrans) {
	localTrans = glm::translate(amount) * oldLocalTrans;
	if (parentTrans != mat4()) {
		curParentTrans = parentTrans;
	}
	trans = curParentTrans * localTrans * originalTrans;
	mat4 newParentTrans = curParentTrans * localTrans;
	adjustTransl(amount);
	for (SceneNode * node : this->children) {
		node->applyTranslate(vec3(0,0,0), newParentTrans);
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::applyRot(char axis, float angle, glm::mat4 parentTrans) {
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
void SceneNode::applyRot2(float angleX, float angleY, glm::mat4 parentTrans) {
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

//---------------------------------------------------------------------------------------
void SceneNode::applyBallRot(glm::mat4 rotMat, glm::mat4 parentTrans) {
	localTrans = transl * rotMat * invTransl * oldLocalTrans;
	if (parentTrans != mat4()) {
		curParentTrans = parentTrans;
	}
	trans = curParentTrans * localTrans * originalTrans;
	mat4 newParentTrans = curParentTrans * localTrans;
	for (SceneNode * node : this->children) {
		node->applyBallRot(mat4(), newParentTrans);
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::adjustTransl(const glm::vec3 amount) {
	transl = glm::translate(amount) * oldTransl;
	glm::vec3 negAmount = amount;
	negAmount.x = -negAmount.x;
	negAmount.y = -negAmount.y;
	negAmount.z = -negAmount.z;
	invTransl = glm::translate(negAmount) * oldInvTransl;
}

//---------------------------------------------------------------------------------------
void SceneNode::saveTrans() {
	oldTrans = trans;
	oldLocalTrans = localTrans;
	oldTransl = transl;
	oldInvTransl = invTransl;
	for (SceneNode * node : this->children) {
		node->saveTrans();
	}
}

//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;
	os << "]";

	return os;
}

//---------------------------------------------------------------------------------------
SceneNode* SceneNode::getByID(unsigned int id) {
	if (this->m_nodeId == id) {
		return this;
	}
	SceneNode* returnNode = NULL;
    for (SceneNode * node : this->children) {
        returnNode = node->getByID(id);
		if (returnNode != NULL) {
			return returnNode;
		}
    }
	return NULL;
}

//---------------------------------------------------------------------------------------
void SceneNode::reset() {
	trans = mat4();
	oldTrans = mat4();
	localTrans = mat4();
	oldLocalTrans = mat4();
	curParentTrans = mat4();
	transl = origTransl;
	oldTransl = origTransl;
	invTransl = origInvTransl;
	oldInvTransl = origInvTransl;
	isSelected = false;
	for (SceneNode* node : this->children) {
		node->reset();
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::revertSelectionClr() {
	for (SceneNode * node : this->children) {
		node->revertSelectionClr();
	}
}
