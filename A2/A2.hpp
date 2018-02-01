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


	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);


	enum class Mode {RotView, TranslView, Persp, RotMod, TranslMod, ScaMod, ViewP};
	Mode currentMode = Mode::RotMod;
	int currentModeInt = 3;
	enum class Axis {x, y, z};
	void updateCurrentMode();

	void setInitVals();
	void initModel();
	void initWorld();
	void initCube();

	void transform();
	void orthoProject();
	void perspProject();
	void clipEdge(int i, glm::vec4 vect1, glm::vec4 vect2, glm::vec2* targ1, glm::vec2* targ2);
	void draw2D();
	void drawGnomon();
	void drawViewPort();

	void applyTransMat(float x, float y, float z);
	glm::mat4 rotateX(float angle);
	glm::mat4 rotateY(float angle);
	glm::mat4 rotateZ(float angle);
	glm::mat4 translate(float xDist, float yDist, float zDist, glm::mat4 rotMat);
	glm::mat4 scale(float x, float y, float z);
	glm::mat4 localizeTranslate(glm::mat4 translM, glm::mat4 rotateM);
	glm::vec2 viewPortTrans(glm::vec2 vert);
	glm::vec2 getIntersectVert(glm::vec4 vert1, glm::vec4 vert2, float a);
	unsigned char getAutoCode(glm::vec4 vert);

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	glm::vec2 lines[12][2];
	glm::vec2 model_gnomon_lines[3][2];
	glm::vec2 world_gnomon_lines[3][2];
	
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

	int gnomon_edges[3][2] = {
		{0,1},
		{0,2},
		{0,3}
	};

	glm::vec4 model_verts[8];
	glm::vec4 model_gnomon[4];

	glm::vec4 world_gnomon[4];

	glm::vec4 transformed_verts[8];
	glm::vec4 proj_verts[8];
	glm::vec2 verts_2D[8];

	glm::vec4 transformed_model_gnomon[4];
	glm::vec4 proj_model_gnomon_verts[8];
	glm::vec2 model_gnomon_2D[4];

	glm::vec4 transformed_world_gnomon[4];
	glm::vec4 proj_world_gnomon_verts[8];
	glm::vec2 world_gnomon_2D[4];

	glm::mat4 rotatePrev;
	glm::mat4 scalePrev;
	glm::mat4 translPrev;

	glm::mat4 rotateMat;
	glm::mat4 scaleMat;
	glm::mat4 translMat;

	glm::mat4 viewMat;
	glm::mat4 viewPrev;

	float n = 3;
	float nPrev = n;
	float f = 7;
	float fPrev = f;
	float l = -0.9;
	float r = 0.9;
	float t = 0.9;
	float b = -0.9;
	float th = 0.52f;
	float thPrev = th;

	float viewpT = 0.9;
	float viewpB = -0.9;
	float viewpL = -0.9;
	float viewpR = 0.9;

	// Fields related to rotation by mouse drag
	float current_xpos;
	float current_ypos;
	float prev_xpos_L;
	float prev_ypos_L;
	float prev_xpos_M;
	float prev_xpos_R;
	bool mouseActive;
    bool mouseLActive;
    bool mouseMActive;
    bool mouseRActive;
	
	float winWidth = 768;
	float winHeight = 768;
};
