#pragma once
/*#ifndef opengl
#define opengl*/
#include "include/Angel.h"
#pragma comment(lib, "glew32.lib")
#include<cstdio>
#include "mmsystem.h" 
#include <windows.h>  
#pragma comment (lib, "winmm.lib") 

// 获得圆上的点
vec2 getEllipseVertex(vec2 center, double scale, double verticalScale, double angle);
void  init();
/** \brief 生成圆的坐标*/
void generateEllipsePoints(bool currentState, int startVertexIndex, int numPoints,
	vec2 center, double scale, double verticalScale);
// 获得正方形上的点,用来定位最后落棋点
void updateSquarePoints(float x, float y);
void display();
void mainWindowMouse(int button, int state, int x, int y);
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void idle(void);
void music(char ch);
void mainWindowMenuEvents(int menuChoice);
void mainWindowSetupMenu();
//#endif