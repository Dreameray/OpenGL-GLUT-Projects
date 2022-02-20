/*********
ERAY ALTAY
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD   16 // Period for the timer.
#define TIMER_ON        1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height


typedef struct {
	double x, y;
} vec_t;


vec_t mouse;

//
// to draw circle, center at (x,y)
//  radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
   double angle;
   glBegin(GL_POLYGON);
   for (int i = 0; i < 100; i++)
   {
      angle = 2 * PI*i / 100;
      glVertex2f(x + r*cos(angle), y + r*sin(angle));
   }
   glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
   double angle;

   glBegin(GL_LINE_LOOP);
   for (int i = 0; i < 100; i++)
   {
      angle = 2 * PI*i / 100;
      glVertex2f(x + r*cos(angle), y + r*sin(angle));
   }
   glEnd();
}

void print(int x, int y, const char *string, void *font)
{
   int len, i;

   glRasterPos2f(x, y);
   len = (int)strlen(string);
   for (i = 0; i<len; i++)
   {
      glutBitmapCharacter(font, string[i]);
   }
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

void vprint2(int x, int y, double size, const char *string, ...) {
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
   for (i = 0; i<len; i++)
   {
      glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
   }
   glPopMatrix();
}


void display()
{

   // clear window to black
   glClearColor(0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT);

   glPointSize(5);

   glColor3f(1, 1, 0);
   vprint(-80, -5, GLUT_BITMAP_8_BY_13, "x = %.0f, y = %.0f", mouse.x, mouse.y);


   glLoadIdentity();
   glBegin(GL_POINTS);
   glColor3f(1, 1, 1);
   glVertex2f(20, -40); //Original White Point
   glEnd();

   glLoadIdentity();
   glTranslatef(-30, 40, 0); // translating
   glScalef(5, 3, 1); // scaling
   glRotatef(-120, 0, 0, 1); // rotate -120 degrees
   glBegin(GL_POINTS);
   glColor3f(1, 0, 0);
   glVertex2f(20, -40); //Final Red Point after Transitions
   glEnd();

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

void onMove(int x, int y) {
   // Write your codes here.

   mouse = { x - winWidth / (double)2.0, winHeight / (double)2.0 - y };

   // to refresh the window it calls display() function
   glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {
   glutTimerFunc(TIMER_PERIOD, onTimer, 0);
   // Write your codes here.


   // to refresh the window it calls display() function
   glutPostRedisplay(); // display()
}
#endif

void Init() {

   // Smoothing shapes
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}


void main(int argc, char *argv[])
{

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
   glutCreateWindow("Eray Altay Question 6");

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