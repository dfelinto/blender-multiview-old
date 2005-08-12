/*
 * Copyright (c) 2005 Erwin Coumans <www.erwincoumans.com>
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies.
 * Erwin Coumans makes no representations about the suitability 
 * of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 */

#ifdef WIN32//for glut.h
#include <windows.h>
#endif
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "IDebugDraw.h"
//see IDebugDraw.h for modes
static int	sDebugMode = 0;

int		getDebugMode()
{
	return sDebugMode ;
}

void	setDebugMode(int mode)
{
	sDebugMode = mode;
}




#include "GlutStuff.h"

void myinit(void) {

    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    /*	light_position is NOT default value	*/
    GLfloat light_position0[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat light_position1[] = { -1.0, -1.0, -1.0, 0.0 };
  
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
 

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //  glEnable(GL_CULL_FACE);
    //  glCullFace(GL_BACK);
}


static float DISTANCE = 15; 
void	setCameraDistance(float dist)
{
	DISTANCE  = dist;
}

static float ele = 0, azi = 0;
float eye[3] = {0, 0, DISTANCE};
static float center[3] = {0, 0, 0};
static const double SCALE_BOTTOM = 0.5;
static const double SCALE_FACTOR = 2;

bool stepping= true;
bool singleStep = false;


static bool idle = false;
  
void toggleIdle() {

	
    if (idle) {
		glutIdleFunc(clientMoveAndDisplay);
        idle = false;
    }
    else {
        glutIdleFunc(0);
        idle = true;
    }
}


void setCamera() {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	float rele = ele * 0.01745329251994329547;// rads per deg
    float razi = azi * 0.01745329251994329547;// rads per deg
    eye[0] = DISTANCE * sin(razi) * cos(rele);
	eye[1] = DISTANCE * sin(rele);
	eye[2] = DISTANCE * cos(razi) * cos(rele);
 

    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
    gluLookAt(eye[0], eye[1], eye[2], 
              center[0], center[1], center[2], 
              0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
}



const float STEPSIZE = 5;

void stepLeft() { azi -= STEPSIZE; if (azi < 0) azi += 360; setCamera(); }
void stepRight() { azi += STEPSIZE; if (azi >= 360) azi -= 360; setCamera(); }
void stepFront() { ele += STEPSIZE; if (azi >= 360) azi -= 360; setCamera(); }
void stepBack() { ele -= STEPSIZE; if (azi < 0) azi += 360; setCamera(); }
void zoomIn() { DISTANCE -= 1; setCamera(); }
void zoomOut() { DISTANCE += 1; setCamera(); }

void myReshape(int w, int h) {
    glViewport(0, 0, w, h);
    setCamera();
}

int lastKey  = 0;

void myKeyboard(unsigned char key, int x, int y)
{
	lastKey = 0;

    switch (key) 
    {
    case 'q' : exit(0); break;

    case 'l' : stepLeft(); break;
    case 'r' : stepRight(); break;
    case 'f' : stepFront(); break;
    case 'b' : stepBack(); break;
    case 'z' : zoomIn(); break;
    case 'x' : zoomOut(); break;
    case 'i' : toggleIdle(); break;
	case 't' : 
			if (sDebugMode & IDebugDraw::DBG_DrawText)
				sDebugMode = sDebugMode & (~IDebugDraw::DBG_DrawText);
			else
				sDebugMode |= IDebugDraw::DBG_DrawText;
		   break;
	case 'y':		
			if (sDebugMode & IDebugDraw::DBG_DrawFeaturesText)
				sDebugMode = sDebugMode & (~IDebugDraw::DBG_DrawFeaturesText);
			else
				sDebugMode |= IDebugDraw::DBG_DrawFeaturesText;
		break;
	case 'a':	
		if (sDebugMode & IDebugDraw::DBG_DrawAabb)
				sDebugMode = sDebugMode & (~IDebugDraw::DBG_DrawAabb);
			else
				sDebugMode |= IDebugDraw::DBG_DrawAabb;
			break;
		case 'c' : 
			if (sDebugMode & IDebugDraw::DBG_DrawContactPoints)
				sDebugMode = sDebugMode & (~IDebugDraw::DBG_DrawContactPoints);
			else
				sDebugMode |= IDebugDraw::DBG_DrawContactPoints;
			break;
		break;

	case 'o' :
		{
			stepping = !stepping;
			break;
		}
	case 's' : clientMoveAndDisplay(); break;
//    case ' ' : newRandom(); break;
	case ' ':
		clientResetScene();
			break;
	case 'd':
		{
			if (sDebugMode & IDebugDraw::DBG_NoDeactivation)
				sDebugMode = sDebugMode & (~IDebugDraw::DBG_NoDeactivation);
			else
				sDebugMode |= IDebugDraw::DBG_NoDeactivation;
			break;
		}
    default:
//        std::cout << "unused key : " << key << std::endl;
        break;
    }

	clientDisplay();


}


void mySpecial(int key, int x, int y)
{
    switch (key) 
    {
    case GLUT_KEY_LEFT : stepLeft(); break;
    case GLUT_KEY_RIGHT : stepRight(); break;
    case GLUT_KEY_UP : stepFront(); break;
    case GLUT_KEY_DOWN : stepBack(); break;
    case GLUT_KEY_PAGE_UP : zoomIn(); break;
    case GLUT_KEY_PAGE_DOWN : zoomOut(); break;
    case GLUT_KEY_HOME : toggleIdle(); break;
    default:
//        std::cout << "unused (special) key : " << key << std::endl;
        break;
    }
	clientDisplay();
}


void goodbye( void)
{
    printf("goodbye \n");
    exit(0);
}


void menu(int choice)
{
    static int fullScreen = 0;
    static int px, py, sx, sy;

    switch(choice) {
    case 1:
        if (fullScreen == 1) {
            glutPositionWindow(px,py);
            glutReshapeWindow(sx,sy);
            glutChangeToMenuEntry(1,"Full Screen",1);
            fullScreen = 0;
        } else {
            px=glutGet((GLenum)GLUT_WINDOW_X);
            py=glutGet((GLenum)GLUT_WINDOW_Y);
            sx=glutGet((GLenum)GLUT_WINDOW_WIDTH);
            sy=glutGet((GLenum)GLUT_WINDOW_HEIGHT);
            glutFullScreen();
            glutChangeToMenuEntry(1,"Close Full Screen",1);
            fullScreen = 1;
        }
        break;
    case 2:
        toggleIdle();
        break;
    case 3:
        goodbye();
        break;
    default:
        break;
    }
}

void createMenu()
{
    glutCreateMenu(menu);
    glutAddMenuEntry("Full Screen", 1);
    glutAddMenuEntry("Toggle Idle (Start/Stop)", 2);
    glutAddMenuEntry("Quit", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}



int glutmain(int argc, char **argv,int width,int height,const char* title) {
    

	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);
	
    myinit();
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(mySpecial);
    glutReshapeFunc(myReshape);
    createMenu();
	glutIdleFunc(clientMoveAndDisplay);

	glutDisplayFunc( clientDisplay );

    glutMainLoop();
    return 0;
}
