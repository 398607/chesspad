#ifndef OPENGLSUPPORT_H
#define OPENGLSUPPORT_H

#include <cmath>
#include <windows.h>
#include "chesspad.h"
#include <GL/glut.h> // can`t find in mingw? fixed when copied and restarted clion
#include <GL/GL.h>
#include <GL/GLU.h>

int geti(int, int);
int getj(int, int);
void myInit();
void vertex(int, int, float);
void vertex(int, int, int);
void vertexPoi(char, int, int);
void vertexPoiExact(char, int, int);
void displayPad();
void myMovedMouse(int, int);
void myMouse(int, int, int, int);
void myReshape(int, int);

#endif // OPENGLSUPPORT_H