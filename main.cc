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
#include <vector>
#include "image_loader.h"
#include "3d.h"

#define DFLT_WIDTH 640
#define DFLT_HEIGHT 360

using namespace std;

struct frame_manager
{
	frame_manager() : last(0), now(0), elapsed_ms(0), elapsed_sec(0), frame(0), width(DFLT_WIDTH), height(DFLT_HEIGHT) { }
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
	uint16_t width, height;
};

frame_manager frm;

struct grid_display
{
	grid_display(uint16_t divisions) : listnum(0), compiled(false), div(divisions)
	{
		step_x = frm.width/div;
		step_y = frm.height/div;
		vertex p1, p2, p3, p4;
		for(int y = 0;  y < step_y * div; y += step_y)
		{
			for(int x = 0; x < step_x * div; x += step_x)
			{
				p1.x = x; p1.y = y;
				p2.x = x + step_x; p2.y = y;
				p3.x = x + step_x; p3.y = y + step_y;
				p4.x = x; p4.y = y + step_y;
				if(x == 0) grid.push_back(p4);
				if(x == 0) grid.push_back(p1);
				grid.push_back(p3);
				grid.push_back(p2);
			}
		}
	}
	~grid_display()
	{
		if(listnum != 0)
			glDeleteLists(listnum, 1);
	}

	void build_list()
	{
		if(compiled) return;
		listnum = glGenLists(2);
		GLfloat c1[4] = {1.0, 0.7, 0.0, 0.25};
		GLfloat c2[4] = {0.0, 0.1, 1.0, 0.25};
		GLfloat c3[4] = {1.0, 0.0, 0.0, 0.25};
		GLfloat* c = (GLfloat*)&c1;
		// draw polygons
		glNewList(listnum, GL_COMPILE);
		glPolygonMode(GL_FRONT, GL_FILL);
		for(int i = 0; i < grid.size(); i++)
		{
			bool new_row = (i % ((div + 1) * 2) == 0);
			bool last_in_row = (i % ((div + 1) * 2) == (div + 1) * 2 - 1);
			if (new_row) glBegin(GL_TRIANGLE_STRIP);
			if (new_row) if(c == (GLfloat*) &c1) c = (GLfloat*)&c2; else c = (GLfloat*)&c1;
			glColor4fv(c);
			glVertex2f(grid[i].x, grid[i].y);
			if (last_in_row) glEnd();
		}
		glEndList();
		
		glNewList(listnum + 1, GL_COMPILE);
		glPolygonMode(GL_FRONT, GL_LINE);
		for(int i = 0; i < grid.size(); i++)
		{
			if(i % ((div + 1) * 2) == 0) glBegin(GL_TRIANGLE_STRIP);
			glColor4fv(c3);
			glVertex2f(grid[i].x, grid[i].y);
			if(i % ((div+ 1) * 2) == (div + 1) * 2 - 1) glEnd();
		}
		glPolygonMode(GL_FRONT, GL_FILL);
		glEndList();
		compiled = true;
	}

	void display()
	{
		build_list();
		glCallList(listnum);
		glCallList(listnum + 1);
	}

	GLuint listnum;
	bool compiled;
	vector<vertex> grid;
	uint16_t div;
	float step_x, step_y;
};

void draw_image_at(const string& image_id, uint16_t x, uint16_t y)
{
	glRasterPos2s(x, y);

	image* img = image_manager::get(image_id);
	if(img)
	{
		glDrawPixels(img->width, img->height, img->bpp == 32 ? GL_BGRA : GL_BGR, GL_UNSIGNED_BYTE, img->data);
	}
	else
	{
		cout << "image " << image_id << " is no good" << endl;
	}
}

grid_display grid(20);

void display()
{
	frm.refresh();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glBlendFunc(GL_ONE, GL_ZERO);
	draw_image_at("img/skull01_orig.tga", 0, 0);
	glEnable(GL_BLEND);	
	//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
	draw_image_at("img/skull01_sobel.tga", 0, 0);
	draw_image_at("img/skull01_canvas.tga", 0, 0);


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	grid.display();
	
	
	glutSwapBuffers();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	// no z-buffer
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	frm.width = width;
	frm.height = height;
}

void idle(void)
{
	glutPostRedisplay();
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(DFLT_WIDTH, DFLT_HEIGHT);
	
	glutCreateWindow("demo");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	
	glutMainLoop();
	return EXIT_SUCCESS;
}
