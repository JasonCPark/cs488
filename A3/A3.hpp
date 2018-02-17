#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"

#include <glm/glm.hpp>
#include <memory>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(SceneNode &node);
	void renderArcCircle();

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;


	// 

	glm::vec3 get_arcball_vec(int x, int y);

	// Interface Elements
	int currentMode = 0; // Position/Orientation Mode

	// Mouse Elements
	float current_x = 0;
	float current_y = 0;
	float prev_x_L = 0;
	float prev_y_L = 0;
	float prev_x_M = 0;
	float prev_y_M = 0;
	float prev_x_R = 0;
	float prev_y_R = 0;

	bool mouseLActive = false;
	bool mouseMActive = false;
	bool mouseRActive = false;

	void applyMouse(
		float diffLx, float diffLy,
		float diffMx, float diffMy, 
		float diffRx, float diffRy);

	// Trackball Elements
	void vCalcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter,
                 float *fVecX, float *fVecY, float *fVecZ);

	glm::mat4 vAxisRotMatrix(float fVecX, float fVecY, float fVecZ);

	// Window Elemenets
	float winWidth = 1024;
	float winHeight = 768;

	// Picking Elements
	int jointMap(int ind);
	bool do_picking = false;
	bool selected[46];

	// Option Elements
	bool render_circle = false;
	bool z_buffer = true;
	bool backface_cull = false;
	bool frontface_cull = false;
};
