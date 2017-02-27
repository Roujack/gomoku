#pragma once
/*#ifndef opengl
#define opengl*/
#include "include/Angel.h"
#pragma comment(lib, "glew32.lib")
#include<cstdio>
#include "mmsystem.h" 
#include <windows.h>  
#pragma comment (lib, "winmm.lib") 

// ���Բ�ϵĵ�
vec2 getEllipseVertex(vec2 center, double scale, double verticalScale, double angle);
void  init();
/** \brief ����Բ������*/
void generateEllipsePoints(bool currentState, int startVertexIndex, int numPoints,
	vec2 center, double scale, double verticalScale);
// ����������ϵĵ�,������λ��������
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