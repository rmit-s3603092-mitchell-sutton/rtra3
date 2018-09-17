/*
 *  VBO-cube.c
 *  This program demonstrates VBOs for a cube.
 *  $Id: VBO-cube.c,v 1.4 2014/07/28 02:03:24 gl Exp gl $
 */

#define DEBUG
#define GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#define VERTICES 0
#define COLORS 2
#define INDICES 4
#define NUM_BUFFERS 5
#define SPACEBAR 32

#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#if _WIN32
#	include <Windows.h>
#	include <GL/glew.h>
#endif
#if __APPLE__
#	include <OpenGL/gl.h>
#	include <OpenGL/glu.h>
#	include <GLUT/glut.h>
#else
#	include <GL/gl.h>
#	include <GL/glu.h>
#	include <GL/glut.h>
#endif

/* *********************** Global Variables *********************** */

typedef struct
{
	bool animate;
	float t, lastT;
} Global;

typedef enum { inactive, rotate, pan, zoom } CameraControl;

struct camera_t {
	int lastX, lastY;
	float rotateX, rotateY;
	float scale;
	CameraControl control;
} camera = { 0, 0, 30.0, -30.0, 1.0, inactive };

typedef struct { float r, g, b; } color3f;
typedef struct { float x, y, z; } vec3f;//3D vector



Global g = {false, 0.0, 0.0};

const float milli = 1000.0;



bool useBufferObjects = false;

float vertices[] = {
	-1.0, -1.0, -1.0, // 0
	1.0, -1.0, -1.0, // 1
	1.0,  1.0, -1.0, // 2
	-1.0,  1.0, -1.0, // 3
	-1.0, -1.0,  1.0, // 4
	1.0, -1.0,  1.0, // 5
	1.0,  1.0,  1.0, // 6
	-1.0,  1.0,  1.0  // 7
};

float colors[] = {
	0.0, 0.0, 0.0, // 0
	1.0, 0.0, 0.0, // 1
	0.0, 1.0, 0.0, // 2
	0.0, 0.0, 1.0, // 3
	1.0, 1.0, 0.0, // 4
	1.0, 0.0, 1.0, // 5
	0.0, 1.0, 1.0, // 6
	1.0, 1.0, 1.0, // 7
};

GLuint indices2DArray[][4] = {
	{ 4, 5, 6, 7 },  // Front
	{ 0, 3, 2, 1 },  // Back
	{ 0, 4, 7, 3 },  // Left
	{ 1, 2, 6, 5 },  // Right
	{ 0, 1, 5, 4 },  // Bottom
	{ 2, 3, 7, 6 }   // Top
};

GLsizei indicesCounts[] = { 4, 4, 4, 4, 4, 4 };

const GLvoid* indicesOffsets[] = {
	(GLvoid*)(0),
	(GLvoid*)(4 * sizeof(GLuint)),
	(GLvoid*)(8 * sizeof(GLuint)),
	(GLvoid*)(12 * sizeof(GLuint)),
	(GLvoid*)(16 * sizeof(GLuint)),
	(GLvoid*)(20 * sizeof(GLuint)),
};

GLsizei numQuads = 6;

GLuint buffers[NUM_BUFFERS];

/* *********************** General functions *********************** */

void checkForGLerrors(int lineno)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
		printf("%d: %s\n", lineno, gluErrorString(error));
}

/* *********************** Vertex enables/disables *********************** */


void enableVertexArrays(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
}

void disableVertexArrays(void)
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

/* *********************** Buffer enables/disables *********************** */

void generateBuffers(void)
{
	glGenBuffers(NUM_BUFFERS, buffers);
}

void bufferData()
{
	//GL_ARRAY_BUFFER for vertex data
	/*
	 void glBufferData( GLenum target, GLsizeiptr size,
	 const GLvoid *data, GLenum usage)
	 - target again GL ARRAY BUFFER or GL ELEMENT ARRAY BUFFER
	 - size is number of bytes
	 - data is pointer to client memory or NULL
	 - usage is a hint for performance
	*/
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[COLORS]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	
	//GL_ELEMENT_ARRAY_BUFFER for index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2DArray),
				 indices2DArray, GL_STATIC_DRAW);
}

void unBindBuffers()
{
	int buffer;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
	if (buffer != 0)
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &buffer);
	if (buffer != 0)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


/* *********************** Rendering VBO *********************** */


void renderVBO()
{
	
	/*
	 void glVertexPointer(	GLint size,GLenum type,GLsizei stride,const GLvoid * pointer);
	 
	 size = num of vertex per coordinate (only 2 as we doing 2D)
	 type = Data type of each coordinate (0.0) so float
	 stride = the amount of data between coordinates. Its tightly packed so I made it 0
	 pointer = pointer to first coordinate (0);
	 */
	
	//We could use pointer to print different things using the same array!!

	glBindBuffer(GL_ARRAY_BUFFER, buffers[VERTICES]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[COLORS]);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDICES]);

	glMultiDrawElements(GL_QUADS, indicesCounts, GL_UNSIGNED_INT,
						indicesOffsets, numQuads);
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	generateBuffers();
	enableVertexArrays();
	bufferData();
	unBindBuffers();
}

void display(void)
{
#ifdef DEBUG
	
#endif
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	
	/* Oblique view, scale cube */
	glRotatef(camera.rotateX, 1.0, 0.0, 0.0);
	glRotatef(camera.rotateY, 0.0, 1.0, 0.0);

	//glRotatef(-30.0, 0.0, 1.0, 0.0);
	glScalef(0.5, 0.5, 0.5);
	

	bufferData();
	renderVBO();
	
	checkForGLerrors(__LINE__);
	
	glutSwapBuffers();
}

void idle()
{
	
	float t, dt;
	
	t = glutGet(GLUT_ELAPSED_TIME) / milli;
	
	// Accumulate time if animation enabled
	if (g.animate) {
		dt = t - g.lastT;
		g.t += dt;
		g.lastT = t;
	}
	
	glutPostRedisplay();

	
}

void reshape (int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

/* *********************** Inputs *********************** */

void mouse(int button, int state, int x, int y)
{
	
	camera.lastX = x;
	camera.lastY = y;
	
	if (state == GLUT_DOWN)
		switch(button) {
			case GLUT_LEFT_BUTTON:
				camera.control = rotate;
				break;
			case GLUT_MIDDLE_BUTTON:
				camera.control = pan;
				break;
			case GLUT_RIGHT_BUTTON:
				camera.control = zoom;
				break;
		}
	else if (state == GLUT_UP)
		camera.control = inactive;
}

void motion(int x, int y)
{
	float dx, dy;
	
	dx = x - camera.lastX;
	dy = y - camera.lastY;
	camera.lastX = x;
	camera.lastY = y;
	
	switch (camera.control) {
		case inactive:
			break;
		case rotate:
			camera.rotateX += dy;
			camera.rotateY += dx;
			break;
		case pan:
			break;
		case zoom:
			camera.scale += dy / 100.0;
			break;
	}
	
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 'l':
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glutPostRedisplay();
			break;
		case 'f':
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glutPostRedisplay();
			break;
		case 'c':
			glShadeModel(GL_FLAT);
			glutPostRedisplay();
			break;
		case 's':
			glShadeModel(GL_SMOOTH);
			glutPostRedisplay();
			break;
		case 32:
			printf("Aye got here \n");
			for (int i = 0; i<32; i++){
				//if(i%3 == 0){
					vertices[i] += 0.05;
				//}
			}
			break;
		case 27:
			exit(0);
			break;
	}
}

/* *********************** Main loop *********************** */


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(350, 350);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
