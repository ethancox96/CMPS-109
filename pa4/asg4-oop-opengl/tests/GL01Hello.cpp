#include <iostream>
#include <string>
using namespace std;
#include <GL/freeglut.h>
#include <libgen.h>

struct window {
   string name;
   int width {256};
   int height {192};
} window;

void display() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);
   glutSwapBuffers();
}

int main (int argc, char** argv) {
   window.name = basename (argv[0]);
   glutInit (&argc, argv);
   glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize (window.width, window.height);
   glutCreateWindow (window.name.c_str());
   glutDisplayFunc (display);
   glutMainLoop();
   return 0;
}
