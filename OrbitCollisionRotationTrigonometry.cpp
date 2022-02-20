/*********
ERAY ALTAY
----------
PROBLEMS:

I could control player with arrow keys, not with mouse movement. However, shooting is still left click.

First target positins are at 0,0 and you have to shoot them first for entering action phase.

----------
FEATURES:

You have to start game with pressing F1 key, also that key restarts countdown and pauses action.

Arrow keys are the control.

Targets are triangle, they can rotate randomly around themselves and around the player.

20 seconds countdown can be increased by hitting stars. Each star adds +2 seconds.

Scores and how many second you have added can be seen in the bottom right of the screen.

Player object has rotating triangle.

Throwing object is triangle, also it is rotating.

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

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define TARGET_RADIUS 20

#define RUN 0
#define ENDGAME 1
#define DURATION 2000


/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, shot = false;
int  winWidth, winHeight; // current Window width and height

bool activeTimer = false;
double timeCounter = DURATION;

int appState = ENDGAME;

float A = 200, A2=250, A3=300, //amplitude
fq = 1,  //frequency
C = 0,   //horizontal phase shift
B = 0;   //vertical phase shift

int scorestreak = 0;

typedef struct point_t{
	double x, y;
} point_t;

typedef struct color_t{
	int r, g, b;
} color_t;

typedef struct player_t{
	point_t pos;    // position of the object
	float   angle;  // view angle 
	float   r;
} player_t;

typedef struct fire_t{
	point_t pos;
	float angle;
	float startangle = 10;
	bool active;
} fire_t;

typedef struct entity_t{
	point_t pos;
	double angle;
	float startangle; //start angle
	unsigned char r, g, b;
	bool hit;      // collision state
	bool active;
	double radius = 20;
	double speed1; //on its self
	double speed2; //outer road
} entity_t;

typedef struct circlem_t{
	point_t posm;
	float anglem;
	float rm;//medium circle
}circlem_t;

#define MAX 1
#define MAX_FIRE 1 // 10 fires at a time.
#define FIRE_RATE 4 // after 8 frames you can throw another one.

int ammo = MAX_FIRE;

player_t p = { {0,0}, 45, 24 };
point_t mouse;

fire_t fr[MAX_FIRE];
int fire_rate = 0;

entity_t i1;
entity_t i2;
entity_t i3;

int count = 0;


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

void vertex(point_t P, point_t Tr, double angle) {


	double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
	double yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
	glVertex2d(xp, yp);


}

float f(float x) {
	return A * sin((fq * x + C) * D2R) + B;
}

void drawIcon1(entity_t e) {

	double r = fabs(A);
	//double r2 = fabs(A2);//amplitude
	//double r3 = fabs(A3);


	glColor3ub(e.r, e.g, e.b);
	glBegin(GL_TRIANGLES);
	vertex({ 0, 20 }, {  r * cos(e.startangle * D2R), r * sin(e.startangle * D2R) }, e.angle);
	vertex({ -17.3, -10 }, { r * cos(e.startangle * D2R), r * sin(e.startangle * D2R) }, e.angle);
	vertex({ 17.3, -10 }, { r * cos(e.startangle * D2R), r * sin(e.startangle * D2R) }, e.angle);
	glEnd();
	glColor3ub(105, 105, 105);
	glLineWidth(0.5);
	glBegin(GL_LINE_LOOP);
	vertex({ 0, 20 }, { r * cos(e.startangle * D2R), r * sin(e.startangle * D2R) }, e.angle);
	vertex({ -17.3, -10 }, { r * cos(e.startangle * D2R), r * sin(e.startangle * D2R) }, e.angle);
	vertex({ 17.3, -10 }, { r * cos(e.startangle * D2R), r * sin(e.startangle * D2R) }, e.angle);
	glEnd();
	glLineWidth(1);

}

void drawIcon2(entity_t e) {

	//double r = fabs(A);
	double r2 = fabs(A2);//amplitude
	//double r3 = fabs(A3);


	glColor3ub(e.r, e.g, e.b);
	glBegin(GL_TRIANGLES);
	vertex({ 0, 20 }, { r2 * cos(e.startangle * D2R), r2 * sin(e.startangle * D2R) }, e.angle);
	vertex({ -17.3, -10 }, { r2 * cos(e.startangle * D2R), r2 * sin(e.startangle * D2R) }, e.angle);
	vertex({ 17.3, -10 }, { r2 * cos(e.startangle * D2R), r2 * sin(e.startangle * D2R) }, e.angle);
	glEnd();
	glColor3ub(105, 105, 105);
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
	vertex({ 0, 20 }, { r2 * cos(e.startangle * D2R), r2 * sin(e.startangle * D2R) }, e.angle);
	vertex({ -17.3, -10 }, { r2 * cos(e.startangle * D2R), r2 * sin(e.startangle * D2R) }, e.angle);
	vertex({ 17.3, -10 }, { r2 * cos(e.startangle * D2R), r2 * sin(e.startangle * D2R) }, e.angle);
	glEnd();
	glLineWidth(1);


}

void drawIcon3(entity_t e) {

	//double r = fabs(A);
	//double r2 = fabs(A2);//amplitude
	double r3 = fabs(A3);


	glColor3ub(e.r, e.g, e.b);
	glBegin(GL_TRIANGLES);
	vertex({ 0, 20 }, { r3 * cos(e.startangle * D2R), r3 * sin(e.startangle * D2R) }, e.angle);
	vertex({ -17.3, -10 }, { r3 * cos(e.startangle * D2R), r3 * sin(e.startangle * D2R) }, e.angle);
	vertex({ 17.3, -10 }, { r3 * cos(e.startangle * D2R), r3 * sin(e.startangle * D2R) }, e.angle);
	glEnd();

	glColor3ub(105, 105, 105);
	glLineWidth(1.5);
	glBegin(GL_LINE_LOOP);
	vertex({ 0, 20 }, { r3 * cos(e.startangle * D2R), r3 * sin(e.startangle * D2R) }, e.angle);
	vertex({ -17.3, -10 }, { r3 * cos(e.startangle * D2R), r3 * sin(e.startangle * D2R) }, e.angle);
	vertex({ 17.3, -10 }, { r3 * cos(e.startangle * D2R), r3 * sin(e.startangle * D2R) }, e.angle);
	glEnd();
	glLineWidth(1);


}


void drawCircle() {
	
	float r = fabs(A);
	float r2 = fabs(A2);//amplitude
	float r3 = fabs(A3);

	glColor3ub(238, 130, 238);
	circle_wire(0, 0, r); 

	glColor3ub(148, 0, 211);
	circle_wire(0, 0, r2);

	glColor3ub(75, 0, 130);
	circle_wire(0, 0, r3); 

}


void drawPlayer(player_t tp) {

	double angle = p.angle * D2R;
	glColor3ub(255, 215, 0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(tp.pos.x, tp.pos.y);
	glVertex2f(tp.pos.x + 50 * cos(tp.angle * D2R), tp.pos.y + 50 * sin(tp.angle * D2R));
	glEnd();
	glLineWidth(1);

	glColor3ub(192,192,192);
	circle(tp.pos.x, tp.pos.y, tp.r);

	glColor3ub(30, 144, 255);
	circle(tp.pos.x, tp.pos.y, tp.r - 4);
	
	glColor3ub(135, 206, 250);
	glBegin(GL_TRIANGLES);
	vertex({ 0, 20 }, { 0, 0 }, angle);
	vertex({ -17.3, -10 }, { 0,0 }, angle);
	vertex({ 17.3, -10 }, {0, 0}, angle);
	glEnd();


	glColor3ub(255, 20, 147);
	vprint(tp.pos.x + 170 * cos(tp.angle * D2R), tp.pos.y + 170 * sin(tp.angle * D2R), GLUT_BITMAP_8_BY_13, "%.0f", tp.angle);
	//vprint(tp.pos.x - 12, tp.pos.y - 5, GLUT_BITMAP_8_BY_13, "%.0f", tp.angle);
}

void drawFires() {
	for (int i = 0; i < MAX_FIRE; i++) {
		if (fr[i].active) {
			/*glColor3f(1, 0, 0);
			circle(fr[i].pos.x, fr[i].pos.y, 5);*/

			glColor3ub(255, 215, 0);
			glBegin(GL_TRIANGLES);
			vertex({ 0, 15 }, fr[i].pos, fr[i].startangle);
			vertex({ -12.3, -5 }, fr[i].pos, fr[i].startangle);
			vertex({ 12.3, -5 }, fr[i].pos, fr[i].startangle);
			glEnd();

		}
	}
}

void drawBackground() {

	glColor3ub(0, 0, 0);
	glRectf(-400, -300, 400, 300);

	glColor3ub(255, 255, 0);
	vprint2(-370, 260, 0.09, "HOMEWORKK 3");

	glColor3ub(255, 255, 0);
	vprint2(-370, 240, 0.09, "by ERAY ALTAY");

	glColor3ub(255, 255, 255);

	glBegin(GL_LINES);

	glVertex2f(-400, 0);
	glVertex2f(400, 0);

	glVertex2f(0, -300);
	glVertex2f(0, 300);

	glEnd();

}

void displayMessage() {

	glColor3ub(255, 0, 0);
	vprint2(245, 255, 0.10, "Press F1 to Start");

}

void displayScore() {

	glColor3ub(0, 255, 171);
	vprint2(200, -255, 0.09, "You have destroyed %d stars", scorestreak-3);

	glColor3ub(213, 0, 255);
	vprint2(200, -275, 0.09, "+%d seconds added",(scorestreak-3)*2);

}

void displayRun() {
	glColor3ub(0, 255, 0);
	vprint2(245, 255, 0.10, "%.0f Seconds Left", timeCounter / 100);
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
		drawBackground();
		drawPlayer(p);
		displayMessage();
	
		drawCircle();
		//drawIcon1(i1);
		//drawIcon2(i2);
		//drawIcon3(i3);
		displayScore();

		if (activeTimer = activeTimer)
			activeTimer = !activeTimer;

		else
			activeTimer = activeTimer;

		break;

	case RUN:
		drawBackground();
		drawPlayer(p);
		displayRun(); 
		
		drawCircle();
		drawFires();
		drawIcon1(i1);
		drawIcon2(i2);
		drawIcon3(i3);
		displayScore();
		

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

void resetTarget1() {
	
	i1.startangle = rand() % 361;
	i1.r = rand() % 256;
	i1.g = rand() % 256;
	i1.b = rand() % 256;
	i1.speed1 = 1.5 * (rand() % 200 + (-100)) / 100.0 + 0.5;
	i1.speed2 = 2.0 * (rand() % 200 + (-100)) / 100.0 + 0.5;
	
}


void resetTarget2() {
	
	i2.startangle = rand() % 361;
	i2.r = rand() % 256;
	i2.g = rand() % 256;
	i2.b = rand() % 256;
	i2.speed1 = 1.5 * (rand() % 200 + (-100)) / 100.0 + 0.5;
	i2.speed2 = 2.0 * (rand() % 200 + (-100)) / 100.0 + 0.5;
	
}

void resetTarget3() {

	i3.startangle = rand() % 361;
	i3.r = rand() % 256;
	i3.g = rand() % 256;
	i3.b = rand() % 256;
	i3.speed1 = 1.5 * (rand() % 200 + (-100)) / 100.0 + 0.5;
	i3.speed2 = 2.0 * (rand() % 200 + (-100)) / 100.0 + 0.5;
	
}

void turnPlayer(player_t* tp, float inc) {
	tp->angle += inc;
	if (tp->angle > 360) 
		tp->angle -= 360;
	if (tp->angle < 0) 
		tp->angle += 360;
}

bool testCollision(fire_t fr, entity_t i1) {

	double r = fabs(A);

	float dx = (r * cos(i1.startangle * D2R)) - fr.pos.x;
	float dy = (r * sin(i1.startangle * D2R)) - fr.pos.y;
	float d = sqrt(dx * dx + dy * dy);
	return d <= i1.radius;
}

bool testCollision2(fire_t fr, entity_t i2) {

	double r2 = fabs(A2);

	float dx = (r2 * cos(i2.startangle * D2R)) - fr.pos.x;
	float dy = (r2 * sin(i2.startangle * D2R)) - fr.pos.y;
	float d = sqrt(dx * dx + dy * dy);
	return d <= i2.radius;
}

bool testCollision3(fire_t fr, entity_t i3) {

	double r3 = fabs(A3);

	float dx = (r3 * cos(i3.startangle * D2R)) - fr.pos.x;
	float dy = (r3 * sin(i3.startangle * D2R)) - fr.pos.y;
	float d = sqrt(dx * dx + dy * dy);
	return d <= i3.radius;
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
		turnPlayer(&p, -3);
	}
	if (left) {
		turnPlayer(&p, 3);
	}

	if (shot && fire_rate == 0) {
		int availFire = findAvailableFire();
		if (availFire != -1) {
			fr[availFire].pos = p.pos;
			fr[availFire].angle = p.angle;
			fr[availFire].startangle += 100;
			if (fr[availFire].startangle > 360)
				fr[availFire].startangle -= 360;
			fr[availFire].active = true;
			fire_rate = FIRE_RATE;
		}
	}

	if (fire_rate > 0)
		fire_rate--;


	for (int i = 0; i < MAX_FIRE; i++) {
		if (fr[i].active) {
			fr[i].pos.x += 10 * cos(fr[i].angle * D2R);
			fr[i].pos.y += 10 * sin(fr[i].angle * D2R);


			if (fr[i].pos.x > 500 || fr[i].pos.x < -500 || fr[i].pos.y>500 || fr[i].pos.y < -500) {
				fr[i].active = false;
			}

			if (testCollision(fr[i], i1)) {
				fr[i].active = false;
				resetTarget1();
				scorestreak++;
				timeCounter=timeCounter + 200;
				ammo += 1;
			}

			if (testCollision(fr[i], i2)) {
				fr[i].active = false;
				resetTarget2();
				scorestreak++;
				timeCounter = timeCounter + 200;
				ammo += 1;
			}

			if (testCollision(fr[i], i3)) {
				fr[i].active = false;
				resetTarget3();
				scorestreak++;
				timeCounter = timeCounter + 200;
				ammo += 1;
			}

		}
	}

	i1.startangle += i1.speed2;
	if (i1.startangle > 360)
		i1.startangle -= 360;

	i1.angle += i1.speed1;

	//i2

	i2.startangle += i2.speed2;
	if (i2.startangle > 360)
		i2.startangle -= 360;

	i2.angle += i2.speed1;

	//i3

	i3.startangle += i3.speed2;
	if (i3.startangle > 360)
		i3.startangle -= 360;

	i3.angle += i3.speed1;
	
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

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

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

		/*for (int i = 0; i < MAX; i++) {
			f1cars[i].active = true;
		}*/
	}

	else if (key == GLUT_KEY_F1 && appState == RUN) {
		appState = ENDGAME;
	}

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

	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
		shot = true;
	}

	if (button == GLUT_LEFT_BUTTON && stat == GLUT_UP) {
		shot = false;
	}

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
	glutCreateWindow("Shooting Stars");

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