/*********
ERAY ALTAY
----------
PROBLEMS: Sometimes it cannot destroy every car. However I am sure that testcollision function is true. 
If you hold spacebar it counts wrong.
However, if you press spacebar every second once, it counts right.
----------
FEATURES:

Random Colors for each cars
Start or pause with F1 Key
20 seconds time countdown and limit
Counting every hitted car and display
10 fires at a time
8 frames after one fire
Adding boundaries for canon gun

*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  20 // Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer

#define LIVE	3
#define PAUSE	4

#define D2R 0.0174532
#define TARGET_RADIUS 75

//PHASE
#define START 0
#define RUN 1
#define ENDGAME 2

#define DURATION 2000
#define MAX 6

int cnt = 0, cnt2 = 0;

typedef struct point_t{
	float x, y;
} point_t;

typedef struct color_t{
	int r, g, b;
} color_t;

typedef struct formula_t{
	point_t pos = { -400,0 };
	color_t color;
	float radius;
	float trad;
	float speed;
	bool active;
} formula_t;

typedef struct player_t{
	point_t pos;
}player_t;

typedef struct fire_t{
	point_t pos;
	bool active;
} fire_t;

#define MAX_FIRE 10 // 10 fires at a time.
#define FIRE_RATE 8 // after 8 frames you can throw another one.
#define CAR_RATE 4

player_t p = { 0,-300 };
fire_t fr[MAX_FIRE];
formula_t f1cars[MAX];

int fire_rate = 0;
int car_rate = 0;

int count = 0;
int lvl = 0;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false;
int  winWidth, winHeight; // current Window width and height

int appState = ENDGAME;
double timeCounter = DURATION;
bool activeTimer = false;
bool isFinished = false;

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}


void drawf1(formula_t f) {

	//BACK
	glColor3ub(31, 0, 0);
	glRectf(f.pos.x, f.pos.y, f.pos.x + 5, f.pos.y + 18);

	//COCKPIT
	glColor3ub(224, 255, 255);
	circle(f.pos.x + 21, f.pos.y + 9, f.radius - 2);

	//FRONT
	//glColor3ub(146, 0, 10);
	glColor3ub(f.color.r, f.color.g, f.color.b);
	glBegin(GL_TRIANGLES);
	glVertex2f(f.pos.x + 5, f.pos.y);
	glVertex2f(f.pos.x + 5, f.pos.y + 18);
	glVertex2f(f.pos.x + 53, f.pos.y);
	glEnd();

	//BACK TIRE
	glColor3ub(0, 0, 0);
	circle(f.pos.x + 9, f.pos.y, f.radius);

	//FRONT TIRE
	glColor3ub(0, 0, 0);
	circle(f.pos.x + 33, f.pos.y, f.radius);

	//BACK RIM
	glColor3ub(255, 255, 255);
	circle(f.pos.x + 9, f.pos.y, f.radius - 4);

	//FRONT RIM
	glColor3ub(255, 255, 255);
	circle(f.pos.x + 33, f.pos.y, f.radius - 4);

	//OUTER NOS
	glColor3ub(30, 144, 255);
	glBegin(GL_TRIANGLES);
	glVertex2f(f.pos.x, f.pos.y + 1);
	glVertex2f(f.pos.x, f.pos.y + 13);
	glVertex2f(f.pos.x - 25, f.pos.y + 6);
	glEnd();

	//INNER NOS
	glColor3ub(224, 255, 255);
	glBegin(GL_TRIANGLES);
	glVertex2f(f.pos.x, f.pos.y + 4);
	glVertex2f(f.pos.x, f.pos.y + 10);
	glVertex2f(f.pos.x - 18, f.pos.y + 6);
	glEnd();

}

void drawAllFormula() {

	for (int i = 0; i < MAX; i++) {
		if(f1cars[i].active)
			drawf1(f1cars[i]);
	}
}

void drawPlayer(player_t tp) {

	//BASE
	glColor3f(1, 1, 1);
	glRectf(tp.pos.x, tp.pos.y, tp.pos.x + 70, tp.pos.y + 10);

	glColor3f(1, 1, 1);
	glRectf(tp.pos.x, tp.pos.y, tp.pos.x + 10, tp.pos.y + 30);

	glColor3f(1, 1, 1);
	glRectf(tp.pos.x+60, tp.pos.y, tp.pos.x + 70, tp.pos.y + 30);

}

void drawFires() {
	for (int i = 0; i < MAX_FIRE; i++) {
		if (fr[i].active) {
			glColor3f(1, 0, 0);
			circle(fr[i].pos.x+5, fr[i].pos.y+30, 5);

			glColor3f(1, 0, 0);
			circle(fr[i].pos.x+65, fr[i].pos.y+30, 5);
		}
	}
}

void displayMessage() {

	glColor3ub(255, 255, 255);
	vprint2(-60, 255, 0.09, "Press F1 to Start.");

}

void displayLvl() {

	glColor3ub(255, 255, 255);
	vprint2(-70, 255, 0.09, "You have destroyed %d cars", lvl);

}

void displayRun() {
	glColor3ub(0, 255, 0);
	vprint2(225, 255, 0.10, "Time Left %0.2f", timeCounter / 100);
}


void background() {

	//TARMAC 
	glColor3ub(40, 43, 42);
	glRectf(-400, 300, 400, -300);

	//GRASS TOP
	glColor3ub(53, 98, 6);
	glRectf(-400, 300, 400, 225);

	//GRASS BOTTOM
	glColor3ub(53, 98, 6);
	glRectf(-400, -300, 400, -225);

	/*
	//ID CARD
	glColor3ub(243, 184, 0);
	glRectf(-50, 275, 50, 225);

	glColor3ub(2, 28, 18);
	vprint2(-37, 260, 0.09, "ERAY ALTAY");

	glColor3ub(2, 28, 18);
	vprint2(-27, 240, 0.09, "21803051");*/


	//SEPERATORS
	glColor3ub(170, 170, 170);
	glBegin(GL_LINES);

	glVertex2f(-400, 225);
	glVertex2f(400, 225);

	glVertex2f(-400, 112.5);
	glVertex2f(400, 112.5);

	glVertex2f(-400, 0);
	glVertex2f(400, 0);

	glVertex2f(-400, -112.5);
	glVertex2f(400, -112.5);

	glVertex2f(-400, -225);
	glVertex2f(400, -225);

	glEnd();

	//TARMAC LINES

	glColor3ub(170, 170, 170);
	glRectf(-300, 176, -225, 161);

	glColor3ub(170, 170, 170);
	glRectf(-300, 64, -225, 49);

	glColor3ub(170, 170, 170);
	glRectf(-300, -48, -225, -63);

	glColor3ub(170, 170, 170);
	glRectf(-300, -160, -225, -175);

	//TARMAC LINE 2
	glColor3ub(170, 170, 170);
	glRectf(-125, 176, -50, 161);

	glColor3ub(170, 170, 170);
	glRectf(-125, 64, -50, 49);

	glColor3ub(170, 170, 170);
	glRectf(-125, -48, -50, -63);

	glColor3ub(170, 170, 170);
	glRectf(-125, -160, -50, -175);

	//TARMAC LINE 3
	glColor3ub(170, 170, 170);
	glRectf(50, 176, 125, 161);

	glColor3ub(170, 170, 170);
	glRectf(50, 64, 125, 49);

	glColor3ub(170, 170, 170);
	glRectf(50, -48, 125, -63);

	glColor3ub(170, 170, 170);
	glRectf(50, -160, 125, -175);

	//TARMAC LINE 4
	glColor3ub(170, 170, 170);
	glRectf(225, 176, 300, 161);

	glColor3ub(170, 170, 170);
	glRectf(225, 64, 300, 49);

	glColor3ub(170, 170, 170);
	glRectf(225, -48, 300, -63);

	glColor3ub(170, 170, 170);
	glRectf(225, -160, 300, -175);

	//GRID LINE
	glColor3ub(243, 184, 0);
	glRectf(-385, 225, -400, -225);

	//FINISH LINE

	glColor3ub(255, 255, 255);
	glRectf(385, 225, 400, -225);


}


//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	switch (appState) {

	case ENDGAME:
		background();
		drawAllFormula();
		drawPlayer(p);
		drawFires();
		displayMessage();
		

		if (activeTimer = activeTimer)
			activeTimer = !activeTimer;

		else
			activeTimer = activeTimer;

		break;

	case RUN:
		background();
		drawAllFormula();
		displayRun();
		drawPlayer(p);
		drawFires();
		displayLvl();

		if (activeTimer = !activeTimer)
			activeTimer = activeTimer;


		break;
	}

	glutSwapBuffers();
}


int findAvailableFire() {
	for (int i = 0; i < MAX_FIRE; i++) {
		if (fr[i].active == false) 
			return i;
	}
	return -1;
}

int findAvailableCar() {
	for (int i = 0; i < MAX; i++) {
		if (f1cars[i].active == false) 
			return i;
	}
	return -1;
}

void movePlayerRight(player_t* tp, float speed) {

	// check rectangular boundary
	if (tp->pos.x <= 310) {

		tp->pos.x += 3;

	}

}

void movePlayerLeft(player_t* tp, float speed) {

	// check rectangular boundary
	if (tp->pos.x >= -380){
		tp->pos.x -= 3;
	}

}

bool testCollision(fire_t fr, formula_t f1cars) {
	
	float dx = f1cars.pos.x - fr.pos.x;
	float dy = f1cars.pos.y - fr.pos.y;
	float d = sqrt(dx * dx + dy * dy);
		
	return d <= f1cars.trad;

}

#if TIMER_ON == 1
void onTimer(int v) {

	//glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	if (appState == RUN) {
		timeCounter -= 2;
		if (timeCounter > 0) {
			glutTimerFunc(TIMER_PERIOD, onTimer, 0);
		}
		else {
			// time expires
			appState = ENDGAME;
		}
	}

	if (right) {
		movePlayerRight(&p, 3);
	}
	if (left) {
		movePlayerLeft(&p, -3);
	}

	if (spacebar && fire_rate == 0) {
		int availFire = findAvailableFire();
		if (availFire != -1) {
			fr[availFire].pos = p.pos;
			fr[availFire].active = true;
			fire_rate = FIRE_RATE;
		}
	}

	if (fire_rate > 0) 
		fire_rate--;

	// Move all fires that are active.
	for (int i = 0; i < MAX_FIRE; i++) {
		if (fr[i].active) {
			fr[i].pos.y += 10;


			if (fr[i].pos.x > 400 || fr[i].pos.x < -400 || fr[i].pos.y>300 || fr[i].pos.y < -300) {
				fr[i].active = false;
			}

			if (testCollision(fr[i], f1cars[i])) {
				f1cars[i].active = false;
				fr[i].active = false;
				lvl++;
			}
		}
	}

	if (car_rate == 0) {
		int availCar = findAvailableCar();
		if (availCar != -1) {
			f1cars[availCar].pos.x = rand() % (-400 + 450 + 1) - 450;
			f1cars[availCar].pos.y = rand() % (225 + 225 + 1) - 225;
			f1cars[availCar].color.r = rand() % 256;
			f1cars[availCar].color.g = rand() % 256;
			f1cars[availCar].color.b = rand() % 256;
			f1cars[availCar].radius = 7;
			f1cars[availCar].trad = TARGET_RADIUS;
			f1cars[availCar].active = true;
			car_rate = CAR_RATE;
		}
	}

	if (car_rate > 0)
		car_rate--;

	// Move all Cars that are active.
	for (int j = 0; j < MAX; j++) {
		if (f1cars[j].active) {
			f1cars[j].pos.x += 10;

			if (f1cars[j].pos.x > 400 || f1cars[j].pos.x < -400 || f1cars[j].pos.y>300 || f1cars[j].pos.y < -300) {
				f1cars[j].active = false;
			}

		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif


//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		spacebar = true;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		spacebar = false;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.

	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}

	if (key == GLUT_KEY_F1 && appState == ENDGAME) {
		appState = RUN;
		timeCounter = DURATION;
		glutTimerFunc(TIMER_PERIOD, onTimer, 0);

		for (int i = 0; i < MAX; i++) {
			f1cars[i].active = true;
		}
	}

	else if (key == GLUT_KEY_F1 && appState == RUN) {
		appState = ENDGAME;
	}

	/*if (key == GLUT_KEY_F1 && appState == ENDGAME) {
		appState = RUN;
		for (int i = 0; i < MAX; i++) {
			f1cars[i].active = true;
		}
	}

	else if (key == GLUT_KEY_F1 && appState == RUN) {
		if (isFinished) {
			appState = ENDGAME;
			isFinished = false;
		}
	}*/

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.

	/*
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && count < MAX && appState == CREATE) {
		f1cars[count].pos = { x - winWidth / 2.0 , winHeight / 2.0 - y };
		f1cars[count].r = rand() % 256;
		f1cars[count].g = rand() % 256;
		f1cars[count].b = rand() % 256;
		f1cars[count].radius = 7;
		count++;
	}

	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && appState == ACTION) {
		count = 0;
	}*/


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.

	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.

	// to refresh the window it calls display() function
	glutPostRedisplay();
}


void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Eray Altay Grand Prix Shooter");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}