#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initCube();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);


	void setInitVals();

	void orthoProject();

	void draw2D();

	void rotateX(float angle);
	void rotateY(float angle);
	
	void transform();

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	
	int edges[12][2] = {
		{0,1},
		{1,2},
		{2,3},
		{3,0},

		{4,5},
		{5,6},
		{6,7},
		{7,4},

		{0,4},
		{1,5},
		{2,6},
		{3,7}
	};

	glm::vec4 model_verts[8];
	glm::vec4 transformed_verts[8];
	glm::vec2 verts_2D[8];

	glm::mat4 modelMat;
	glm::mat4 prev_model;

	glm::mat4 xRotationMat;
	glm::mat4 yRotationMat;

	// Fields related to rotation by mouse drag
	float current_xpos;
	float prev_xpos_L;
	float prev_xpos_M;
	float prev_xpos_R;
    bool mouseLActive;
    bool mouseMActive;
    bool mouseRActive;
	
};
