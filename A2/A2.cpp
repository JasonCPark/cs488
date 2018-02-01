#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

	setInitVals();

	initModel();
	
	initWorld();

	initCube();
}

//----------------------------------------------------------------------------------------
void A2::setInitVals()
{
	n = 3;
	nPrev = n;     
	f = 7;
	fPrev = f;
	l = -0.9;
	r = 0.9;
	t = 0.9;
	b = -0.9;
	th = 0.52f;
	thPrev = th;
	viewpT = 0.9;
	viewpB = -0.9;
	viewpL = -0.9;
	viewpR = 0.9;
	current_xpos = 0.0f;
	current_ypos = 0.0f;
	prev_xpos_L = 0.0f;
	prev_ypos_L = 0.0f;
	prev_xpos_M = 0.0f;
	prev_xpos_R = 0.0f;
	mouseActive = false;
    mouseLActive = false;
    mouseMActive = false;
    mouseRActive = false;
	rotateMat = mat4(1.0f); // Identity Matrix
	scaleMat = mat4(1.0f);
	translMat = mat4(1.0f);
	viewMat = mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, -5, 1
	);
}

//----------------------------------------------------------------------------------------
void A2::initModel()
{
	model_gnomon[0] = vec4(0, 0, 0, 1);
	model_gnomon[1] = vec4(0.25f, 0, 0, 1);
	model_gnomon[2] = vec4(0, 0.25f, 0, 1);
	model_gnomon[3] = vec4(0, 0, 0.25f, 1);
}

//----------------------------------------------------------------------------------------
void A2::initWorld()
{
	world_gnomon[0] = vec4(0, 0, 0, 1);
	world_gnomon[1] = vec4(0.25f, 0, 0, 1);
	world_gnomon[2] = vec4(0, 0.25f, 0, 1);
	world_gnomon[3] = vec4(0, 0, 0.25f, 1);
}

//----------------------------------------------------------------------------------------
void A2::initCube()
{
	model_verts[0] = vec4(0.5f, 0.5f, 0.5f, 1);
	model_verts[1] = vec4(-0.5f, 0.5f, 0.5f, 1);
	model_verts[2] = vec4(-0.5f, 0.5f, -0.5f, 1);
	model_verts[3] = vec4(0.5f, 0.5f, -0.5f, 1);
	model_verts[4] = vec4(0.5f, -0.5f, 0.5f, 1);
	model_verts[5] = vec4(-0.5f, -0.5f, 0.5f, 1);
	model_verts[6] = vec4(-0.5f, -0.5f, -0.5f, 1);
	model_verts[7] = vec4(0.5f, -0.5f, -0.5f, 1);
}


//----------------------------------------------------------------------------------------
void A2::orthoProject()
{
	for (int i=0; i<8; i++)
	{
		verts_2D[i] = vec2(transformed_verts[i].x, transformed_verts[i].y);
	}
	for (int i=0; i<4; i++)
	{
		model_gnomon_2D[i] = vec2(transformed_model_gnomon[i].x, transformed_model_gnomon[i].y);
	}
	for (int i=0; i<4; i++)
	{
		world_gnomon_2D[i] = vec2(transformed_world_gnomon[i].x, transformed_world_gnomon[i].y);
	}
}

//----------------------------------------------------------------------------------------
void A2::perspProject()
{
	mat4 perspMat = mat4(
		(1/tan(th/2))/((r-l)/(t-b)), 0, 0, 0,
		0, 1/tan(th/2), 0, 0,
		0, 0, -(f+n)/(f-n), -1,
		0, 0, -(2*f*n)/(f-n), 0
	);
	for (int i=0; i<8; i++)
	{
		proj_verts[i] = perspMat * transformed_verts[i];
	}
	for (int i=0; i<4; i++)
	{
		proj_model_gnomon_verts[i] = perspMat * transformed_model_gnomon[i];
		proj_world_gnomon_verts[i] = perspMat * transformed_world_gnomon[i];
	}

	for (int i=0; i<12; i++)
	{
		vec4 vert1 = proj_verts[edges[i][0]];
		vec4 vert2 = proj_verts[edges[i][1]];
		clipEdge(i, vert1, vert2, &(lines[i][0]), &(lines[i][1]));
	}
	for (int i=0; i<3; i++)
	{
		vec4 vert1 = proj_model_gnomon_verts[gnomon_edges[i][0]];
		vec4 vert2 = proj_model_gnomon_verts[gnomon_edges[i][1]];
		clipEdge(i, vert1, vert2, &(model_gnomon_lines[i][0]), &(model_gnomon_lines[i][1]));
		vert1 = proj_world_gnomon_verts[gnomon_edges[i][0]];
		vert2 = proj_world_gnomon_verts[gnomon_edges[i][1]];
		clipEdge(i, vert1, vert2, &(world_gnomon_lines[i][0]), &(world_gnomon_lines[i][1]));
	}
}

//----------------------------------------------------------------------------------------
void A2::clipEdge(int i, vec4 vert1, vec4 vert2, vec2* targ1, vec2* targ2)
{
	unsigned char v1Code = getAutoCode(vert1);
	unsigned char v2Code = getAutoCode(vert2);
	unsigned char intersects = v1Code | v2Code;
	*targ1 = vec2(vert1.x/vert1.w, vert1.y/vert1.w);
	*targ2 = vec2(vert2.x/vert2.w, vert2.y/vert2.w);
	if (intersects == 0)
	{
		return;
	}
	if ((v2Code & v1Code) != 0)
	{
		*targ1 = vec2(-1,-1);
		*targ2 = vec2(-1,-1);
		return;
	}
	int invalidInterCount = 0;
	if (intersects & 0x1) {
		float B1 = vert1.w + vert1.x;
		float B2 = vert2.w + vert2.x;
		float a = B1/(B1-B2);
		vec2 inter = getIntersectVert(vert1, vert2, a);
		if (inter.y > -1 && inter.y < 1) {
			if (v1Code & 0x1) {
				*targ1 = inter;
			} else {
				*targ2 = inter;
			}
		} else { invalidInterCount++; }
	}
	if (intersects & 0x2) {
		float B1 = vert1.w - vert1.x;
		float B2 = vert2.w - vert2.x;
		float a = B1/(B1-B2);
		vec2 inter = getIntersectVert(vert1, vert2, a);
		if (inter.y > -1 && inter.y < 1) {
			if (v1Code & 0x2) {
				*targ1 = inter;
			} else {
				*targ2 = inter;
			}
		} else { invalidInterCount++; }
	}
	if (intersects & 0x4) {
		float B1 = vert1.w + vert1.y;
		float B2 = vert2.w + vert2.y;
		float a = B1/(B1-B2);
		vec2 inter = getIntersectVert(vert1, vert2, a);
		if (inter.x > -1 && inter.x < 1) {
			if (v1Code & 0x4) {
				*targ1 = inter;
			} else {
				*targ2 = inter;
			}
		} else { invalidInterCount++; }
	}
	if (intersects & 0x8) {
		float B1 = vert1.w - vert1.y;
		float B2 = vert2.w - vert2.y;
		float a = B1/(B1-B2);
		vec2 inter = getIntersectVert(vert1, vert2, a);
		if (inter.x > -1 && inter.x < 1) {
			if (v1Code & 0x8) {
				*targ1 = inter;
			} else {
				*targ2 = inter;
			}
		} else { invalidInterCount++; }
	}
	if (intersects & 0x10) {
		float B1 = vert1.w + vert1.z;
		float B2 = vert2.w + vert2.z;
		float a = B1/(B1-B2);
		vec2 inter = getIntersectVert(vert1, vert2, a);
		if (inter.x > -1 && inter.x < 1 && inter.y > -1 && inter.y < 1) {
			if (v1Code & 0x10) {
				*targ1 = inter;
			} else {
				*targ2 = inter;
			}
		} else { invalidInterCount++; }
	}
	if (intersects & 0x20) {
		float B1 = vert1.w - vert1.z;
		float B2 = vert2.w - vert2.z;
		float a = B1/(B1-B2);
		vec2 inter = getIntersectVert(vert1, vert2, a);
		if (inter.x > -1 && inter.x < 1 && inter.y > -1 && inter.y < 1) {
			if (v1Code & 0x20) {
				*targ1 = inter;
			} else {
				*targ2 = inter;
			}
		} else { invalidInterCount++; }
	}
	if (invalidInterCount > 1) {
		*targ1 = vec2(-1,-1);
		*targ2 = vec2(-1,-1);
	}
}

//----------------------------------------------------------------------------------------
vec2 A2::getIntersectVert(vec4 vert1, vec4 vert2, float a)
{
	vec4 intersect = (1-a)*vert1 + a*vert2;
	return vec2(intersect.x/intersect.w, intersect.y/intersect.w);
}

//----------------------------------------------------------------------------------------
unsigned char A2::getAutoCode(vec4 vert)
{
	unsigned char flag = 0;
	if (vert.w + vert.x < 0) {
		flag |= 0x1;
	}
	if (vert.w - vert.x < 0) {
		flag |= 0x2;
	}
	if (vert.w + vert.y < 0) {
		flag |= 0x4;
	}
	if (vert.w - vert.y < 0) {
		flag |= 0x8;
	}
	if (vert.w + vert.z < 0) {
		flag |= 0x10;
	}
	if (vert.w - vert.z < 0) {
		flag |= 0x20;
	}
	return flag;
}

//----------------------------------------------------------------------------------------
void A2::applyTransMat(float x, float y, float z)
{
	if (currentMode == Mode::RotMod)
	{
		mat4 xRotate = rotateX(x);
		mat4 yRotate = rotateY(y);
		mat4 zRotate = rotateZ(z);
		rotateMat = rotatePrev * xRotate * yRotate * zRotate;
	}
	if  (currentMode == Mode::TranslMod)
	{
		mat4 xTransl = translate(x, 0, 0, rotateMat);
		mat4 yTransl = translate(0, y, 0, rotateMat);
		mat4 zTransl = translate(0, 0, z, rotateMat);
		translMat = translPrev * xTransl * yTransl * zTransl;
	}
	if  (currentMode == Mode::ScaMod)
	{
		mat4 xScale = scale(1+x, 1, 1);
		mat4 yScale = scale(1, 1+y, 1);
		mat4 zScale = scale(1, 1, 1+z);
		scaleMat = scalePrev * xScale * yScale * zScale;
	}
	if (currentMode == Mode::RotView)
	{
		mat4 xRotate = rotateX(x);
		mat4 yRotate = rotateY(y);
		mat4 zRotate = rotateZ(z);
		viewMat = zRotate * yRotate * xRotate * viewPrev;
	}
	if (currentMode == Mode::TranslView)
	{
		mat4 xTransl = translate(-x, 0, 0, mat4(1.0f));
		mat4 yTransl = translate(0, -y, 0, mat4(1.0f));
		mat4 zTransl = translate(0, 0, -z, mat4(1.0f));
		viewMat = zTransl * yTransl * xTransl * viewPrev;
	}
	if (currentMode == Mode::Persp)
	{
		th = clamp(thPrev + x, 0.09f, 2.8f);
		n = nPrev + y;
		f = fPrev + z;
	}
	if (currentMode == Mode::ViewP)
	{
		float norm_prev_xpos = clamp(prev_xpos_L/(winWidth/2)-1, -1.0f, 1.0f);
		float norm_xpos = clamp(current_xpos/(winWidth/2)-1, -1.0f, 1.0f);
		float norm_prev_ypos = clamp(-(prev_ypos_L/(winHeight/2)-1), -1.0f, 1.0f);
		float norm_ypos = clamp(-(current_ypos/(winHeight/2)-1), -1.0f, 1.0f);
		if (norm_prev_xpos < norm_xpos) {
			viewpL = norm_prev_xpos;
			viewpR = norm_xpos;
		} else {
			viewpL = norm_xpos;
			viewpR = norm_prev_xpos;
		}
		if (norm_prev_ypos > norm_ypos) { 
			viewpT = norm_prev_ypos;
			viewpB = norm_ypos;
		} else {
			viewpT = norm_ypos;
			viewpB = norm_prev_ypos;
		}
	}
}

//----------------------------------------------------------------------------------------
void A2::drawViewPort()
{
	vec2 tl = vec2(viewpL,viewpT);
	vec2 tr = vec2(viewpR,viewpT);
	vec2 bl = vec2(viewpL,viewpB);
	vec2 br = vec2(viewpR,viewpB);
	setLineColour(vec3(1, 1, 0));
	drawLine(tl,tr);
	drawLine(tr,br);
	drawLine(br,bl);
	drawLine(bl,tl);
}

//----------------------------------------------------------------------------------------
mat4 A2::rotateX(float angle)
{
	return mat4(
		1, 0, 0, 0,
		0, cos(angle), sin(angle), 0,
		0, -sin(angle), cos(angle), 0,
		0, 0, 0, 1
	);
}

//----------------------------------------------------------------------------------------
mat4 A2::rotateY(float angle)
{
	return mat4(
		cos(angle), 0, -sin(angle), 0, 
		0, 1, 0, 0,
		sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1
	);
}

//----------------------------------------------------------------------------------------
mat4 A2::rotateZ(float angle)
{
	return mat4(
		cos(angle), sin(angle), 0, 0, 
		-sin(angle), cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

//----------------------------------------------------------------------------------------
mat4 A2::translate(float xDist, float yDist, float zDist, mat4 rotMat)
{
	vec4 translVec = vec4(xDist, yDist, zDist, 1);
	translVec = rotMat * translVec;
	return mat4(
		vec4(1, 0, 0, 0),
		vec4(0, 1, 0, 0),
		vec4(0, 0, 1, 0),
		translVec
	);
}

//----------------------------------------------------------------------------------------
mat4 A2::scale(float x, float y, float z)
{
	return mat4(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	);
}

//----------------------------------------------------------------------------------------
mat4 A2::localizeTranslate(mat4 translM, mat4 rotateM)
{
	mat4 localTransl = translM;
	vec4 translVec = translMat[3];
	translVec = rotateM * translVec;
	localTransl[3] = translVec;
	return localTransl;
}

//----------------------------------------------------------------------------------------
vec2 A2::viewPortTrans(vec2 vert)
{
	float w = viewpR-viewpL;
	float h = viewpT-viewpB;
	return vec2((vert.x+1)*(w/2)+viewpL,(vert.y+1)*(h/2)+viewpB);
}

//----------------------------------------------------------------------------------------
void A2::transform()
{
	for (int i=0; i<8; i++)
	{
		transformed_verts[i] =
			viewMat *
			translMat * rotateMat * scaleMat *
			model_verts[i];
	}
	for (int i=0; i<4; i++)
	{
		transformed_model_gnomon[i] = 
			viewMat *
			translMat * rotateMat * 
			model_gnomon[i];
	}
	for (int i=0; i<4; i++)
	{
		transformed_world_gnomon[i] = 
			viewMat *
			world_gnomon[i];
	}
}

//----------------------------------------------------------------------------------------
void A2::draw2D()
{
	setLineColour(vec3(1, 1, 1));
	for (int i=0; i<12; i++)
	{
		drawLine(viewPortTrans(lines[i][0]), 
			viewPortTrans(lines[i][1]));
	}
	for (int i=0; i<3; i++)
	{
		drawLine(viewPortTrans(model_gnomon_lines[i][0]), 
			viewPortTrans(model_gnomon_lines[i][1]));
		drawLine(viewPortTrans(world_gnomon_lines[i][0]), 
			viewPortTrans(world_gnomon_lines[i][1]));
	}
	drawGnomon();
}

//----------------------------------------------------------------------------------------
void A2::drawGnomon()
{
	setLineColour(vec3(1, 0, 0));
	drawLine(viewPortTrans(model_gnomon_lines[0][0]), 
		viewPortTrans(model_gnomon_lines[0][1]));
	drawLine(viewPortTrans(world_gnomon_lines[0][0]), 
		viewPortTrans(world_gnomon_lines[0][1]));
	setLineColour(vec3(0, 1, 0));
	drawLine(viewPortTrans(model_gnomon_lines[1][0]), 
		viewPortTrans(model_gnomon_lines[1][1]));
	drawLine(viewPortTrans(world_gnomon_lines[1][0]), 
		viewPortTrans(world_gnomon_lines[1][1]));
	setLineColour(vec3(0, 0, 1));
	drawLine(viewPortTrans(model_gnomon_lines[2][0]), 
		viewPortTrans(model_gnomon_lines[2][1]));
	drawLine(viewPortTrans(world_gnomon_lines[2][0]), 
		viewPortTrans(world_gnomon_lines[2][1]));
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//----------------------------------------------------------------------------------------
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & v0,   // Line Start (NDC coordinate)
		const glm::vec2 & v1    // Line End (NDC coordinate)
) {
	m_vertexData.positions[m_vertexData.index] = v0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = v1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	updateCurrentMode();
	initLineData();
	transform();
	perspProject();
	draw2D();
	drawViewPort();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	
	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...


		if( ImGui::Button( "Reset" ) ) {
			// Reset to initial values
			setInitVals();
		}

		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Interaction Mode" );
		ImGui::RadioButton( "Rotate View", &currentModeInt, 0 );
		ImGui::RadioButton( "Translate View", &currentModeInt, 1 );
		ImGui::RadioButton( "Perspective", &currentModeInt, 2 );
		ImGui::RadioButton( "Rotate Model", &currentModeInt, 3 );
		ImGui::RadioButton( "Translate Model", &currentModeInt, 4 );
		ImGui::RadioButton( "Scale Model", &currentModeInt, 5 );
		ImGui::RadioButton( "Viewport", &currentModeInt, 6 );

		ImGui::Text( "Near Plane: %.1f", n );
		ImGui::Text( "Far Plane: %.1f", f );

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::updateCurrentMode()
{
	switch(currentModeInt)
	{
		case 0: currentMode = Mode::RotView;
			break;
		case 1: currentMode = Mode::TranslView;
			break;
		case 2: currentMode = Mode::Persp;
			break;
		case 3: currentMode = Mode::RotMod;
			break;
		case 4: currentMode = Mode::TranslMod;
			break;
		case 5: currentMode = Mode::ScaMod;
			break;
		case 6: currentMode = Mode::ViewP;
			break;
	}

}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);
	current_xpos = xPos;
	current_ypos = yPos;

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (mouseLActive || mouseMActive || mouseRActive)
		{
			float diffL = 0;
			float diffM = 0;
			float diffR = 0;
			if (mouseLActive) {
				diffL = (xPos - prev_xpos_L)/100;
			}
			if (mouseMActive) {
				diffM = (xPos - prev_xpos_M)/100;
			}
			if (mouseRActive) {
				diffR = (xPos - prev_xpos_R)/100;
			}
			applyTransMat(diffL, diffM, diffR);
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (actions == GLFW_PRESS) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				prev_xpos_L = current_xpos;
				prev_ypos_L = current_ypos;
				mouseLActive = true;
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				prev_xpos_M = current_xpos;
				mouseMActive = true;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				prev_xpos_R = current_xpos;
				mouseRActive = true;
			}
			bool prevMouseActive = mouseActive;
			mouseActive = mouseLActive || mouseMActive || mouseRActive;

			if (!prevMouseActive && mouseActive)
			{
				rotatePrev = rotateMat;
				scalePrev = scaleMat;
				translPrev = translMat;
				viewPrev = viewMat;
				nPrev = n;
				fPrev = f;
				thPrev = th;
			}
		}
	} 

    if (actions == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			mouseLActive = false;
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			mouseMActive = false;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			mouseRActive = false;
		}
		mouseActive = mouseLActive || mouseMActive || mouseRActive;
    }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	winWidth = width;
	winHeight = height;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {

		if (key == GLFW_KEY_Q) {
			cout << "Q key pressed" << endl;
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_A) {
			cout << "A key pressed" << endl;
			setInitVals();
			eventHandled = true;
		}
		if (key == GLFW_KEY_R) {
			cout << "R key pressed" << endl;
			currentModeInt = 3;
			eventHandled = true;
		}
		if (key == GLFW_KEY_T) {
			cout << "T key pressed" << endl;
			currentModeInt = 4;
			eventHandled = true;
		}
		if (key == GLFW_KEY_S) {
			cout << "S key pressed" << endl;
			currentModeInt = 5;
			eventHandled = true;
		}
		if (key == GLFW_KEY_O) {
			cout << "O key pressed" << endl;
			currentModeInt = 0;
			eventHandled = true;
		}
		if (key == GLFW_KEY_N) {
			cout << "N key pressed" << endl;
			currentModeInt = 1;
			eventHandled = true;
		}
		if (key == GLFW_KEY_P) {
			cout << "P key pressed" << endl;
			currentModeInt = 2;
			eventHandled = true;
		}
		if (key == GLFW_KEY_V) {
			cout << "V key pressed" << endl;
			currentModeInt = 6;
			eventHandled = true;
		}
	}

	return eventHandled;
}
