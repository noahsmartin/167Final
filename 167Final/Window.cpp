//
//  Window.cpp
//  167Final
//
//  Created by Noah Martin on 12/12/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#include <iostream>
#include "Camera.h"
#include "Window.h"
#ifdef __APPLE__
#include <sys/time.h>
#include <GLUT/glut.h>
#else
#include <time.h>
#include "GLee.h"
#include <GL/glut.h>
#endif
#include <math.h>
#include "Mountain.h"
#include "main.h"
#include "SOIL.h"
#include "paths.h"

using namespace std;

int Window::width  = 512;   // set window width in pixels here
int Window::height = 512;   // set window height in pixels here

Camera camera(Vector3(0, 0, 10), Vector3(0, 0, -1), Vector3(0, 1, 0));

bool shader_enabled = true;
bool enemyDead = true;
bool box_1 = true;
bool box_2 = true;

Matrix4 model;
Matrix4 ship;
Matrix4 enemy;

bool bounding_sphere = false;
bool bezier = false;

int ship_respawn = 0;
int enemy_respawn = 0;
GLint *ints = new GLint[2];

bool keystates[256];

const int num_mountains = 10;
Mountain mountains[num_mountains];
Mountain closeRange[num_mountains];
bool genMountains = true;
bool moving = true;
bool bouncing = false;
bool firstperson = false;

void translate(Matrix4 &m, double tx, double ty, double tz)
{
	Matrix4 temp;
	temp.makeTranslate(tx, ty, tz);
	m = temp * m;
}

void rotate(Matrix4 &m, double r, double tx, double ty, double tz)
{
  Matrix4 temp;
  temp.makeRotate(r, Vector3(tx, ty, tz));
  m = m * temp;
}

void spawnShip() {
	ship.makeRotateY(90);
	Matrix4 temp;
	temp.makeScale(1, 1.0, 1);
	ship = temp * ship;
	translate(ship, -25, 8, -10);
}

void spawnEnemy() {
	enemyDead = false;
	enemy_respawn = 0;
	//enemy.makeRotateY(-90);
	//Matrix4 temp;
	//temp.makeScale(1, 1.0, 1);
	//enemy = temp * enemy;
	translate(enemy, 25, 8, -10);
}

const int max_proj = 20;
float proj_radius = 0.5;
Matrix4 projectile[max_proj];
Vector3 projectile_speeds[max_proj];
int proj_index = 0;

const int enemy_max_proj = 20;
float enemy_proj_radius = 0.5;
Matrix4 enemy_projectile[enemy_max_proj];
Vector3 enemy_projectile_speeds[enemy_max_proj];
int enemy_proj_index = 0;

const int max_asteroids = 10;
float asteroids_radius = 3;
Matrix4 asteroids[max_asteroids];
Vector3 asteroids_vel[max_asteroids];

Vector3 gravity(0, -0.002, 0);

int score = 0;

#define PI 3.14159
#define DEG_TO_RAD (PI/180.0)
#define ANG_STEP 10.0
#define NUM_DIVS 50
#define CACHE_SIZE 240

GLfloat sintheta[NUM_DIVS+1];
GLfloat costheta[NUM_DIVS+1];
GLfloat sinphi[NUM_DIVS+1];
GLfloat cosphi[NUM_DIVS+1];

void asteroid(int i) {
	float x = 75 + (rand() % 10);
	float y = (rand() % 60 + 10);
	float z = -10;
	if (rand() % 10 > 8) {
		x = 175;
		y = 100;
		z = -120 + (rand() % 30);
	}
	else if (rand() % 10 > 6) {
		z = -30 + (rand() % 30);
	}
	asteroids[i].makeTranslate(x, y, z);
	asteroids_vel[i].scale(0);
	asteroids_vel[i].setx(-(double)(rand() % 50 + 25) / 100);
	asteroids_vel[i].setz(-0.01 + (double)(rand() % 200) / 10000);
}

void updateAsteroids() {    
    for (int i = 0; i < max_asteroids; i++) {
        if(moving) {
            asteroids_vel[i] = asteroids_vel[i] + gravity;
        }
        rotate(asteroids[i], 1.0, asteroids_vel[i].x(), asteroids_vel[i].y(), asteroids_vel[i].z());
        if(moving) {
            translate(asteroids[i], asteroids_vel[i].x(), asteroids_vel[i].y(), asteroids_vel[i].z());
        }
    }
	for (int i = 0; i < max_asteroids; i++) {
		Vector3 position(asteroids[i].getPointer()[3], asteroids[i].getPointer()[7], asteroids[i].getPointer()[11]);
		for (int k = 0; k < max_proj; k++) {
			Vector3 proj(projectile[k].getPointer()[3], projectile[k].getPointer()[7], projectile[k].getPointer()[11]);
			if ((position - proj).length() < (asteroids_radius + proj_radius) && projectile_speeds[k].x() != 0) {
				if (!bouncing) { asteroid(i); }
				else { asteroids_vel[i] = projectile_speeds[k]; }
				projectile_speeds[k].scale(0);
				projectile[k].makeScale(0, 0, 0);
				score += 50;
			}
		}

		for (int k = 0; k < enemy_max_proj; k++) {
			Vector3 proj(enemy_projectile[k].getPointer()[3], enemy_projectile[k].getPointer()[7], enemy_projectile[k].getPointer()[11]);
			if ((position - proj).length() < (asteroids_radius + proj_radius) && enemy_projectile_speeds[k].x() != 0) {
				if (!bouncing) { asteroid(i); }
				else { asteroids_vel[i] = enemy_projectile_speeds[k]; }
				enemy_projectile_speeds[k].scale(0);
				enemy_projectile[k].makeScale(0, 0, 0);
			}
		}
		
		for (int k = 0; k < max_asteroids; k++) {
			if (i != k) {
				Vector3 ast(asteroids[k].getPointer()[3], asteroids[k].getPointer()[7], asteroids[k].getPointer()[11]);
				if ((position - ast).length() < (asteroids_radius * 2)) {
                    double overlap = asteroids_radius * 2 - (position - ast).length();
                    Vector3 v = position - ast;
                    v.normalize();
                    v.scale(overlap);
                    Vector3 newAst = ast - v;
                    asteroids[k].getPointer()[3] = newAst.v[0];
                    asteroids[k].getPointer()[7] = newAst.v[1];
                    asteroids[k].getPointer()[11] = newAst.v[2];
                    
                    newAst = position + v;
                    asteroids[i].getPointer()[3] = newAst.v[0];
                    asteroids[i].getPointer()[7] = newAst.v[1];
                    asteroids[i].getPointer()[11] = newAst.v[2];
                    
                    Vector3 prevI = asteroids_vel[i];
                    asteroids_vel[i] = asteroids_vel[k];
                    asteroids_vel[k] = prevI;
                    // Friction
					asteroids_vel[i].scale(0.998);
					asteroids_vel[k].scale(0.998);
				}
			}
		}

		Vector3 jet(ship.getPointer()[3], ship.getPointer()[7], ship.getPointer()[11]);
		if ((position - jet).length() < (asteroids_radius + 2.5)) {
			asteroid(i);
			ship_respawn = 10;
			score -= 100;
		}
		jet = Vector3 (enemy.getPointer()[3], enemy.getPointer()[7], enemy.getPointer()[11]);
		if ((position - jet).length() < (asteroids_radius + 3.5)) {
			asteroid(i);
			//enemy_respawn = 10;
		}

		if (asteroids[i].getPointer()[3] < -75 || asteroids[i].getPointer()[7] < -20 ||
			asteroids[i].getPointer()[3] > 200 || asteroids[i].getPointer()[7] > 200)
		{
			asteroid(i);
		}
	}
}

void shoot() {
	projectile[proj_index].identity();
	projectile[proj_index].getPointer()[3] = ship.getPointer()[3];
	projectile[proj_index].getPointer()[7] = ship.getPointer()[7];
	projectile[proj_index].getPointer()[11] = ship.getPointer()[11];
    projectile_speeds[proj_index] = Vector3(1.2, 0, 0);
    if (keystates[GLUT_KEY_LEFT]) {
        // This isn't really right, we just don't want the bullet to
        // move really slow, so pretent it is fired with more initial
        // velocity when the ship is moving backwards.
        projectile_speeds[proj_index] = projectile_speeds[proj_index] - Vector3(0.5, 0, 0);
    }
    else if (keystates[GLUT_KEY_RIGHT]) {
        projectile_speeds[proj_index] = projectile_speeds[proj_index] + Vector3(1, 0, 0);
    }
    
    if (keystates[GLUT_KEY_UP]) {
        projectile_speeds[proj_index] = projectile_speeds[proj_index] + Vector3(0, 0.2, 0);
    }
    else if (keystates[GLUT_KEY_DOWN]) {
        projectile_speeds[proj_index] = projectile_speeds[proj_index] - Vector3(0, 0.2, 0);
    }
	translate(projectile[proj_index++], 4, 0, 0);


	if (proj_index == max_proj)
	{
		proj_index = 0;
	}
}



void enemyShoot(Matrix4& enemy) {
	enemy_projectile[enemy_proj_index].identity();
	enemy_projectile[enemy_proj_index].getPointer()[3] = enemy.getPointer()[3];
	enemy_projectile[enemy_proj_index].getPointer()[7] = enemy.getPointer()[7];
	enemy_projectile[enemy_proj_index].getPointer()[11] = enemy.getPointer()[11];
	enemy_projectile_speeds[enemy_proj_index] = Vector3(-1.2, 0, 0);
	translate(enemy_projectile[enemy_proj_index++], -4, 0, 0);

	if (enemy_proj_index == enemy_max_proj)
	{
		enemy_proj_index = 0;
	}
}


const int max_particles = 10;
const float particle_radius = 1;
Matrix4 particles[max_particles];
Vector3 particles_pos[max_particles];
float particles_rad[max_particles];
float particles_dist[max_particles];

void setupParticles(int i) {
	particles_pos[i] = Vector3(0, 0, 0);
	particles_rad[i] = particle_radius;
	particles_dist[i] = (float)(rand() % 100) / 100;
}

bool loadOnce = true;

void loadOnceF() {
	model.identity();
	translate(model, 0, 0, -50);
    
    for (int i = 0; i < NUM_DIVS; i++) {
        double angle = 2.0 * M_PI * i / NUM_DIVS;
        sintheta[i] = sin(angle);
        costheta[i] = cos(angle);
    }
    
    for (int j = 0; j <= NUM_DIVS; j++) {
        double angle = M_PI/2.0 - M_PI * j / NUM_DIVS;
        sinphi[j] = sin(angle);
        cosphi[j] = cos(angle);
    }
    /* Make sure it comes to a point */
    cosphi[0] = 0;
    cosphi[NUM_DIVS] = 0;
    sintheta[NUM_DIVS] = sintheta[0];
    costheta[NUM_DIVS] = costheta[0];

	spawnShip();
	enemyDead = true;

	for (int i = 0; i < max_asteroids; i++) {
		asteroid(i);
	}

	for (int i = 0; i < max_particles; i++) {
		setupParticles(i);
	}

	loadOnce = false;
}

long timeLast;
int fps;

void Window::idleCallback()
{
    if(loadOnce) {
        loadOnceF();
    }
    long ms;
#ifdef __APPLE__
    struct timeval tp;
    gettimeofday(&tp, NULL);
    ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
#else
    SYSTEMTIME tp;
    GetLocalTime(&tp);
    ms = tp.wSecond * 1000 + tp.wMilliseconds;
#endif
    fps = (int) (1000.0/(ms - timeLast));
    timeLast = ms;

    displayCallback();         // call display routine to show the cube
  
  //Globals::shader = new Shader(BUMP_VERT_SHADER, BUMP_FRAG_SHADER);
}

void Window::reshapeCallback(int w, int h)
{
    cerr << "Window::reshapeCallback called" << endl;
    glViewport(0, 0, w, h);  // set new viewport size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double aspect = double(width)/(double)height;
    gluPerspective(60.0, aspect, 1.0, 1000); // set perspective projection viewing frustum
    //glTranslatef(0, 0, -20);    // move camera back 20 units so that it looks at the origin (or else it's in the origin)
    glMatrixMode(GL_MODELVIEW);
}


/*~~~~~~~~~~~~~~~~SHADOWS~~~~~~~~~~~~~~*/

// Expressed as float so gluPerspective division returns a float and not 0 (640/480 != 640.0/480.0).
#define SHADOW_MAP_RATIO 6


//Camera position
float p_camera[3] = { 0, 10, 50 };

//Camera lookAt
float l_camera[3] = { 0, 5, -10 };

//Light position
float p_light[3] = { 80, 800, 0 };

//Light lookAt
float l_light[3] = { 0, 0, 0 };


//Light mouvement circle radius
float light_mvnt = 50.0f;

// Hold id of the framebuffer for light POV rendering
GLuint fboId;

// Use to activate/disable shadowShader
GLhandleARB shadowShaderId;
GLuint shadowMapUniform;



// Loading shader function
GLhandleARB loadShader(char* filename, unsigned int type)
{
	FILE *pfile;
	GLhandleARB handle;
	const GLcharARB* files[1];

	// shader Compilation variable
	GLint result;				// Compilation code result
	GLint errorLoglength;
	char* errorLogText;
	GLsizei actualErrorLogLength;

	char buffer[400000];
	memset(buffer, 0, 400000);

	// This will raise a warning on MS compiler
	pfile = fopen(filename, "rb");
	if (!pfile)
	{
		printf("Sorry, can't open file: '%s'.\n", filename);
		exit(0);
	}

	fread(buffer, sizeof(char), 400000, pfile);
	//printf("%s\n",buffer);


	fclose(pfile);

	handle = glCreateShaderObjectARB(type);
	if (!handle)
	{
		//We have failed creating the vertex shader object.
		printf("Failed creating vertex shader object from file: %s.", filename);
		exit(0);
	}

	files[0] = (const GLcharARB*)buffer;
	glShaderSourceARB(
		handle, //The handle to our shader
		1, //The number of files.
		files, //An array of const char * data, which represents the source code of theshaders
		NULL);

	glCompileShaderARB(handle);

	//Compilation checking.
	glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);

	// If an error was detected.
	if (!result)
	{
		//We failed to compile.
		printf("Shader '%s' failed compilation.\n", filename);

		//Attempt to get the length of our error log.
		glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);

		//Create a buffer to read compilation error message
		errorLogText = (char *) malloc(sizeof(char) * errorLoglength);

		//Used to get the final length of the log.
		glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);

		// Display errors.
		printf("%s\n", errorLogText);

		// Free the buffer malloced earlier
		free(errorLogText);
	}

	return handle;
}

void Window::loadShadowShader()
{
	GLhandleARB vertexShaderHandle;
	GLhandleARB fragmentShaderHandle;

	vertexShaderHandle = loadShader(SHADOW_MAP_VERT_SHADER, GL_VERTEX_SHADER);
	fragmentShaderHandle = loadShader(SHADOW_MAP_FRAG_SHADER, GL_FRAGMENT_SHADER);

	shadowShaderId = glCreateProgramObjectARB();

	glAttachObjectARB(shadowShaderId, vertexShaderHandle);
	glAttachObjectARB(shadowShaderId, fragmentShaderHandle);
	glLinkProgramARB(shadowShaderId);

	shadowMapUniform = glGetUniformLocationARB(shadowShaderId, "ShadowMap");
}

void Window::generateShadowFBO()
{
	int shadowMapWidth = Window::width * SHADOW_MAP_RATIO;
	int shadowMapHeight = Window::height * SHADOW_MAP_RATIO;

	//GLfloat borderColor[4] = {0,0,0,0};

	GLenum FBOstatus;

	// Try to use a texture depth component
	glBindTexture(GL_TEXTURE_2D, Globals::textures[1]);

	// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Remove artefact on the edges of the shadowmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );



	// No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create a framebuffer object
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// attach the texture to FBO depth attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, Globals::textures[1], 0);

	// check FBO status
	FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setupMatrices(float position_x, float position_y, float position_z, float lookAt_x, float lookAt_y, float lookAt_z)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)Window::width / (double)Window::height, 10, 40000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(position_x, position_y, position_z, lookAt_x, lookAt_y, lookAt_z, 0, 1, 0);
}

void setTextureMatrix(void)
{
	static double modelView[16];
	static double projection[16];

	// This is matrix transform every coordinate x,y,z
	// x = x* 0.5 + 0.5 
	// y = y* 0.5 + 0.5 
	// z = z* 0.5 + 0.5 
	// Moving from unit cube [-1,1] to [0,1]  
	const GLdouble bias[16] = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0 };

	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);


	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);

	glLoadIdentity();
	glLoadMatrixd(bias);

	// concatating all matrice into one.
	glMultMatrixd(projection);
	glMultMatrixd(modelView);

	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
}

// During translation, we also have to maintain the GL_TEXTURE8, used in the shadow shader
// to determine if a vertex is in the shadow.
void startTranslate(float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(x, y, z);

	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glPushMatrix();
	glTranslatef(x, y, z);
}

void startModel(Matrix4 m)
{
    
    glMatrixMode(GL_MODELVIEW);
	Matrix4 modelToWorld = m;
	modelToWorld.transpose();

	glPushMatrix();
	glMultMatrixd(modelToWorld.getPointer());

	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glPushMatrix();
	glMultMatrixd(modelToWorld.getPointer());
}

void endTranslate()
{
  glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

long int time_in_ms(){
	#ifdef __APPLE__
	  struct timeval tp;
	  gettimeofday(&tp, NULL);
	  return tp.tv_sec * 1000 + tp.tv_usec / 1000;
	#else
		SYSTEMTIME tp;
		GetLocalTime(&tp);
		return tp.wSecond * 1000 + tp.wMilliseconds;
	#endif
}

void updateParticle(int i) {
	if (particles_dist[i] < 0)
	{
		setupParticles(i);
	}
	
	if (keystates[GLUT_KEY_LEFT]) {
		// This isn't really right, we just don't want the bullet to
		// move really slow, so pretent it is fired with more initial
		// velocity when the ship is moving backwards.
		if (firstperson)
		{
			particles_pos[i] = particles_pos[i] + Vector3(0, 0, 0.1);
		}
		else
		{
			particles_pos[i] = particles_pos[i] + Vector3(0.1, 0, 0);
		}
	}
	else if (keystates[GLUT_KEY_RIGHT]) {
		if (firstperson)
		{
			particles_pos[i] = particles_pos[i] - Vector3(0, 0, 0.1);
		}
		else
		{
			particles_pos[i] = particles_pos[i] - Vector3(0.1, 0, 0);
		}
	}

	if (keystates[GLUT_KEY_UP]) {
		particles_pos[i] = particles_pos[i] - Vector3(0, 0.1, 0);
	}
	else if (keystates[GLUT_KEY_DOWN]) {
		particles_pos[i] = particles_pos[i] + Vector3(0, 0.1, 0);
	}


	particles_pos[i] = particles_pos[i] + Vector3(-0.1, -0.1 + (double)(rand() % 100) / 1000 * 2, 0);

	particles[i] = ship;
	particles[i].getPointer()[3] += particles_pos[i].x();
	particles[i].getPointer()[7] += particles_pos[i].y();
	particles[i].getPointer()[11] += particles_pos[i].z();

	particles_rad[i] = particle_radius * particles_dist[i];
	particles_dist[i] -= 0.01;
}

void drawParticles() {
	for (int i = 0; i < max_particles; i++) {
		updateParticle(i);
		startModel(particles[i]);
		glColor3d(1, 0, 0);
		glutSolidSphere(particles_rad[i], 10, 10);
		endTranslate();
	}
}

void draw_ship() {
    
    if (ship_respawn > 0) {
        static bool on_off = false;
        static long int last_time = 0;
        
        if (time_in_ms() - last_time > 250) {
            on_off = !on_off;
            last_time = time_in_ms();
            ship_respawn--;
        }
        
        if (on_off) {
            return;
        }
    }

	drawParticles();
    
    startModel(ship);
    glColor3d(1, 0, 0);
    glutSolidCone(1, 5, 10, 10);
  
	if (bounding_sphere) {
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0, 0, 2.5);
		glScaled(1.1, 1.1, 1.1);
		glutWireSphere(2.5, 10, 10);
	}
  
    endTranslate();
}


Vector4 p0(25, 0, -15, 1);
Vector4 p1(25, 3, -10, 1);
Vector4 p2(25, -3, -5, 1);
Vector4 p3(25, 0, 0, 1);
Vector4 p4(25, 0, 0, 1);
Vector4 p5(25, 3, 5, 1);
Vector4 p6(25, -3, 10, 1);
Vector4 p7(25, 0, 15, 1);

void generateCurve()
{
	//p0 = Vector4()
}

long factorial(long n)
{
	long result = 1;

	//If n is 0, by definition 0! is equal to 1
	if (n <= 0)
		return result;

	//Calculate the factorial, n * n-1 * n-2 * ... * 1
	for (long i = n; i > 0; --i)
	{
		result *= i;
	}

	return result;
}

long combo(long n, long i)
{
	//C(n, i) = n! / ((n-1)! * i!)
	return factorial(n) / (factorial(n - i) * factorial(i));
}

double bernstizzlesCoeff(long n, long i, double t)
{
	//long n: is the degree of our curve, in the case of a cubic curve it is 3
	//long i: the index of the Bernstein coefficient and the control point
	//double t: is the time we are evaluating at

	//Calculate the Bernstein coefficient
	return combo(n, i) * pow(1.0 - t, n - i) * pow(t, i);
}

Vector4 Bez(double t, Vector4 p0, Vector4 p1, Vector4 p2, Vector4 p3)
{
	//Setup the control point matrix

	double n[4][4] = { p0.v[0], p1.v[0], p2.v[0], p3.v[0],
		p0.v[1], p1.v[1], p2.v[1], p3.v[1],
		p0.v[2], p1.v[2], p2.v[2], p3.v[2],
		0.0, 0.0, 0.0, 0.0 };

	Matrix4 Mp;
	for (int i = 0; i < 4; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			Mp.m[i][k] = n[i][k];
		}
	}

	//Mp.print("Mp");

	//Create a vector with our Bernstein coefficients
	Vector4 C(bernstizzlesCoeff(3, 0, t),
		bernstizzlesCoeff(3, 1, t),
		bernstizzlesCoeff(3, 2, t),
		bernstizzlesCoeff(3, 3, t));

	//Calculate the final point q
	Vector4 q = Mp * C;

	//And make sure q is a point by setting its w-component to 1
	q.v[3] = 1.0;

	return q;
}


void draw_enemy() {
	if (enemy_respawn > 0) {
		enemyDead = true;
		static bool on_off = false;
		static long int last_time = 0;

		if (time_in_ms() - last_time > 250) {
			on_off = !on_off;
			last_time = time_in_ms();
			enemy_respawn--;
		}

		if (on_off) {
			return;
		}
	}

	//translate(enemy, 0, -0.5 + (double)(rand() % 10) / 10, -0.5 + (double)(rand() % 10) / 10);
	static double i = 0;
	static double direction = 1;
	Vector4 point;
	if (i < 1)
		point = Bez(i, p0, p1, p2, p3);
	else
		point = Bez(i - 1, p4, p5, p6, p7);
	i += 0.01 * direction;
	if (i >= 2 || i <= 0)
	{
		direction *= -1;
	}
	enemy.identity();
	translate(enemy, 25, point.v[1], point.v[2]);

	startModel(enemy);
	glColor3d(1, 0, 1);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, Globals::textures[2]);
	glutSolidCube(4);
	glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
	glDisable(GL_TEXTURE_GEN_T);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

	if (bounding_sphere) {
		glMatrixMode(GL_MODELVIEW);
		glutWireSphere(3.5, 10, 10);
	}

	endTranslate();
}

// Modified from http://faculty.ycp.edu/~dbabcock/PastCourses/cs370/labs/lab22.html
// Modified from the glu source code for gluSphere() for a multi-textured unit sphere with normals
void mySphere2()
{
    int i,j;
    float sintemp1 = 0.0, sintemp2 = 0.0;
    float costemp1 = 0.0, costemp2 = 0.0;
    float x,y,z;
    float tangent[3];
    
    for (j = 0; j < NUM_DIVS; j++) {
        sintemp1 = sinphi[j];
        sintemp2 = sinphi[j+1];
        costemp1 = cosphi[j];
        costemp2 = cosphi[j+1];
        
        glBegin(GL_QUAD_STRIP);
        for (i = 0; i <= NUM_DIVS; i++) {
            // Compute coordinates
            x = costheta[i] * costemp1;
            y = sintheta[i] * costemp1;
            z = sintemp1;
            tangent[0] = -costheta[i]*sintemp1;
            tangent[1] = -sintheta[i]*sintemp1;
            tangent[2] = costemp1;
            // Set vectors
            glNormal3f(x,y,z);
            glVertexAttrib3fv(ints[1], tangent);
            glVertex3f(x,y,z);
          
          
            // Compute coordinates
            x = costheta[i] * costemp2;
            y = sintheta[i] * costemp2;
            z = sintemp2;
            tangent[0] = -costheta[i]*sintemp2;
            tangent[1] = -sintheta[i]*sintemp2;
            tangent[2] = costemp2;
            // Set vectors
            glNormal3f(x,y,z);
            glVertexAttrib3fv(ints[1], tangent);
            glVertex3f(x,y,z);
          
        }
        glEnd();
    }
}

void drawObjects(void)
{
    GLint id = glGetAttribLocationARB(shadowShaderId, "bias");
    glVertexAttrib1f(id, 0.0005);
	// Ground
	glColor4f(0.3f, 0.3f, 0.3f, 1);
	glBegin(GL_QUADS);
  glNormal3f(0, 1.0, 0);
	glVertex3f(-75, -20, -35);
	glVertex3f(-75, -20, 15);
	glVertex3f(75, -20, 15);
	glVertex3f(75, -20, -35);
	glEnd();
    glVertexAttrib1f(id, -0.000055);

	glColor4f(0.9f, 0.9f, 0.9f, 1);

	// Instead of calling glTranslatef, we need a custom function that also maintain the light matrix
	if (box_1)
	{
		startTranslate(0, 4, -16);
		glutSolidCube(4);
		endTranslate();
	}

	if (box_2)
	{
		startTranslate(-6, 4, -5);
		glutSolidCube(4);
		endTranslate();
	}

	/* gen/update mountains moved to update */
    /*Matrix4 translate;
    translate.makeTranslate(0, 10, 0);
	startModel(translate * model);
	for (int i = 0; i < num_mountains; i++) {
		mountains[i].draw();
	}
    endTranslate();
    //translate.makeTranslate(0, -20, 0);
    startModel(model);
    for (int i = 0; i < num_mountains; i++) {
        
        closeRange[i].draw();
    }
	endTranslate();
	*/
//  glMatrixMode(GL_MODELVIEW);
  
//	glUseProgramObjectARB(0);
  draw_ship();

  for (int i = 0; i < max_proj; i++) {
	  if (projectile[i].getPointer()[3] < 100)
	  {
		  startModel(projectile[i]);
		  glColor3d(0, 0, 1);
		  glutSolidSphere(proj_radius, 10, 10);
		  endTranslate();
	  }
  }


  if (!enemyDead){
	draw_enemy();
	  if (rand() % 100 > 95)
	  {
		  enemyShoot(enemy);
	  }
  }

  for (int i = 0; i < enemy_max_proj; i++) {
	  if (enemy_projectile[i].getPointer()[3] < 100)
	  {
		  startModel(enemy_projectile[i]);
		  glColor3d(1, 1, 0);
		  glutSolidSphere(enemy_proj_radius, 10, 10);
		  endTranslate();
	  }
  }
  
  if (shader_enabled) {
    Globals::shader->bind();
  }
  glActiveTexture(GL_TEXTURE7);
  int shadow_locaiton = glGetUniformLocationARB(Globals::shader->getPid(), "ShadowMap");
  glUniform1i(shadow_locaiton, 7);
  
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    int normal_location = glGetUniformLocationARB(Globals::shader->pid, "normal_texture");
    ints[1] = glGetAttribLocationARB(Globals::shader->pid, "tangent");
    glUniform1i(normal_location, 1);
    glBindTexture(GL_TEXTURE_2D, Globals::textures[2]);
    glDisable(GL_LIGHTING);
	for (int i = 0; i < max_asteroids; i++) {
		startModel(asteroids[i]);
    glMatrixMode(GL_MODELVIEW);
        glScalef(asteroids_radius, asteroids_radius, asteroids_radius);
		mySphere2();
		endTranslate();
	}
  if (shader_enabled) {
    Globals::shader->unbind();
  }
  
//    glMatrixMode(GL_TEXTURE);
  
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE);
  
}

// This update only change the position of the light. (NOT ANYMORE!)
//int elapsedTimeCounter = 0;
void update(void)
{
    
	if (keystates[GLUT_KEY_LEFT]) {
		if (firstperson)
		{
			translate(ship, 0, 0, -1);
		}
		else
		{
			translate(ship, -1, 0, 0);
		}
	}
	else if (keystates[GLUT_KEY_RIGHT]) {
		if (firstperson)
		{
			translate(ship, 0, 0, 1);
		}
		else
		{
			translate(ship, 1, 0, 0);
		}
	}

	if (keystates[GLUT_KEY_UP]) {
		translate(ship, 0, 1, 0);
	}
	else if (keystates[GLUT_KEY_DOWN]) {
		translate(ship, 0, -1, 0);
	}

	for (int i = 0; i < max_proj; i++)
	{
		translate(projectile[i], projectile_speeds[i].x(), projectile_speeds[i].y(), 0);
		projectile_speeds[i] = projectile_speeds[i] + gravity;
	}
	for (int i = 0; i < enemy_max_proj; i++)
	{
		translate(enemy_projectile[i], enemy_projectile_speeds[i].x(), enemy_projectile_speeds[i].y(), 0);
		enemy_projectile_speeds[i] = enemy_projectile_speeds[i] + gravity;
	}
    updateAsteroids();

	Vector3 position(enemy.getPointer()[3], enemy.getPointer()[7], enemy.getPointer()[11]);
	for (int k = 0; k < max_proj; k++) {
		Vector3 proj(projectile[k].getPointer()[3], projectile[k].getPointer()[7], projectile[k].getPointer()[11]);

		Vector3 box(0, 4, -16);
		if ((proj - box).length() < (3.5 + proj_radius) && projectile_speeds[k].x() != 0) {
			box_1 = false;
			spawnEnemy();

			projectile_speeds[k].scale(0);
			projectile[k].makeScale(0, 0, 0);
		}
		Vector3 box2(-6, 4, -5);
		if ((proj - box2).length() < (3.5 + proj_radius) && projectile_speeds[k].x() != 0) {
			box_2 = false;
			spawnEnemy();

			projectile_speeds[k].scale(0);
			projectile[k].makeScale(0, 0, 0);
		}

		if ((position - proj).length() < (3.5 + proj_radius) && projectile_speeds[k].x() != 0) {
			enemy_respawn = 10;
			if (!enemyDead)
				score += 100;

			projectile_speeds[k].scale(0);
			projectile[k].makeScale(0, 0, 0);
		}
	}

	position = Vector3(ship.getPointer()[3], ship.getPointer()[7], ship.getPointer()[11]);
	for (int k = 0; k < enemy_max_proj; k++) {
		Vector3 proj(enemy_projectile[k].getPointer()[3], enemy_projectile[k].getPointer()[7], enemy_projectile[k].getPointer()[11]);
		if ((position - proj).length() < (2.5 + proj_radius) && enemy_projectile_speeds[k].x() != 0) {
			ship_respawn = 10;
			score -= 100;

			enemy_projectile_speeds[k].scale(0);
			enemy_projectile[k].makeScale(0, 0, 0);
		}
	}

	if (genMountains) {
		double startY = (rand() % 100 - 50) / ((double)10);
        double startYClose = (rand() % 100 - 50) / ((double)60);
		for (int i = 0; i < num_mountains; i++) {
			mountains[i] = Mountain(4 * i - 20, 4, 2 + startY, -10, Vector3(0, 0, 0));
			mountains[i].generate();
			startY = mountains[i].endY;
            
            closeRange[i] = Mountain(4 * i - 20, 4, 2 + startYClose, 0, Vector3(0.8, 0.8, 0.8));
            closeRange[i].generate();
            startYClose = closeRange[i].endY;		}
		    genMountains = false;
	}

	for (int i = 0; i < num_mountains; i++) {
		mountains[i].translate(-0.01);
        closeRange[i].translate(-0.02);
	}
	if (mountains[0].endX <= -20) {
		for (int i = 0; i < num_mountains - 1; i++) {
			mountains[i] = mountains[i + 1];
		}
		mountains[num_mountains - 1] = Mountain(mountains[num_mountains - 2].endX, 4, 2 + mountains[num_mountains - 2].endY, -10, Vector3(0, 0, 0));
		mountains[num_mountains - 1].generate();
	}
    if (closeRange[0].endX <= -20) {
        for (int i = 0; i < num_mountains - 1; i++) {
            closeRange[i] = closeRange[i + 1];
        }
        closeRange[num_mountains - 1] = Mountain(closeRange[num_mountains - 2].endX, 4, 2 + closeRange[num_mountains - 2].endY, 0, Vector3(0.8, 0.8, 0.8));
        closeRange[num_mountains - 1].generate();
    }

	//p_light[0] = light_mvnt * cos(glutGet(GLUT_ELAPSED_TIME) / 1000.0);
	//p_light[2] = light_mvnt * sin(glutGet(GLUT_ELAPSED_TIME) / 1000.0);
	//float position[] = { p_light[0], p_light[1], p_light[2], 1.0 };
	//glLightfv(GL_LIGHT0, GL_POSITION, position);

	//p_light[0] = light_mvnt * cos(3652/1000.0);
	//p_light[2] = light_mvnt * sin(3652/1000.0);

	// fps test
	if (firstperson)
	{
		p_camera[0] = ship.getPointer()[3] - 15;
		p_camera[1] = ship.getPointer()[7] + 7.5;
		p_camera[2] = ship.getPointer()[11];

		l_camera[0] = ship.getPointer()[3] + 15;
		l_camera[1] = ship.getPointer()[7];
		l_camera[2] = ship.getPointer()[11];
	}
	else
	{
		p_camera[0] = 0;
		p_camera[1] = 10;
		p_camera[2] = 50;

		l_camera[0] = 0;
		l_camera[1] = 5;
		l_camera[2] = -10;
	}
}

void output(int x, int y, float r, float g, float b, void* font, char *string)
{
    glColor3f( r, g, b );
    glRasterPos2f(x, y);
    
    int len, i;
    len = (int)strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(font, (int)string[i]);
    }
}

/*~~~~~~~~~~~~~~~~SHADOWS~~~~~~~~~~~~~~*/

void Window::displayCallback()
{
    if(loadOnce) {
        loadOnceF();
    }
    /*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);  // make sure we're in Modelview mode
    glDisable(GL_LIGHTING);
    
    glFlush();
    glutSwapBuffers();*/

  
//  loadShadowShader();
  
	update();

	//First step: Render from the light POV to a FBO, story depth values only
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);	//Rendering offscreen

	//Using the fixed pipeline to render to the depthbuffer
	glUseProgramObjectARB(0);

	// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
	glViewport(0, 0, Window::width * SHADOW_MAP_RATIO, Window::height * SHADOW_MAP_RATIO);

	// Clear previous frame values
	glClear(GL_DEPTH_BUFFER_BIT);

	//Disable color rendering, we only want to write to the Z-Buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	setupMatrices(p_light[0], p_light[1], p_light[2], l_light[0], l_light[1], l_light[2]);

	// Culling switching, rendering only backface, this is done to avoid self-shadowing
	glCullFace(GL_FRONT);
	drawObjects();
	
	//Save modelview/projection matrice into texture7, also add a biais
	setTextureMatrix();


	// Now rendering from the camera POV, using the FBO to generate shadows
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glViewport(0, 0, Window::width, Window::height);

	//Enabling color write (previously disabled for light POV z-buffer rendering)
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Clear previous frame values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Using the shadow shader
  
  if (shader_enabled) {
    glUseProgramObjectARB(shadowShaderId);
    glUniform1iARB(shadowMapUniform, 7);
  }
  
	glActiveTextureARB(GL_TEXTURE7);
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Globals::textures[1]);



	setupMatrices(p_camera[0], p_camera[1], p_camera[2], l_camera[0], l_camera[1], l_camera[2]);

	glCullFace(GL_BACK);
	drawObjects();
    
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    if(!box_1 || !box_2) {
        glBindTexture(GL_TEXTURE_2D, Globals::textures[3]);
    } else {
        glBindTexture(GL_TEXTURE_2D, Globals::textures[0]);
    }
    glUseProgramObjectARB(0);
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    
    glColor3f(1, 1, 1);
    
    // specify texture coordinates for each vertex
    // note that textures are stored "upside down"
	double sc = 8;
	glTexCoord2f(0, 1); glVertex3f(-50 * sc, -38 * (8 - (sc - 3)), -300);
	glTexCoord2f(1, 1); glVertex3f(50 * sc, -38 * (8 - (sc - 3)), -300);
	glTexCoord2f(1, 0); glVertex3f(50 * sc, 38 * (sc - 3), -300);
	glTexCoord2f(0, 0); glVertex3f(-50 * sc, 38 * (sc - 3), -300);
    
    glEnd();
    
	/* gen/update mountains moved to update */
	Matrix4 translate;
	translate.makeTranslate(0, 5, 0);
	startModel(translate * model);
	for (int i = 0; i < num_mountains; i++) {
		mountains[i].draw();
	}
	endTranslate();
	//translate.makeTranslate(0, -20, 0);
	translate.makeTranslate(0, -10, 0);
	startModel(translate * model);
	for (int i = 0; i < num_mountains; i++) {

		closeRange[i].draw();
	}
	endTranslate();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    
    glPushMatrix();
    setupMatrices(0, 10, 50, 0, 5, -10);
    char buffer[20];
    sprintf(buffer, "Score: %d", score);
    output(-40, 30, 0, 1.0, 0.0, GLUT_BITMAP_HELVETICA_18, buffer);
    sprintf(buffer, "FPS: %d", fps);
    output(35, 30, 0, 1.0, 0.0, GLUT_BITMAP_HELVETICA_18, buffer);
    glPopMatrix();

	if (bezier)
	{
		glPushMatrix();
		for (double i = 0; i <= 1; i += 0.01)
		{
			Vector4 point = Bez(i, p0, p1, p2, p3);
			Vector4 point2 = Bez(i, p4, p5, p6, p7);
			// Draw green control points.
			glColor3f(0.0, 1.0, 0.0);
			glPointSize(5);
			glBegin(GL_POINTS);
			glVertex3d(point.v[0], point.v[1], point.v[2]);
			glVertex3d(point2.v[0], point2.v[1], point2.v[2]);
			/*glVertex3f(p0.v[0], p0.v[1], p0.v[2]);
			glVertex3f(p1.v[0], p1.v[1], p1.v[2]);
			glVertex3f(p2.v[0], p2.v[1], p2.v[2]);
			glVertex3f(p3.v[0], p3.v[1], p3.v[2]);*/
			glEnd();
		}
		glPopMatrix();
	}

	// DEBUG only. this piece of code draw the depth buffer onscreen
	/*
	glUseProgramObjectARB(0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-Window::width / 2, Window::width / 2, -Window::height / 2, Window::height / 2, 1, 20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4f(1,1,1,1);
	glActiveTextureARB(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, Globals::textures[1]);
	glEnable(GL_TEXTURE_2D);
	glTranslated(0,0,-1);
	glBegin(GL_QUADS);
	glTexCoord2d(0,0);glVertex3f(0,0,0);
	glTexCoord2d(1, 0); glVertex3f(Window::width / 2, 0, 0);
	glTexCoord2d(1, 1); glVertex3f(Window::width / 2, Window::height / 2, 0);
	glTexCoord2d(0, 1); glVertex3f(0, Window::height / 2, 0);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	*/
	
	glutSwapBuffers();
}

void Window::keyboardCallback(unsigned char key, int x, int y)
{
        if(key == 's')
        {
            if(asteroids_radius >= 5) {
                asteroids_radius -= 5;
            }

        } else if(key == 'S')
        {
            asteroids_radius += 5;
        }
        else if (key == 'b'){
          bounding_sphere = !bounding_sphere;
        }
        else if (key == 'e'){
          shader_enabled = !shader_enabled;
        }
        if(key == 'x')
        {
            translate(model, -1, 0, 0);
        } else if(key == 'X')
        {
			translate(model, 1, 0, 0);
        } else if(key == 'y')
        {
			translate(model, 0, -1, 0);
        } else if(key == 'Y')
        {
			translate(model, 0, 1, 0);
        } else if(key == 'z')
        {
			translate(model, 0, 0, -1);
        } else if(key == 'Z')
        {
			translate(model, 0, 0, 1);
        } else if(key == 'r')
        {
            asteroids_radius = 3;
        } else if(key == 'g')
		{
            genMountains = true;
		} else if (key == 32)
		{
			shoot();
        } else if(key == 'm')
		{
            moving = !moving;
		} else if (key == 't')
		{
			bouncing = !bouncing;
		}
		else if (key == 'f')
		{
			firstperson = !firstperson;
		}
		else if (key == '1')
		{
			bezier = !bezier;
		}
}

void Window::mouseFunc(int button, int state, int x, int y) {

}

void Window::motionFunc(int x, int y) {

}

void Window::specialCallback(int key, int b, int c)
{
	keystates[key] = true;
	/*if (key == GLUT_KEY_LEFT) {
		translate(ship, -1, 0, 0);
	}
	else if (key == GLUT_KEY_RIGHT) {
		translate(ship, 1, 0, 0);
	}
	else if (key == GLUT_KEY_UP) {
		translate(ship, 0, 1, 0);
	}
	else if (key == GLUT_KEY_DOWN) {
		translate(ship, 0, -1, 0);
	}*/
}

void Window::specialUpCallback(int key, int b, int c)
{
	keystates[key] = false;
}