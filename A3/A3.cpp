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

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.


	for (int i=0; i<46; i++) {
		selected[i] = false;
	}
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {

	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("picking");
		glUniform1i( location, do_picking ? 1 : 0 );

		//-- Set LightSource uniform for the scene:
		if( !do_picking ) {
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;

			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
	if (backface_cull && frontface_cull) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	} else if (frontface_cull) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	} else if (backface_cull) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	} else {
		glDisable(GL_CULL_FACE);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);


	ImGui::Begin("Menu", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		
		if (ImGui::BeginMenu("Application"))
		{
		if( ImGui::Button( "Reset All" ) ) {
			m_rootNode->reset();
			for (int i=0; i<46; i++) {
				if (jointMap(i) > -1) {
					GeometryNode* myNode = (GeometryNode*)m_rootNode->getByID(i);
					if (selected[i]) {
						myNode->revertSelectionClr();
					}
				}
				selected[i] = false;
			}
			m_rootNode->applyTranslate(vec3(0,0,0), mat4());
		}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::Checkbox("Circle", &render_circle);
			ImGui::Checkbox("Z-buffer", &z_buffer);
			ImGui::Checkbox("Backface culling", &backface_cull);
			ImGui::Checkbox("Frontface culling", &frontface_cull);
			ImGui::EndMenu();
		}
		ImGui::Text("Interaction Mode"); 
		ImGui::RadioButton( "Position/Orientation", &currentMode, 0 );
		ImGui::RadioButton( "Joints", &currentMode, 1 );

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

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
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {
	if (z_buffer) {
		glEnable( GL_DEPTH_TEST );
	}
	renderSceneGraph(*m_rootNode);

	glDisable( GL_DEPTH_TEST );
	if (render_circle) {
		renderArcCircle();
	}
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);
	root.render(m_view, &m_shader, &m_batchInfoMap, do_picking);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.
/*
	for (const SceneNode * node : root.children) {

		if (node->m_nodeType != NodeType::GeometryNode)
			continue;

		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);

		updateShaderUniforms(m_shader, *geometryNode, m_view);


		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
	}
	*/
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
vec3 A3::get_arcball_vec(int x, int y) {
	
}

//----------------------------------------------------------------------------------------
void A3::applyMouse(
	float diffLx, float diffLy,
	float diffMx, float diffMy, 
	float diffRx, float diffRy)
{
	if (currentMode == 0) // Position/Orientation
	{
		float centerX = winWidth/2;
		float centerY = winHeight/2;
		float curModX = current_x - centerX;
		float curModY = current_y - centerY;
		float prevModY = prev_y_R - centerY;
		float prevModX = prev_x_R - centerX;
		float ballDiameter = (winWidth < winHeight) ? winWidth * 0.5 : winHeight * 0.5;
		float rotVecX, rotVecY, rotVecZ;
		vCalcRotVec(curModX, curModY, 
			prevModX, prevModY, 
			ballDiameter,
			&rotVecX, &rotVecY, &rotVecZ);
		mat4 rotMat = vAxisRotMatrix(-rotVecX, rotVecY, -rotVecZ);
		m_rootNode->applyBallRot(rotMat,mat4());
		if (diffLx != 0 || diffLy != 0 || diffMy != 0) {
			m_rootNode->applyTranslate(vec3(diffLx, -diffLy, diffMy), mat4());
		}
	}
	if (currentMode == 1) // Joints
	{
		for (int i=0; i<46; i++){
			if (selected[i]){
				JointNode* myNode = (JointNode*)m_rootNode->getByID(jointMap(i));
				if (i == 5) {
					myNode->applyRot2(diffMy*50, diffMx*50, mat4());
				} else {
					myNode->applyRot('x', diffMy*50, mat4());
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	current_x = xPos;
	current_y = yPos;

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (mouseLActive || mouseMActive || mouseRActive)
		{
			float diffLx = 0;
			float diffMx = 0;
			float diffRx = 0;
			float diffLy = 0;
			float diffMy = 0;
			float diffRy = 0;
			if (mouseLActive) {
				diffLx = (xPos - prev_x_L)/100;
				diffLy = (yPos - prev_y_L)/100;
			}
			if (mouseMActive) {
				diffMx = (xPos - prev_x_M)/100;
				diffMy = (yPos - prev_y_M)/100;
			}
			if (mouseRActive) {
				diffRx = (xPos - prev_x_R)/100;
				diffRy = (yPos - prev_y_R)/100;
			}
			applyMouse(diffLx, diffLy, diffMx, diffMy, diffRx, diffRy);
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (actions == GLFW_PRESS) {
			m_rootNode->saveTrans();
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				prev_x_L = current_x;
				prev_y_L = current_y;
				mouseLActive = true;

				if (currentMode == 1) { // Joints
					double xpos, ypos;
					glfwGetCursorPos( m_window, &xpos, &ypos );

					do_picking = true;

					uploadCommonSceneUniforms();
					glClearColor(1.0, 1.0, 1.0, 1.0 );
					glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
					glClearColor(0.35, 0.35, 0.35, 1.0);

					draw();

					// I don't know if these are really necessary anymore.
					// glFlush();
					// glFinish();

					CHECK_GL_ERRORS;

					// Ugly -- FB coordinates might be different than Window coordinates
					// (e.g., on a retina display).  Must compensate.
					xpos *= double(m_framebufferWidth) / double(m_windowWidth);
					// WTF, don't know why I have to measure y relative to the bottom of
					// the window in this case.
					ypos = m_windowHeight - ypos;
					ypos *= double(m_framebufferHeight) / double(m_windowHeight);

					GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
					// A bit ugly -- don't want to swap the just-drawn false colours
					// to the screen, so read from the back buffer.
					glReadBuffer( GL_BACK );
					// Actually read the pixel at the mouse location.
					glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
					CHECK_GL_ERRORS;

					// Reassemble the object ID.
					unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

					if( what < 46 ) {
						int ji = jointMap(what);
						if (ji > -1) {
							selected[what] = !selected[what];
							GeometryNode* myNode = (GeometryNode*)m_rootNode->getByID(what);
							if (selected[what]){
								myNode->kd = myNode->material.kd;
								myNode->ks = myNode->material.ks;
								myNode->material.kd = vec3(0,0.282,0.318);
								myNode->material.ks = vec3(0,0.282,0.318);
							} else {
								myNode->material.kd = myNode->kd;
								myNode->material.ks = myNode->ks;
							}
						}
					}

					do_picking = false;

					CHECK_GL_ERRORS;
				}
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				prev_x_M = current_x;
				prev_y_M = current_y;
				mouseMActive = true;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				prev_x_R = current_x;
				prev_y_R = current_y;
				mouseRActive = true;
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
    }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	winWidth = width;
	winHeight = height;
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
int A3::jointMap(int ind) {
	switch(ind) {
		case 3: {
			return 2;
		}
		case 5: {
			return 4;
		}
		case 12: {
			return 11;
			break;
		}
		case 15: {
			return 14;
			break;
		}
		case 18: {
			return 17;
			break;
		}
		case 21: {
			return 20;
			break;
		}
		case 24: {
			return 23;
			break;
		}
		case 27: {
			return 26;
			break;
		}
		case 30: {
			return 29;
			break;
		}
		case 33: {
			return 32;
			break;
		}
		case 36: {
			return 35;
			break;
		}
		case 39: {
			return 38;
			break;
		}
		case 42: {
			return 41;
			break;
		}
		case 45: {
			return 44;
			break;
		}
		default: {
			break;
		}
	}
	return -1;
}

//----------------------------------------------------------------------------------------
/*
 * Trackball code provided by Greg Veres and Michael Hardy.
 */
void A3::vCalcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter,
                 float *fVecX, float *fVecY, float *fVecZ) {
   long  nXOrigin, nYOrigin;
   float fNewVecX, fNewVecY, fNewVecZ,
         fOldVecX, fOldVecY, fOldVecZ,
         fLength;

   fNewVecX    = fNewX * 2.0 / fDiameter;
   fNewVecY    = fNewY * 2.0 / fDiameter;
   fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

   if (fNewVecZ < 0.0) {
      fLength = sqrt(1.0 - fNewVecZ);
      fNewVecZ  = 0.0;
      fNewVecX /= fLength;
      fNewVecY /= fLength;
   } else {
      fNewVecZ = sqrt(fNewVecZ);
   }

   fOldVecX    = fOldX * 2.0 / fDiameter;
   fOldVecY    = fOldY * 2.0 / fDiameter;
   fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);
 
   if (fOldVecZ < 0.0) {
      fLength = sqrt(1.0 - fOldVecZ);
      fOldVecZ  = 0.0;
      fOldVecX /= fLength;
      fOldVecY /= fLength;
   } else {
      fOldVecZ = sqrt(fOldVecZ);
   }

   *fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
   *fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
   *fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;
}

//----------------------------------------------------------------------------------------
/*
 * Trackball code provided by Greg Veres and Michael Hardy.
 */
mat4 A3::vAxisRotMatrix(float fVecX, float fVecY, float fVecZ) {
    float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;
	mat4 mNewMat = mat4(1.0f);

    fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);

    if (fRadians > -0.000001 && fRadians < 0.000001) {
        return mNewMat;
    }

    fInvLength = 1 / fRadians;
    fNewVecX   = fVecX * fInvLength;
    fNewVecY   = fVecY * fInvLength;
    fNewVecZ   = fVecZ * fInvLength;

    double dSinAlpha = sin(fRadians);
    double dCosAlpha = cos(fRadians);
    double dT = 1 - dCosAlpha;

	mNewMat = {
		{
			dCosAlpha + fNewVecX*fNewVecX*dT,
			fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha,
			fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha,
			0
		},
		{
    		fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ,
    		dCosAlpha + fNewVecY*fNewVecY*dT,
    		fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX,
    		0
		},
		{
    		fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY,
    		fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX,
    		dCosAlpha + fNewVecZ*fNewVecZ*dT,
    		0,
		},
		{
    		0,
    		0,
    		0,
    		1
		}
	};
	return mNewMat;
}
