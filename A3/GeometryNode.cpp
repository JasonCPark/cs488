#include "GeometryNode.hpp"

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		bool do_picking
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		if( do_picking ) {
			float r = float(node.m_nodeId&0xff) / 255.0f;
			float g = float((node.m_nodeId>>8)&0xff) / 255.0f;
			float b = float((node.m_nodeId>>16)&0xff) / 255.0f;

			location = shader.getUniformLocation("material.kd");
			glUniform3f( location, r, g, b );
			CHECK_GL_ERRORS;
		} else {
			//-- Set NormMatrix:
			location = shader.getUniformLocation("NormalMatrix");
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;


			//-- Set Material values:
			location = shader.getUniformLocation("material.kd");
			vec3 kd = node.material.kd;
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.ks");
			vec3 ks = node.material.ks;
			glUniform3fv(location, 1, value_ptr(ks));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.shininess");
			glUniform1f(location, node.material.shininess);
			CHECK_GL_ERRORS;
		}

	}
	shader.disable();

}

//---------------------------------------------------------------------------------------
void GeometryNode::render(glm::mat4 view, ShaderProgram* shader, BatchInfoMap* batchInfoMap, bool do_picking) {
	updateShaderUniforms(*shader, *this, view, do_picking);

	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	BatchInfo batchInfo = (*batchInfoMap)[this->meshId];

	//-- Now render the mesh:
	shader->enable();
	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	shader->disable();
	for (SceneNode * node : this->children) {
		node->render(view, shader, batchInfoMap, do_picking);
	}
}

//---------------------------------------------------------------------------------------
void GeometryNode::revertSelectionClr() {
	this->material.kd = this->kd;
	this->material.ks = this->ks;
	for (SceneNode * node : this->children) {
		node->revertSelectionClr();
	}
}
