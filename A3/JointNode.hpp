#pragma once

#include "SceneNode.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);
	void applyRot(char axis, float angle, glm::mat4 parentTrans);
	void applyRot2(float angleX, float angleY, glm::mat4 parentTrans);

	struct JointRange {
		double min, init, max;
	};


	JointRange m_joint_x, m_joint_y;
};
