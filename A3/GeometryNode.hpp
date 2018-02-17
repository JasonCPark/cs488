#pragma once

#include "SceneNode.hpp"
#include "A3.hpp"

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);
	void render(glm::mat4 view, ShaderProgram* shader, BatchInfoMap* batchInfoMap, bool do_picking);
	void revertSelectionClr();

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;

	glm::vec3 kd;
	glm::vec3 ks;
};

