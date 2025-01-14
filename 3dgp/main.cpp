#include <iostream>
#include <GL/glew.h>
#include <3dgl/3dgl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

// GLSL Program
C3dglProgram program;

// 3D models
C3dglModel camera;

C3dglModel table;

C3dglModel vase;

C3dglModel bunny;


// The View Matrix
mat4 matrixView;

// Camera & navigation
float maxspeed = 4.f;	// camera max speed
float accel = 4.f;		// camera acceleration
vec3 _acc(0), _vel(0);	// camera acceleration and velocity vectors
float _fov = 60.f;		// field of view (zoom)

// buffers names
unsigned vertexBuffer = 0;
unsigned normalBuffer = 0;
unsigned indexBuffer = 0;

float pyramidRotation = 0.0f;



bool init()
{
	// shaders
	C3dglShader vertexShader;
	C3dglShader fragmentShader;

	if (!vertexShader.create(GL_VERTEX_SHADER)) return false;
	if (!vertexShader.loadFromFile("shaders/basic.vert")) return false;
	if (!vertexShader.compile()) return false;

	if (!fragmentShader.create(GL_FRAGMENT_SHADER)) return false;
	if (!fragmentShader.loadFromFile("shaders/basic.frag")) return false;
	if (!fragmentShader.compile()) return false;

	if (!program.create()) return false;
	if (!program.attach(vertexShader)) return false;
	if (!program.attach(fragmentShader)) return false;
	if (!program.link()) return false;
	if (!program.use(true)) return false;

	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!


	// load your 3D models here!
	if (!camera.load("models\\camera.3ds")) return false;

	if (!table.load("models\\table.obj")) return false;

	if (!vase.load("models\\vase.obj")) return false;

	if (!bunny.load("models\\bunny.obj")) return false;

	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1), radians(12.f), vec3(1, 0, 0));
	matrixView *= lookAt(
		vec3(0.0, 5.0, 10.0),
		vec3(0.0, 5.0, 0.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.18f, 0.25f, 0.22f, 1.0f);   // deep grey background

	// glut additional setup
	glutSetVertexAttribCoord3(program.getAttribLocation("aVertex"));
	glutSetVertexAttribNormal(program.getAttribLocation("aNormal"));

	float vertices[] = {
		-4, 0, -4, 4, 0, -4, 0, 7, 0, -4, 0, 4, 4, 0, 4, 0, 7, 0,
		-4, 0, -4, -4, 0, 4, 0, 7, 0, 4, 0, -4, 4, 0, 4, 0, 7, 0,
		-4, 0, -4, -4, 0, 4, 4, 0, -4, 4, 0, 4 };
	float normals[] = {
		0, 4, -7, 0, 4, -7, 0, 4, -7, 0, 4, 7, 0, 4, 7, 0, 4, 7,
		-7, 4, 0, -7, 4, 0, -7, 4, 0, 7, 4, 0, 7, 4, 0, 7, 4, 0,
		0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0 };
	unsigned indices[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 13, 14, 15 };

	// prepare vertex data
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// prepare normal data
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	// prepare indices array
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Shift to speed up your movement" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;

	//program.sendUniform("material", vec3(0.2f, 0.5f, 0.4f));
	//program.sendUniform("lightAmbient.color", vec3(0.1, 0.1, 0.1));

	return true;
}

void renderScene(mat4& matrixView, float time, float deltaTime)
{
	program.sendUniform("lightDir.direction", vec3(1.0, 0.5, 1.0));
	program.sendUniform("lightDir.diffuse", vec3(0.2, 0.2, 0.2));	  // dimmed white light

	mat4 m;

	// setup materials - grey
	program.sendUniform("materialDiffuse", vec3(0.6f, 0.6f, 0.6f));
	program.sendUniform("materialSpecular", vec3(0.6f, 0.6f, 1.0f));
	program.sendUniform("shininess", 10.0f);


	program.sendUniform("lightAmbient.color", vec3(0.1, 0.1, 0.1));
	program.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));

	//---------------------------------
	//render bulb 1
	m = matrixView;
	m = translate(m, vec3(-2.95f, 4.24f, -1.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	program.sendUniform("matrixModelView", m);

	program.sendUniform("materialDiffuse", vec3(1.0f, 1.0f, 1.0f));
	program.sendUniform("materialSpecular", vec3(0.0f, 0.0f, 0.0f));
	program.sendUniform("lightAmbient.color", vec3(1.0, 1.0, 1.0)); // Set emissive light
	glutSolidSphere(1, 32, 32);
	program.sendUniform("lightAmbient.color", vec3(0.1, 0.1, 0.1)); // Reset ambient light
	//---------------------------------
	//render bulb 2
	m = matrixView;
	m = translate(m, vec3(2.95f, 4.24f, -1.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	program.sendUniform("matrixModelView", m);

	program.sendUniform("materialDiffuse", vec3(1.0f, 0.0f, 0.0f));
	program.sendUniform("materialSpecular", vec3(0.0f, 0.0f, 0.0f));
	program.sendUniform("lightAmbient.color", vec3(1.0, 0.0, 0.0)); // Set emissive light
	glutSolidSphere(1, 32, 32);
	program.sendUniform("lightAmbient.color", vec3(0.1, 0.1, 0.1)); // Reset ambient light
	//---------------------------------

	// Point light setup
	program.sendUniform("lightPoint1.position", vec3(-2.95f, 4.24f, -1.0f));
	program.sendUniform("lightPoint1.diffuse", vec3(0.5f, 0.5f, 0.5f));
	program.sendUniform("lightPoint1.specular", vec3(1.0f, 1.0f, 1.0f));

	program.sendUniform("lightPoint2.position", vec3(2.95f, 4.24f, -1.0f));
	program.sendUniform("lightPoint2.diffuse", vec3(0.5f, 0.0f, 0.0f));
	program.sendUniform("lightPoint2.specular", vec3(1.0f, 1.0f, 1.0f));



	m = matrixView;
	m = translate(m, vec3(0.0f, 0, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.004f, 0.004f, 0.004f));
	table.render(0, m);

	program.sendUniform("materialDiffuse", vec3(0.9f, 0.5f, 0.3f));
	program.sendUniform("materialSpecular", vec3(0.0f, 0.0f, 0.0f));
	table.render(1, m);

	// setup materials - grey
	program.sendUniform("materialDiffuse", vec3(0.6f, 0.6f, 0.6f));
	program.sendUniform("materialSpecular", vec3(0.6f, 0.6f, 1.0f));

	m = matrixView;
	m = translate(m, vec3(0.0f, 0, 0.0f));
	m = rotate(m, radians(0.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.004f, 0.004f, 0.004f));
	table.render(0, m);

	m = matrixView;
	m = translate(m, vec3(0.0f, 0, 0.0f));
	m = rotate(m, radians(270.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.004f, 0.004f, 0.004f));
	table.render(0, m);

	m = matrixView;
	m = translate(m, vec3(0.0f, 0, 0.0f));
	m = rotate(m, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.004f, 0.004f, 0.004f));
	table.render(0, m);

	// setup materials - light green
	program.sendUniform("materialDiffuse", vec3(0.5f, 0.7f, 0.9f));
	program.sendUniform("materialSpecular", vec3(0.6f, 0.6f, 1.0f));
	m = matrixView;
	m = translate(m, vec3(0.0f, 3.04f, 0.0f));
	m = rotate(m, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	vase.render(0, m);

	// setup materials - blue
	program.sendUniform("materialDiffuse", vec3(0.2f, 0.2f, 0.8f));
	program.sendUniform("materialSpecular", vec3(0.6f, 0.6f, 1.0f));


	// teapot
	m = matrixView;
	m = translate(m, vec3(1.5f, 3.36f, 0.5f));
	m = rotate(m, radians(320.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.2f, 0.2f, 0.2f));
	// the GLUT objects require the Model View Matrix setup
	program.sendUniform("matrixModelView", m);
	glutSolidTeapot(2.0);


	// pyramid
	m = matrixView;

	program.sendUniform("materialDiffuse", vec3(0.9f, 0.1f, 0.1f));
	program.sendUniform("materialSpecular", vec3(0.6f, 0.6f, 1.0f));

	m = translate(m, vec3(-1.5f, 3.74f, 0.5f));
	m = rotate(m, radians(180.f), vec3(0.0f, 0.0f, 1.0f));

	if (pyramidRotation > 360.0f)
		pyramidRotation = 0.0f;
	else
		pyramidRotation += 0.1f;

	m = rotate(m, radians(pyramidRotation), vec3(0.0f, 1.0f, 0.0f));

	m = scale(m, vec3(0.1f, 0.1f, 0.1f));

	program.sendUniform("matrixModelView", m);

	// Get Attribute Locations
	GLuint attribVertex = program.getAttribLocation("aVertex");
	GLuint attribNormal = program.getAttribLocation("aNormal");

	// Enable vertex attribute arrays
	glEnableVertexAttribArray(attribVertex);
	glEnableVertexAttribArray(attribNormal);

	// Bind (activate) the vertex buffer and set the pointer to it
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Bind (activate) the normal buffer and set the pointer to it
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Draw triangles � using index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	// Disable arrays
	glDisableVertexAttribArray(attribVertex);
	glDisableVertexAttribArray(attribNormal);

	//program.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	program.sendUniform("materialDiffuse", vec3(0.2f, 0.5f, 0.1f));
	program.sendUniform("materialSpecular", vec3(0.6f, 0.6f, 1.0f));
	// bunny
	m = matrixView;

	m = translate(m, vec3(-1.5f, 3.55f, 0.5f));

	m = rotate(m, radians(pyramidRotation), vec3(0.0f, -1.0f, 0.0f));

	m = scale(m, vec3(4.0f, 4.0f, 4.0f));

	bunny.render(0, m);
}

void onRender()
{
	// these variables control time & animation
	static float prev = 0;
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;	// time since start in seconds
	float deltaTime = time - prev;						// time since last frame
	prev = time;										// framerate is 1/deltaTime

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	_vel = clamp(_vel + _acc * deltaTime, -vec3(maxspeed), vec3(maxspeed));
	float pitch = getPitch(matrixView);
	matrixView = rotate(translate(rotate(mat4(1),
		pitch, vec3(1, 0, 0)),	// switch the pitch off
		_vel * deltaTime),		// animate camera motion (controlled by WASD keys)
		-pitch, vec3(1, 0, 0))	// switch the pitch on
		* matrixView;

	// setup View Matrix
	program.sendUniform("matrixView", matrixView);

	// render the scene objects
	renderScene(matrixView, time, deltaTime);

	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}

// called before window opened or resized - to setup the Projection Matrix
void onReshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 matrixProjection = perspective(radians(_fov), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix
	program.sendUniform("matrixProjection", matrixProjection);
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': _acc.z = accel; break;
	case 's': _acc.z = -accel; break;
	case 'a': _acc.x = accel; break;
	case 'd': _acc.x = -accel; break;
	case 'e': _acc.y = accel; break;
	case 'q': _acc.y = -accel; break;
	}
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': _acc.z = _vel.z = 0; break;
	case 'a':
	case 'd': _acc.x = _vel.x = 0; break;
	case 'q':
	case 'e': _acc.y = _vel.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
void onMouse(int button, int state, int x, int y)
{
	glutSetCursor(state == GLUT_DOWN ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
	if (button == 1)
	{
		_fov = 60.0f;
		onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}

// handle mouse move
void onMotion(int x, int y)
{
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

	// find delta (change to) pan & pitch
	float deltaYaw = 0.005f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
	float deltaPitch = 0.005f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

	if (abs(deltaYaw) > 0.3f || abs(deltaPitch) > 0.3f)
		return;	// avoid warping side-effects

	// View = Pitch * DeltaPitch * DeltaYaw * Pitch^-1 * View;
	constexpr float maxPitch = radians(80.f);
	float pitch = getPitch(matrixView);
	float newPitch = glm::clamp(pitch + deltaPitch, -maxPitch, maxPitch);
	matrixView = rotate(rotate(rotate(mat4(1.f),
		newPitch, vec3(1.f, 0.f, 0.f)),
		deltaYaw, vec3(0.f, 1.f, 0.f)),
		-pitch, vec3(1.f, 0.f, 0.f))
		* matrixView;
}

void onMouseWheel(int button, int dir, int x, int y)
{
	_fov = glm::clamp(_fov - dir * 5.f, 5.0f, 175.f);
	onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

int main(int argc, char** argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("3DGL Scene: First Example");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		C3dglLogger::log("GLEW Error {}", (const char*)glewGetErrorString(err));
		return 0;
	}
	C3dglLogger::log("Using GLEW {}", (const char*)glewGetString(GLEW_VERSION));

	// register callbacks
	glutDisplayFunc(onRender);
	glutReshapeFunc(onReshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutMouseWheelFunc(onMouseWheel);

	C3dglLogger::log("Vendor: {}", (const char*)glGetString(GL_VENDOR));
	C3dglLogger::log("Renderer: {}", (const char*)glGetString(GL_RENDERER));
	C3dglLogger::log("Version: {}", (const char*)glGetString(GL_VERSION));
	C3dglLogger::log("");

	// init light and everything � not a GLUT or callback function!
	if (!init())
	{
		C3dglLogger::log("Application failed to initialise\r\n");
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}