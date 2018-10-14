
#define DEBUG
#define GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#define VERTICES 0
#define COLORS 2
#define INDICES 4
#define NUM_BUFFERS 5
#define SPACEBAR 32
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define GRAV 0.91

#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "shaders.h"
#include <unistd.h>

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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
/* *********************** Global Variables *********************** */

//GIT TEST

typedef struct
{
	bool animate;
	float t, lastT, dt;
	bool levelOpenVisual;
	bool levelDevel;
	bool playing;
	bool mainMenu;
} Global;

typedef struct
{
	int x,y;
} Coord;

typedef struct
{
	float x,y;
} Pos;
typedef struct
{
	float vx,vy;
} Vel;
typedef struct
{
	Pos pos;
	Vel vel;
	bool go;
} Projectile;
typedef struct
{
	float inc;
	float deg;
} Turret;
typedef enum { inactive, rotate, pan, zoom } CameraControl;

struct camera_t {
	int lastX, lastY;
	float rotateX, rotateY;
	float scale;
	CameraControl control;
} camera = { 0, 0, 30.0, -30.0, 1.0, inactive };

typedef struct { float r, g, b; } color3f;
typedef struct { float x, y, z; } vec3f;//3D vector



Global g = {true, 0.0, 0.0, false, false, false, true};
int levels[25][225]= {{0}};
int newLevel[1][225] = {{0}};
const float milli = 1000.0;
float levelOpenTimer = 0.0;
float oneItemTimer = 0.0;
float playingTimer = 0.0;
bool isNewLevel = true;
int scrollAmount = 0;
GLfloat openLevelShader = 0;
GLfloat mainMenuShader = 0;
GLfloat playingShader = 0;
glm::mat4 modelViewMatrix;
glm::mat3 normalMatrix;
int levelCount = 0;
int playingLevel = 1;
int editingLevel = 1;
bool useBufferObjects = false;
int cursorPos[2] = {0,0};
Pos ballPos = {0.0,1.0};
Vel ballVel = {1.0,1.0};
Projectile ball1 = {ballPos, ballVel, false};
Turret turret;

/*void updateProjectile(){
	
	if(ball1.go){
		ball1.pos.x += ball1.vel.vx * g.dt;
		ball1.pos.y += g.dt * ball1.vel.vy;
		ball1.vel.vy += GRAV * g.dt;
		if(ball1.pos.y < -1){
			ball1.go = false;
		}
	}
	
}

void drawProjectile(){
	
	if(ball1.go){
		glPointSize(5.0);
		glBegin(GL_POINTS);
		glColor3f(1, 1, 1);
		glVertex3f(ball1.pos.x, ball1.pos.y, 0);
		glEnd();
	}
	
	
}*/

void renderSquare(float size, float x, float y){

	glBegin(GL_POLYGON);
	glVertex3f(x,y,0.0);
	glVertex3f(x+size/7.5,y,0.0);
	glVertex3f(x+size/7.5,y+size/7.5,0.0);
	glVertex3f(x,y+size/7.5,0.0);
	glEnd();

}


void renderCircle(float size, float x, float y){
		glBegin(GL_POLYGON);
		for(double i = 0; i < 2 * 3.142; i += 3.142 / 8) //<-- Change this Value
			glVertex3f(cos(i) * size/15.0+x, sin(i) * size/15.0+y, 0.0);
		glEnd();
}

void loadLevels(){
	bool allowed = true;
	int i = 1;
	while(allowed){
		char fileString[300];
		snprintf(fileString, sizeof fileString, "levels/level%d", i);
		strcat(fileString,".txt");
		if( access( fileString, F_OK ) != -1 ) {
		    	FILE *fp;
			fp = fopen(fileString,"r");
			int j = 0;
			int x;
			while(( x = fgetc( fp ) ) != EOF ){
				if(x==49) levels[i][j] = 1;
				if(x==50) levels[i][j] = 2;
				j++;
			}
			levelCount++; i++;
			printf("level%d\n",levelCount);
 
		} else {
			allowed = false;
		}

		

	}

}


void renderGrid(float size, float x, float y, int level){

	for(int i = 0; i < 225; i++){

		if(levels[level][i]==1){
			renderSquare(size,x+(i%15)/(7.5/size),y+floor(i/15.0)/(7.5/size));
		}
		if(levels[level][i]==2){
			renderCircle(size,x+((i%15)/(7.5/size))+(0.0666*size),y+(floor(i/15.0)/(7.5/size))+(0.0666*size));
		}
	}




}


void renderTurret(){
	
	glTranslatef(0.0, 1, 0.0);
	
	glRotatef(turret.inc, 0.0, 0.0, 1.0);
	
	glTranslatef(0.0, -1, 0.0);
	
	
	glBegin(GL_QUADS);
	glColor4f(252,252,0, 1);
	
	glVertex3f(-0.03,1,0);
	glVertex3f(0.03,1,0);
	glVertex3f(0.03,0.75,0);
	glVertex3f(-0.03,0.75,0);
	
	glEnd();
	
}

void renderTurretBase()
{
	float radius = 0.15;
	float twoPI = 2 * M_PI;
	
	glTranslatef(0.0, 1, 0.0);
	glRotatef(90, 0.0, 0.0, 1.0);
	glBegin(GL_TRIANGLE_FAN);
	
	for (float i = M_PI; i <= twoPI; i += 0.001)
		glVertex3f(+(sin(i)*radius), (cos(i)*radius), 0);
	
	glEnd();
	glRotatef(-90, 0.0, 0.0, 1.0);
	glTranslatef(0.0, -1, 0.0);
}


void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	openLevelShader = getShader("openLevel.vs","openLevel.fs");
	mainMenuShader = getShader("mainMenu.vs","mainMenu.fs");
	playingShader = getShader("playing.vs","playing.fs");
	loadLevels();
	
	modelViewMatrix = glm::mat4(1.0);
	normalMatrix = glm::mat3(1.0);
	
	modelViewMatrix = glm::rotate(modelViewMatrix, camera.rotateX * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
	modelViewMatrix = glm::rotate(modelViewMatrix, camera.rotateY * glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
	modelViewMatrix = glm::scale(modelViewMatrix, glm::vec3(camera.scale));
	
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

}


void visuallyOpenNewLevel(){
	float a = (g.t-levelOpenTimer)/3;
	renderGrid(0.85*a,-0.85*a,-0.85*a,playingLevel);
	glUseProgram(openLevelShader);
	if(!levelOpenTimer)
		levelOpenTimer = g.t;


	
	glutSolidSphere(0.8*(g.t - levelOpenTimer),200,200);


	

	if(g.t - levelOpenTimer > 3){
		levelOpenTimer = 0.0;
		g.levelOpenVisual = false;
		g.playing = true;
		playingTimer = 0.0;
	}
	glUseProgram(0);
	
}
void idle()
{
	
	float t, dt;
	
	t = glutGet(GLUT_ELAPSED_TIME) / milli;
	
	// Accumulate time if animation enabled
	if (g.animate) {
		dt = t - g.lastT;
		g.t += dt;
		oneItemTimer += dt;
		playingTimer += dt;

		g.lastT = t;
	}
	
	glutPostRedisplay();

	
}
void shaderDisplayOpenVisual(){
	glUseProgram(openLevelShader);
	
	GLfloat normalMat = glGetUniformLocation(openLevelShader, "normalMatrix");
	GLfloat modelMat = glGetUniformLocation(openLevelShader, "modelViewMatrix");
	GLfloat theG = glGetUniformLocation(openLevelShader, "time");
	glUniformMatrix3fv(normalMat,1,false,&normalMatrix[0][0]);
	glUniformMatrix4fv(modelMat,1,false,&modelViewMatrix[0][0]);
	glUniform1f(theG,g.t);
	glUseProgram(0);
	visuallyOpenNewLevel();

}
void renderGridLines(){
	
	glBegin(GL_LINES);
	for(int i = 0; i <= 15; i++){
		glVertex2f(-0.85,i/(7.5/0.85)-0.85);
		glVertex2f(0.85,i/(7.5/0.85)-0.85);
		glVertex2f(i/(7.5/0.85)-0.85,-0.85);
		glVertex2f(i/(7.5/0.85)-0.85,0.85);

	}
	glEnd();

}


void levelDevelHighLight(){
	float x = cursorPos[0];
	float y = cursorPos[1];
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glLineWidth(3);
	glColor3f(1.0,0.0,0.0);
	glLineWidth(1.0);

	glBegin(GL_POLYGON);

	glVertex3f(x/(7.5/0.85)-0.85+0.01,y/(7.5/0.85)-0.85+0.01,-0.01);
	glVertex3f((x+1)/(7.5/0.85)-0.85-0.01, y/(7.5/0.85)-0.85+0.01,-0.01);
	glVertex3f((x+1)/(7.5/0.85)-0.85-0.01, (y+1)/(7.5/0.85)-0.85-0.01,-0.01);
	glVertex3f(x/(7.5/0.85)-0.85+0.01, (y+1)/(7.5/0.85)-0.85-0.01,-0.01);

	glEnd();


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1.0,1.0,1.0);
}

void levelDeveloperDisplay(){
	
	renderGridLines();
	renderGrid(0.85,-0.85,-0.85,editingLevel);
	levelDevelHighLight();
}

void reflectBall(float x1,float x2,float y1,float y2){
	
	float angle = atan((y1-y2)/(x1-x2));
	ball1.vel.vx -= 2*cos(angle)*ball1.vel.vx;
	ball1.vel.vy += 2*sin(angle)*ball1.vel.vy;

}

bool calcDistConfirmCollision(float size,float x1, float x2, float y1, float y2){
	if(sqrt((x1-x2)*(x1-x2)
		       +
		(y1-y2)*(y1-y2))
		       < 
		    size/7.5){
		reflectBall(x1,x2,y1,y2);
		return true;
	}
	return false;
}

bool detectSquareColl(int x, int y, float size){

	float minX = -0.85+((float)(x))/(7.5/size);
	float minY = -0.85+((float)(y))/(7.5/size);
	float maxX = -0.85+((float)(x)+1.0)/(7.5/size);
	float maxY = -0.85+((float)(y)+1.0)/(7.5/size);
	float midX = -0.85+((float)(x))/(7.5/size)+0.0666*size;
	float midY = -0.85+((float)(y))/(7.5/size)+0.0666*size;
	float balX = ball1.pos.x;
	float balY = ball1.pos.y;
	if(balX > maxX){
		midX = maxX;
	} else if( balX < minX){
		midX = minX;
	}
	else{
		midX = balX;
	}
	if(balY > maxY){
		midY = maxY;
	}else if( balY < minY){
		midY = minY;
	}else{
		midY = balY;
	}
	return calcDistConfirmCollision(size/2.0,midX,balX,midY,balY);
}

bool detectCircleColl(int x, int y, float size){

	float midX = -0.85+((float)(x))/(7.5/size)+0.0666*size;
	float midY = -0.85+((float)(y))/(7.5/size)+0.0666*size;
	
	return calcDistConfirmCollision(size,midX,ball1.pos.x,midY,ball1.pos.y);


}

Coord detBallPosition(){

	int x = floor((ball1.pos.x)*7.5/0.85+7+0.15);
	int y = floor((ball1.pos.y)*7.5/0.85+7+0.15);
	Coord ball = {x,y};
	return ball;

}

Coord detectCollision(){

	Coord ball = detBallPosition();
	Coord coll = {-1,-1};
	for(int x = ball.x-1; x <= ball.x + 1; x++){
		for(int y = ball.y-1; y <= ball.y +1; y++){
			if(x>=0&&x<15&&y>=0&&y<15){
				if(levels[playingLevel][y*15+x]==1)
					if(detectSquareColl(x,y,0.85)){
						coll = {x,y};
						break;
					}
				if(levels[playingLevel][y*15+x]==2){
					if(detectCircleColl(x,y,0.85)){
						coll = {x,y};
						break;
					}
				
				}
					
			}
		}
	}
	if(coll.x >= 0)
		levels[playingLevel][coll.y*15+coll.x] = 0;
	
	
}


void renderLevelIcons(){

	glColor3f(1.0,1.0,1.0);

	for(int i = 1; i < levelCount; i++){
		glLineWidth(3);
		if(-1.5+i-scrollAmount == -0.5) i++;
		renderGrid(0.5,(-1.5+i-scrollAmount),-0.5,i);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_POLYGON);

		glVertex2f(-1.5+i-scrollAmount,-0.5);
		glVertex2f(-1.5+i-scrollAmount,0.5);
		glVertex2f(-0.5+i-scrollAmount,0.5);
		glVertex2f(-0.5+i-scrollAmount,-0.5);

		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void menuHighlight(){
		glUseProgram(mainMenuShader);
		GLfloat normalMatmen = glGetUniformLocation(mainMenuShader, "normalMatrix");
		GLfloat modelMatmen = glGetUniformLocation(mainMenuShader, "modelViewMatrix");
		GLfloat theG = glGetUniformLocation(mainMenuShader, "time");
		glUniformMatrix3fv(normalMatmen,1,false,&normalMatrix[0][0]);
		glUniformMatrix4fv(modelMatmen,1,false,&modelViewMatrix[0][0]);
		glUniform1f(theG,oneItemTimer);
		glColor3f(1.0,0.0,0.0);
		renderGrid(0.5,-0.5,-0.5,1+scrollAmount);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_POLYGON);

		glVertex2f(-0.5,-0.5);
		glVertex2f(-0.5,0.5);
		glVertex2f(0.5,0.5);
		glVertex2f(0.5,-0.5);
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(1.0,1.0,1.0);
		glUseProgram(0);

}



void renderButtons(){


}

void displayMainMenu(){
	menuHighlight();
	renderLevelIcons();
	renderButtons();
	
}

void saveLevel(){

	int testForUsedLevel = 0;
	for(int i = 0; i < 225; ++i){
		testForUsedLevel += levels[editingLevel][i];
	}
	if(!testForUsedLevel) return;

	FILE *fp;
	char fileString[300];
	snprintf(fileString, sizeof fileString, "levels/level%d", editingLevel);
	strcat(fileString,".txt");
	fp = fopen(fileString,"wb");
	for(int i = 0; i < 225; ++i){
		int curr = levels[editingLevel][i];
		fprintf(fp, "%d",curr);
	}
	fclose(fp);
	if(editingLevel==levelCount)isNewLevel = true;
	g.levelDevel = false;



}

void renderShadedGridLevel(){
		glUseProgram(playingShader);
		GLfloat theG = glGetUniformLocation(playingShader, "time");
		glUniform1f(theG,playingTimer);
		
		renderGrid(0.85,-0.85,-0.85,playingLevel);
		glUseProgram(0);
}	
void renderBall(){
	
	renderCircle(0.85,ball1.pos.x,ball1.pos.y);

	
}
void renderPlayfield(){

		renderShadedGridLevel();
		renderTurretBase();
		renderTurret();
		if(ball1.go){
			//updateProjectile();
		}
		detectCollision();
		renderBall();

}
void display(void)
{
#ifdef DEBUG
	
#endif
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	
	/* Oblique view, scale cube */
		
	if(g.levelDevel)
		levelDeveloperDisplay();
	else if(g.playing)
		renderPlayfield();
	else if(g.levelOpenVisual)
		shaderDisplayOpenVisual();
	else if(g.mainMenu)
		displayMainMenu();
	
	/*
	glRotatef(camera.rotateX, 1.0, 0.0, 0.0);
	glRotatef(camera.rotateY, 0.0, 1.0, 0.0);

	glRotatef(-30.0, 0.0, 1.0, 0.0);
	glScalef(0.5, 0.5, 0.5);
		bufferData();
		renderVBO();
	*/
	
	glutSwapBuffers();
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
void moveBallToMouse(int x, int y){

	ball1.pos.x = -1+((float)(x)/450.0);
	ball1.pos.y = 1+((float)(y)/(-450.0));
	glutPostRedisplay();
}

void SpecialInput(int key, int x, int y){
	switch(key){

		case GLUT_KEY_UP:
			if(cursorPos[1] + 1 < 15) cursorPos[1]++;
			break;	
		case GLUT_KEY_DOWN:
			if(cursorPos[1] - 1 >= 0) cursorPos[1]--;
			break;
		case GLUT_KEY_LEFT:
			if(cursorPos[0] - 1 >= 0&&g.levelDevel) cursorPos[0]--;
			if(g.mainMenu&&scrollAmount-1>=0) scrollAmount-=1;
			oneItemTimer=0;
			break;
		case GLUT_KEY_RIGHT:
			if(cursorPos[0] + 1 < 15&&g.levelDevel) cursorPos[0]++;
			if(g.mainMenu&&scrollAmount+1<levelCount) scrollAmount+=1;
			oneItemTimer=0;
			
			break;
		default:
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
		case 'a':
			g.animate = !g.animate;
			break;
		case 'm':
			ball1.go = !ball1.go;
			break;
		case KEY_LEFT:
			if(turret.inc<90){
				turret.inc += 2;
			}
			break;
		case KEY_RIGHT:
			if(turret.inc>-90){
				turret.inc -= 2;
			}
			break;
		case 'x':
			if(turret.inc<90){
				turret.inc += 2;
			}
			break;
		case 'z':
			if(turret.inc>-90){
				turret.inc -= 2;
			}
			break;
		case 27:
			g.playing = false;
			
			g.levelDevel = false;

			g.levelOpenVisual = false;
				
			oneItemTimer=0;
			g.mainMenu = true;
			break;
		case 'p':
			if(isNewLevel) {
				levelCount++;
				editingLevel=levelCount;
				isNewLevel = false;
			}
			g.levelDevel = !g.levelDevel;
			break;
		case 'w':
			if(g.levelDevel) saveLevel();
			g.playing = false;
			g.levelOpenVisual = false;
			g.mainMenu = true;
			break;
		case 32:
			if(g.levelDevel){
				int pos = cursorPos[1]*15+cursorPos[0];
				levels[editingLevel][pos]++;
				if(levels[editingLevel][pos] == 3)
					levels[editingLevel][pos] = 0;
			}
			if(g.mainMenu) {
				playingLevel = scrollAmount+1;
				g.mainMenu = false;
				g.levelOpenVisual = true;
			}
			break;
		case 91:
			if(editingLevel-1>0) editingLevel--;
			break;
		case 93:
			if(editingLevel+1<=levelCount) editingLevel++;
			break;
	}
}

/* *********************** Main loop *********************** */


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(900, 900);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(moveBallToMouse);
	glutSpecialFunc(SpecialInput);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
