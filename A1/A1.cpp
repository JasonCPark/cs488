#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;
const float PI = 3.14159265f;

static const int maxScale = 5;
static const int minScale = -5;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 ),
	  mouseButtonActive(false),
	  activeX(0),
	  activeZ(0),
	  num_of_cubes(0),
	  currentScale(0)
{
	for (int i=0; i<8; i++)
	{
		colour[i][0] = 0.0f;
		colour[i][1] = 0.0f;
		colour[i][2] = 0.0f;
	}

	heights = new int[(int)pow(DIM,2)];
	cellClr = new int[(int)pow(DIM,2)];
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
	delete [] heights;
	delete [] cellClr;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	//col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();
	initCubes();
	initActive(activeX,activeZ);

	setInitVals();
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	float *colours = new float [ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}
	for ( int i = 0; i < sz; i++ ) {
		colours[i] = 1.0f;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the grid vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Create the grid vertex colour buffer
	GLuint colourbuffer;
	glGenBuffers( 1, &colourbuffer );
	glBindBuffer( GL_ARRAY_BUFFER, colourbuffer );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		colours, GL_STATIC_DRAW );

	// Specify the means of extracting the colour values properly.
	GLint colAttrib = m_shader.getAttribLocation( "vertexColour" );
	glEnableVertexAttribArray( colAttrib );
	glVertexAttribPointer( colAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;
	delete [] colours;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A1::initCubes()
{
	GLfloat* cube_verts = {};
	GLfloat* cube_clrs = {};
	int cube_count = 0;
	for (int i=0; i<(int)pow(DIM,2); i++)
	{
		if (heights[i] > 0)
		{
			int vert_count = cube_count * 108;
			GLfloat* new_cube = makeCubeVA((float)(i/DIM),(float)(i%DIM),(float)heights[i]);
			GLfloat* new_clr = makeClrVA(cellClr[i]);

			GLfloat* total_verts = new GLfloat[vert_count + 108];
			std::copy(cube_verts, cube_verts + vert_count, total_verts);
			std::copy(new_cube, new_cube + 108, total_verts + vert_count);
			GLfloat* temp = cube_verts;
			cube_verts = total_verts;
			delete [] temp;
			delete [] new_cube;

			GLfloat* total_clrs = new GLfloat[vert_count + 108];
			std::copy(cube_clrs, cube_clrs + vert_count, total_clrs);
			std::copy(new_clr, new_clr + 108, total_clrs + vert_count);
			GLfloat* tempClr = cube_clrs;
			cube_clrs = total_clrs;
			delete [] tempClr;
			delete [] new_clr;

			cube_count++;
		}
	}
	num_of_cubes = cube_count;
	

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_cube_vao );
	glBindVertexArray( m_cube_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_cube_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_cube_vbo );
	glBufferData( GL_ARRAY_BUFFER, num_of_cubes*108*sizeof(float),
		cube_verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Create the cube colour vertex buffer
	GLuint colourbuffer;
	glGenBuffers( 1, &colourbuffer );
	glBindBuffer( GL_ARRAY_BUFFER, colourbuffer );
	glBufferData( GL_ARRAY_BUFFER, num_of_cubes*108*sizeof(float),
		cube_clrs, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint colAttrib = m_shader.getAttribLocation( "vertexColour" );
	glEnableVertexAttribArray( colAttrib );
	glVertexAttribPointer( colAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	delete [] cube_verts;
	delete [] cube_clrs;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A1::initActive(float x, float z)
{
	GLfloat* active_verts = new GLfloat[24] {
		0+x, 0.01, 0+z,
		0+x, 0.01, 1+z,

		1+x, 0.01, 0+z,
		1+x, 0.01, 1+z,

		0+x, 0.01, 0+z,
		1+x, 0.01, 0+z,

		0+x, 0.01, 1+z,
		1+x, 0.01, 1+z,
	};

	GLfloat* active_clr = new GLfloat[24] {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_active_vao );
	glBindVertexArray( m_active_vao );

	// Create the active cell vertex buffer
	glGenBuffers( 1, &m_active_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_active_vbo );
	glBufferData( GL_ARRAY_BUFFER, 24*sizeof(float),
		active_verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Create the active cell vertex buffer
	GLuint colourBuffer;
	glGenBuffers( 1, &colourBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, colourBuffer );
	glBufferData( GL_ARRAY_BUFFER, 24*sizeof(float),
		active_clr, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint colAttrib = m_shader.getAttribLocation( "vertexColour" );
	glEnableVertexAttribArray( colAttrib );
	glVertexAttribPointer( colAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	delete [] active_verts;
	delete [] active_clr;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Returns cube vertex array.
 */
GLfloat* A1::makeCubeVA(float x, float z, float height)
{
	GLfloat* cube_verts = new GLfloat[108] {
		0+x, 0, 0+z,
		0+x, 0, 1+z,
		1+x, 0, 1+z,
		1+x, 0, 1+z,
		1+x, 0, 0+z,
		0+x, 0, 0+z,

		0+x, 0+height, 0+z,
		0+x, 0+height, 1+z,
		1+x, 0+height, 1+z,
		1+x, 0+height, 1+z,
		1+x, 0+height, 0+z,
		0+x, 0+height, 0+z,

		0+x, 0+height, 0+z,
		0+x, 0, 0+z,
		1+x, 0, 0+z,
		1+x, 0, 0+z,
		1+x, 0+height, 0+z,
		0+x, 0+height, 0+z,

		0+x, 0+height, 1+z,
		0+x, 0, 1+z,
		1+x, 0, 1+z,
		1+x, 0, 1+z,
		1+x, 0+height, 1+z,
		0+x, 0+height, 1+z,

		0+x, 0+height, 1+z,
		0+x, 0, 1+z,
		0+x, 0, 0+z,
		0+x, 0, 0+z,
		0+x, 0+height, 0+z,
		0+x, 0+height, 1+z,

		1+x, 0+height, 1+z,
		1+x, 0, 1+z,
		1+x, 0, 0+z,
		1+x, 0, 0+z,
		1+x, 0+height, 0+z,
		1+x, 0+height, 1+z,
	};

	return cube_verts;
}


//----------------------------------------------------------------------------------------
GLfloat* A1::makeClrVA(int clr)
{
	float r = colour[clr][0];
	float g = colour[clr][1];
	float b = colour[clr][2];
	GLfloat* vert_clrs = new GLfloat[108];
	for (int i=0; i<36; i++)
	{
		vert_clrs[i*3] = r;
		vert_clrs[i*3+1] = g;
		vert_clrs[i*3+2] = b;
	}
	return vert_clrs;
}

//----------------------------------------------------------------------------------------
/*
 * Set back to initial values.
 */
void A1::setInitVals()
{
	view = glm::lookAt( 
		glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );

	mouseButtonActive = false;
	activeX = 0;
	activeZ = 0;
	num_of_cubes = 0;
	current_col = 0;

	for (int i=0; i<8; i++)
	{
		colour[i][0] = 0.0f;
		colour[i][1] = 0.0f;
		colour[i][2] = 0.0f;
	}

	for (int i=0; i<(int)pow(DIM,2); i++)
	{
		heights[i] = 0;
		cellClr[i] = 0;
	}

	initCubes();
	initActive(activeX,activeZ);
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset" ) ) {
			// Reset to initial values
			setInitVals();
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		for (int i=0; i<8; i++)
		{
			ImGui::PushID( i );
			ImGui::ColorEdit3( "##Colour", colour[i] );
			ImGui::SameLine();
			if( ImGui::RadioButton( "##Col", &current_col, i ) ) {
				cellClr[activeX*DIM+activeZ] = current_col;
				initCubes();
			}
			ImGui::PopID();
		}

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		//glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		glBindVertexArray( m_cube_vao );
		glDrawArrays( GL_TRIANGLES, 0, num_of_cubes*108 );

		// Highlight the active square.
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindVertexArray( m_active_vao );
		//glUniform3f(col_uni, 1.0f, 0.0f, 0.0f);
		glDrawArrays( GL_LINES, 0, 24 );

	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);
	current_xpos = xPos;

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.

		if (mouseButtonActive) {
			float xdiff = xPos - prev_xpos;
			vec3 y_axis(0.0f, 1.0f, 0.0f);
			view = prev_view * glm::rotate(mat4(), xdiff/100, y_axis);

		}
	}
	initCubes();

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
		if (actions == GLFW_PRESS) {
			mouseButtonActive = true;
			prev_xpos = current_xpos;
			prev_view = view;
		}
	} 

    if (actions == GLFW_RELEASE) {
        mouseButtonActive = false;
    }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.
	if (yOffSet < 0 && currentScale > minScale)
	{
		view *= glm::scale(mat4(), vec3(0.8));
		currentScale--;
	}
	else if (yOffSet > 0 && currentScale < maxScale)
	{
		view *= glm::scale(mat4(), vec3(1.25));
		currentScale++;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) 
{
	bool eventHandled(false);
	bool copyPrevCell = (mods & GLFW_MOD_SHIFT);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {

		if (key == GLFW_KEY_Q) {
			cout << "Q key pressed" << endl;
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_R) {
			cout << "R key pressed" << endl;
			setInitVals();
			eventHandled = true;
		}
		if (key == GLFW_KEY_UP) {
			cout << "Up key pressed" << endl;
			moveActiveCell(0, -1, copyPrevCell);
			eventHandled = true;
		}
		if (key == GLFW_KEY_DOWN) {
			cout << "Down key pressed" << endl;
			moveActiveCell(0, 1, copyPrevCell);
			eventHandled = true;
		}
		if (key == GLFW_KEY_LEFT) {
			cout << "Left key pressed" << endl;
			moveActiveCell(-1, 0, copyPrevCell);
			eventHandled = true;
		}
		if (key == GLFW_KEY_RIGHT) {
			cout << "Right key pressed" << endl;
			moveActiveCell(1, 0, copyPrevCell);
			eventHandled = true;
		}
		if (key == GLFW_KEY_SPACE) {
			cout << "Space key pressed" << endl;
			addCube(activeX, activeZ);
			eventHandled = true;
		}
		if (key == GLFW_KEY_BACKSPACE) {
			cout << "Backspace key pressed" << endl;
			removeCube(activeX, activeZ);
			eventHandled = true;
		}
	}

	initActive(activeX,activeZ);
	return eventHandled;
}


//----------------------------------------------------------------------------------------
void A1::moveActiveCell(float xchange, float zchange, bool copyPrevCell)
{
	int index = activeX*DIM+activeZ;
	int prevHeight = heights[index];
	int prevColour = cellClr[index];

	activeX += xchange;
	activeZ += zchange;

	if (activeX >= DIM || activeX < 0 ||
		activeZ >= DIM || activeZ < 0)
	{
		activeX -= xchange;
		activeZ -= zchange;
	}
	if (copyPrevCell)
	{
		index = activeX*DIM+activeZ;
		heights[index] = prevHeight;
		cellClr[index] = prevColour;
		initCubes();
	}
}


//----------------------------------------------------------------------------------------
void A1::addCube(int x, int z)
{
	int index = DIM*x + z;
	heights[index] += 1;
	cellClr[index] = current_col;
	initCubes();
}

//----------------------------------------------------------------------------------------
void A1::removeCube(int x, int z)
{
	int index = DIM*x + z;
	if (heights[index] > 0)
	{
		heights[index]--;
	}
	initCubes();
}
