#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "grid.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

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

private:
	void initGrid();
	void initCubes();
	void initActive(float x, float z);
	GLfloat* makeCubeVA(float x, float y, float z);
	GLfloat* makeClrVA(int clr);
	void addCube(int x, int z);
	void removeCube(int x, int z);
	void setInitVals();
	void moveActiveCell(float xchange, float zchange, bool copyPrevCell);

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	//GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to cube geometry.
	GLuint m_cube_vao; // Vertex Array Object
	GLuint m_cube_vbo; // Vertex Buffer Object

	// Fields related to active cell geometry.
	GLuint m_active_vao; // Vertex Array Object
	GLuint m_active_vbo; // Vertex Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	float colour[8][3];
	int current_col;

	// Fields related to rotation by mouse drag
	float prev_xpos;
	float current_xpos;
	glm::mat4 prev_view;
    	// True if mouse button is currently down.
    bool mouseButtonActive;

	// Fields related to active cell.
	int activeX;	
	int activeZ;

	// Fields related to bar information.
	int* heights;
	int* cellClr;
	int num_of_cubes;

	int currentScale;
};
