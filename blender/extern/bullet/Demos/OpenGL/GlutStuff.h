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
#ifndef GLUT_STUFF_H
#define GLUT_STUFF_H

//to be implemented by the demo
void clientDisplay();
void clientMoveAndDisplay();
void clientResetScene();


int glutmain(int argc, char **argv,int width,int height,const char* title);

void	setCameraDistance(float dist);
int		getDebugMode();
void	setDebugMode(int mode);


#endif //GLUT_STUFF_H