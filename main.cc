/*
 *  main.cc
 *  glut_test
 *
 *  Created by radu on 12/10/08.
 *  Copyright 2008 Optiver Holding BV. All rights reserved.
 *
 */


#include <stdlib.h>
#include <stdint.h>
#include <GLUT/glut.h>
#include <iostream>
#include <cmath>
#include <map>
#include "image_loader.h"

using namespace std;


struct frame_manager
{
	frame_manager() : last(0), now(0), elapsed_ms(0), elapsed_sec(0), frame(0) { }
	void refresh()
	{
		if(last == 0)
		{
			last = glutGet(GLUT_ELAPSED_TIME);
		}
		now = glutGet(GLUT_ELAPSED_TIME);
		elapsed_ms = now - last;
		elapsed_sec = elapsed_ms/1000.0f;
		last = now;
		frame++;
	}
	uint32_t last, now, elapsed_ms;
	float elapsed_sec;
	uint32_t frame;
};

frame_manager frm;

void display()
{
	frm.refresh();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	image* img = image_manager::get("img/hvp.tga");
	if(img)
	{
		glDrawPixels(img->width, img->height, GL_BGRA, GL_UNSIGNED_BYTE, img->data);
	}
	else
	{
		cout << "image invalid" << endl;
	}

	static float where_x = 0;
	float margin = glutGet(GLUT_WINDOW_WIDTH);

	where_x += 20. * frm.elapsed_sec; // 512 pixels per second
	if(where_x > margin)
	{
		where_x -= margin;
	}
	glPushMatrix();
	glTranslatef(where_x, 100 +  20.*sin(frm.frame*3.14/1000.), 0);
	
	
	glBegin(GL_QUADS);
	glColor3f(1,1,1);
	glVertex2f(  0.0f,   0.0f);
	glColor3f(1,.5,.2);
	glVertex2f(128.0f,   0.0f);
	glColor3f(0,0,1);
	glVertex2f(128.0f, 128.0f);
	glColor3f(0,1,0);
	glVertex2f(  0.0f, 128.0f);
	glEnd();

	glPopMatrix();	
	glutSwapBuffers();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
}

void idle(void)
{
	glutPostRedisplay();
}

void tga_test(const string& fname)
{
	image* img = image_loader::load(fname);
	if(!img)
	{
		cout << "image loading failed" << endl;
		exit(1);
	}
	cout << "img [" << img->id << "] : " << img->width << "x" << img->height << endl;
	exit(0);
}

int main(int argc, char** argv)
{
	if(argc > 1) tga_test(string(argv[1]));
	image_manager::dump("img/hvp.tga", "img/hvp.raw");
	
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize((600*16)/9, 600);
	
	glutCreateWindow("demo");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	
	glutMainLoop();
	return EXIT_SUCCESS;
}
