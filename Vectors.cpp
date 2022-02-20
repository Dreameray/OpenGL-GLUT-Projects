/*********
ERAY ALTAY
*********/


#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 700


#define D2R 0.0174532

#define FOV  45

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

/* 2D-VECTOR LIBRARY STARTS  */
typedef struct vec_t{
   double x, y;
} vec_t;

typedef struct {
   double magnitude, angle;
} polar_t;

typedef struct player_t{
	vec_t pos;
	float angle;
} player_t;

player_t P = { { -50 * sqrt(3) , 200}, 120 };

double magV(vec_t v) {
   return sqrt(v.x * v.x + v.y * v.y);
}

double angleV(vec_t v) {
   double angle = atan2(v.y, v.x)/D2R;
   return angle < 0 ? angle + 360 : angle;
}

vec_t addV(vec_t v1, vec_t v2) {
   return  { v1.x + v2.x, v1.y + v2.y };
}

vec_t subV(vec_t v1, vec_t v2) {
   return { v1.x - v2.x, v1.y - v2.y };
}

vec_t mulV(double k, vec_t v) {
   return { k * v.x, k * v.y };
}

double dotP(vec_t v1, vec_t v2) {
   return v1.x * v2.x + v1.y * v2.y;
}

vec_t unitV(vec_t v) {
   return mulV(1.0 / magV(v), v);
}

// convert from polar representation to rectangular representation
vec_t pol2rec(polar_t p) {
   return { p.magnitude * cos(p.angle * D2R),  p.magnitude * sin(p.angle * D2R) };
}

polar_t rec2pol(vec_t v) {
   return { magV(v), angleV(v) };
}

double angleBetween2V(vec_t v1, vec_t v2) {
   double magV1 = magV(v1);
   double magV2 = magV(v2);
   double dot = dotP(v1, v2);
   double angle = acos(dot / (magV1 * magV2)) / D2R; // in degree
   return angle;
}

/* VECTOR LIBRARY ENDS */


/* GLOBAL VARIABLES */
vec_t v1 = pol2rec({ 150, 90}), v2 = { 50* sqrt(3) , -50}, vAdd, vSub, v1Unit, v2Unit;
double angle;  // angle between v1 and v2

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

// display text with variables.
void vprint(int x, int y, void *font, const char *string, ...)
{
   va_list ap;
   va_start(ap, string);
   char str[1024];
   vsprintf_s(str, string, ap);
   va_end(ap);

   int len, i;
   glRasterPos2f(x, y);
   len = (int)strlen(str);
   for (i = 0; i<len; i++)
   {
      glutBitmapCharacter(font, str[i]);
   }
}


void drawVector(vec_t v, const char label[]) {
   
   double magnitude = magV(v);
   double angle = angleV(v);

   glBegin(GL_LINES);
   //vector
   glVertex2f(0, 0);
   glVertex2f(v.x, v.y);

   // arrow tip 1
   vec_t tip1 = pol2rec({ magnitude * 0.95, angle - 2 });
   glVertex2f(tip1.x, tip1.y);
   glVertex2f(v.x, v.y);

   // arrow tip 2
   vec_t tip2 = pol2rec({ magnitude * 0.95, angle + 2 });
   glVertex2f(tip2.x, tip2.y);
   glVertex2f(v.x,v.y);

   glEnd();

  vec_t textPos = pol2rec({ magnitude + 25, angle });
   vprint(textPos.x + 20, textPos.y, GLUT_BITMAP_HELVETICA_10, "%s", label);
   vprint( textPos.x + 20, textPos.y -12, GLUT_BITMAP_HELVETICA_10,
      "Vector Rectangle= <%.1f , %.1f>", v.x, v.y );

   vprint(textPos.x + 20, textPos.y - 24, GLUT_BITMAP_HELVETICA_10, "Vector Polar = (%.1f,  %.1f degree)", magnitude, angle);

}

void drawAxis() {
   glColor3f(0.5, 0.5, 0.5);
   glBegin(GL_LINES);
   glVertex2f(-480, 0);
   glVertex2f(480, 0);

   glVertex2f(0, 280);
   glVertex2f(0, -280);
   glEnd();

}

void drawPointC() {

	glColor3f(1, 0, 0);
	circle(-95, 250, 3);

	glColor3f(1, 0, 0);
	vprint(-100, 265, GLUT_BITMAP_HELVETICA_10, "C");

}

void drawSight(player_t p) {

	vec_t E1 = addV(p.pos, pol2rec({ 1000, p.angle - FOV }));
	vec_t E2 = addV(p.pos, pol2rec({ 1000, p.angle + FOV }));

	// Field of View Area
	glColor4f(1.0, 1, 0, 0.2);
	glBegin(GL_TRIANGLES);
	glVertex2f(p.pos.x, p.pos.y);
	glVertex2f(E1.x, E1.y);
	glVertex2f(E2.x, E2.y);
	glEnd();
}

void display()
{
   // clear window to black
   glClearColor(0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT);

   drawAxis();

   glColor3f(1, 1, 0);
   drawVector(v1, "A");

   glColor3f(0, 1, 0);
   drawVector(v2, "B");

   glColor3f(0, 0, 1);
   drawVector(vSub, "A - B");

   drawSight(P);

   glColor3f(1, 0.2, 0.2);
   vprint( 30, 20, GLUT_BITMAP_HELVETICA_10, "Angle Between A and B = %.2f", angle);

   drawPointC();
   glutSwapBuffers();
}


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


void Init() {

   // Smoothing shapes
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   vAdd = addV(v1, v2);
   vSub = subV(v1, v2);
   v1Unit = unitV(v1);
   v2Unit = unitV(v2);
   angle = angleBetween2V(v1, v2);

}


void main(int argc, char *argv[])
{

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
   glutCreateWindow("Eray Altay Q2");

   glutDisplayFunc(display);
   glutReshapeFunc(onResize);
   glutKeyboardFunc(onKeyDown);
   Init();
   glutMainLoop();

}