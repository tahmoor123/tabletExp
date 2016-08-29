#include "glut.h"    // Header File For The GLUT Library 
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN  /* somewhat limit Win32 pollution */
#include <windows.h>
#endif
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <stdio.h>      // Header file for standard file i/o.
//#include <unistd.h>     // needed to sleep.
#include <math.h>
#include <string.h>

//#include <stdlib.h>     // Header file for malloc/free.

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TRUE        1           // standard booleans
#define FALSE       0

#define ESCAPE	27

#define NUM_TARGETS 10
#define NUM_REPEATS 36

#define TARGET_RADIUS 13

#define ROTATION_CW  (M_PI*(-30.0)/180.0)		//Cursor rotation 30 degree Clockwise
#define ROTATION_CCW  (M_PI*(30.0)/180.0)		//Cursor rotation 30 degree Clockwise
#define ROTATION_DEGREE_CW  -30.0
#define ROTATION_DEGREE_CCW  30.0
#define CENTER_HOLD_TIME 1000
#define CENTER_HOLD_TIME_2 500
#define BLANK 100
#define FLASH 100
#define MOVEMENT_DELAY_TIME 2000
#define TARGET_HOLD_TIME 1000


#define X_OFFSET 512
#define Y_OFFSET 170

//#define DISTANCE_FROM_HOME 220
#define DISTANCE_12 220
#define DISTANCE_15 330

double rotation = ROTATION_CW;
int rotation_degree = ROTATION_DEGREE_CW;

int obs_hit = 0;
int GOAL = 0;
int error = 0;
int dir = 1;

int order[NUM_TARGETS*NUM_REPEATS];
int temp_order[NUM_TARGETS];
int temp;
FILE *log_file;	// the pointer to the output file

int trial;

int step;	// 0 - moving to center
			// 1 - holding on center
			// 2 - what for start of movement to target
			// 3 - move to target
			// 4 - hold on target
			



int mouse_pos_x;
int mouse_pos_y;
int temp_mouse_x;
int temp_mouse_y;
int cursor_pos_x;
int cursor_pos_y;
int target_idx;
int result;
// will be dynamically allocated in experiment setup
// we done know the number of targets, but they will have 
// an x and a y!
int *target_locations[4];
int *trials[5];


void report()
{
	if(GOAL!=0 && GOAL!=1)
	{
		float target_x_position_pixel = X_OFFSET + trials[3][trial] * cos(M_PI * (trials[2][trial]) / 180.0);
		float target_y_position_pixel = Y_OFFSET + trials[3][trial] * sin(M_PI * (trials[2][trial]) / 180.0);
		//Rounding the values
		//Since round() doesn't work in Microsfot Visual Studio, we used floor function and adding 0.5 to the value.It has same effect.
		target_x_position_pixel = floor(target_x_position_pixel + 0.5);
		target_y_position_pixel = floor(target_y_position_pixel + 0.5);
		// time		trial
		printf("Trial:%d Target: %d Angle: %d Dist: %d Rotate: %d Step: %d Time: %d\n", trials[0][trial], trials[1][trial], trials[2][trial], trials[3][trial], trials[4][trial], step, glutGet(GLUT_ELAPSED_TIME));
		/*printf("time: %d\ttrial: %d\trotation: %f\tstep: %d\t1\n",	
								glutGet(GLUT_ELAPSED_TIME), 
								trial,
								rotation_degree,
								step
								
				);*/
		if(trials[1][trial] <= 4)
		{
			fprintf(log_file, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%f\t%f\t1\n",	
								glutGet(GLUT_ELAPSED_TIME), 
								trial, 
								mouse_pos_x, 
								mouse_pos_y,
								cursor_pos_x, 
								cursor_pos_y,
								trials[4][trial],
								step, 
								trials[2][trial], 
								target_x_position_pixel,
								target_y_position_pixel
								
					);
		}
		else if(trials[1][trial] >= 5)
		{
			fprintf(log_file, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%f\t%f\t-1\n",	
								glutGet(GLUT_ELAPSED_TIME), 
								trial, 
								mouse_pos_x, 
								mouse_pos_y,
								cursor_pos_x, 
								cursor_pos_y,
								trials[4][trial],
								step, 
								trials[2][trial], 
								target_x_position_pixel,
								target_y_position_pixel
								
					);
		}
		/*fprintf(log_file, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%f\t%f\t1\n",	
					glutGet(GLUT_ELAPSED_TIME), 
					trial, 
					mouse_pos_x, 
					mouse_pos_y,
					cursor_pos_x, 
					cursor_pos_y,
					rotation_degree,
					step, 
					trials[2][trial], 
					target_x_position_pixel,
					target_y_position_pixel			
				);*/


	}
}



void cleanup(int return_code)
{
	// free target locations
	free(target_locations[0]);
	free(target_locations[1]);

	fclose(log_file);

	glutLeaveGameMode();
	_exit(return_code);
}


/* The function called whenever a key is pressed. */
void keyPressed(unsigned char key, int x, int y) 
{
    // avoid thrashing this procedure
    Sleep(100);

	switch(key)
	{
		case ESCAPE:
			cleanup(0);
			break;
	}
}// end keyPressed

void mouseMoved(int x, int y)
{
//	printf("[%d\t%d]\n", x, y);
//	printf("%d\n", step);

	mouse_pos_x	= x;
	mouse_pos_y = 768 - y;

	cursor_pos_x = (mouse_pos_x - (X_OFFSET))*cos((M_PI*(trials[4][trial])/180.0)) - (mouse_pos_y - (Y_OFFSET))*sin((M_PI*(trials[4][trial])/180.0)) + (X_OFFSET);
	cursor_pos_y = (mouse_pos_x - (X_OFFSET))*sin((M_PI*(trials[4][trial])/180.0)) + (mouse_pos_y - (Y_OFFSET))*cos((M_PI*(trials[4][trial])/180.0)) + (Y_OFFSET);

	//cursor_pos_x = (mouse_pos_x - (X_OFFSET))*cos(rotation) - (mouse_pos_y - (Y_OFFSET))*sin(rotation) + (X_OFFSET);
	//cursor_pos_y = (mouse_pos_x - (X_OFFSET))*sin(rotation) + (mouse_pos_y - (Y_OFFSET))*cos(rotation) + (Y_OFFSET);
	
	//printf("pos x is %d, and pos y is %d\n", cursor_pos_x, cursor_pos_y);
	//printf("rotation is %d", rotation);
	glutPostRedisplay();
	report();
}

void mousePressed(int button, int state, int x, int y)
{

}

int inside_target(int x, int y, int target)
{
	int target_x;
	int target_y;

	if (target == 360)
	{
		target_x = X_OFFSET + 0 * cos(M_PI * (trials[2][target] - trials[4][target]) / 180.0);
		target_y = Y_OFFSET + 0 * sin(M_PI * (trials[2][target] - trials[4][target]) / 180.0);
	}
	else
	{
		target_x = X_OFFSET + trials[3][target] * cos(M_PI * (trials[2][target] - trials[4][target]) / 180.0);
		target_y = Y_OFFSET + trials[3][target] * sin(M_PI * (trials[2][target] - trials[4][target]) / 180.0);
	}
	// check if the distance from the center of the target to the given point (x, y)
	// is less than the radius
	//printf("target x is %d, target y is %d\t", target_x, target_y);
	//printf(" currently at x = %d and y = %d\n", x, y);
	if( (x - target_x)*(x - target_x) + (y - target_y)*(y - target_y) < TARGET_RADIUS*TARGET_RADIUS)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}


/* The main drawing function. */
int last_time;
void DrawGLScene()
{
	int k;
	int target_x;
	int target_y;
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();				// Reset The View
	
	if(GOAL == 1)
	{
		target_x = 500;
		target_y = 300;
		
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		
/*		if(trials[4][trial] == ROTATION_DEGREE_CW)
		{
			glBegin(GL_LINES);
			glVertex3f(690.0f, 384.0f, 0.0f);
			glVertex3f(425.0f, 384.0f, 0.0f);
			glVertex3f(425.0f, 334.0f, 0.0f);
			glVertex3f(425.0f, 434.0f, 0.0f);
			glVertex3f(425.0f, 434.0f, 0.0f);
			glVertex3f(375.0f, 384.0f, 0.0f);
			glVertex3f(375.0f, 384.0f, 0.0f);
			glVertex3f(425.0f, 334.0f, 0.0f);
			glEnd();

			glBegin(GL_LINES);
			// T
			// vertical
			glVertex3f(375.0f, 284.0f, 0.0f);
			glVertex3f(375.0f, 484.0f, 0.0f);
			// horizontal
			glVertex3f(300.0f, 484.0f, 0.0f);
			glVertex3f(450.0f, 484.0f, 0.0f);
			glEnd();
			// O 384 512
			glBegin(GL_LINE_LOOP);
			glVertex3f(462.0f, 484.0f, 0.0f);
			glVertex3f(562.0f, 484.0f, 0.0f);
			glVertex3f(612.0f, 434.0f, 0.0f);
			glVertex3f(612.0f, 334.0f, 0.0f);
			glVertex3f(562.0f, 284.0f, 0.0f);
			glVertex3f(462.0f, 284.0f, 0.0f);
			glVertex3f(412.0f, 334.0f, 0.0f);
			glVertex3f(412.0f, 434.0f, 0.0f);
			glEnd();
			// P
			glBegin(GL_LINE_LOOP);
			glVertex3f(620.0f, 284.0f, 0.0f);
			glVertex3f(620.0f, 484.0f, 0.0f);
			glVertex3f(640.0f, 484.0f, 0.0f);
			glVertex3f(660.0f, 474.0f, 0.0f);
			glVertex3f(680.0f, 459.0f, 0.0f);
			glVertex3f(680.0f, 409.0f, 0.0f);
			glVertex3f(660.0f, 394.0f, 0.0f);
			glVertex3f(640.0f, 384.0f, 0.0f);
			glVertex3f(620.0f, 384.0f, 0.0f);
			glEnd();

			/*glBegin(GL_TRIANGLES);
			glVertex2f(350.0f, 200.0f);
			glVertex2f(650.0f, 200.0f);
			glVertex2f(500.0f, 300.0f);
			glEnd();

		
		}
		else if(trials[4][trial] == ROTATION_DEGREE_CCW)
		{
			glBegin(GL_LINES);
			glVertex3f(334.0f, 384.0f, 0.0f);
			glVertex3f(599.0f, 384.0f, 0.0f);
			glVertex3f(599.0f, 334.0f, 0.0f);
			glVertex3f(599.0f, 434.0f, 0.0f);
			glVertex3f(599.0f, 434.0f, 0.0f);
			glVertex3f(649.0f, 384.0f, 0.0f);
			glVertex3f(649.0f, 384.0f, 0.0f);
			glVertex3f(599.0f, 334.0f, 0.0f);
			glEnd();

			//B
			glBegin(GL_LINE_LOOP);
			glVertex3f(290.0f, 350.0f, 0.0f);
			glVertex3f(290.0f, 450.0f, 0.0f);
			glVertex3f(300.0f, 450.0f, 0.0f);
			glVertex3f(310.0f, 445.0f, 0.0f);
			glVertex3f(320.0f, 437.5f, 0.0f);
			glVertex3f(320.0f, 412.5f, 0.0f);
			glVertex3f(310.0f, 405.0f, 0.0f);
			glVertex3f(300.0f, 400.0f, 0.0f);
			glVertex3f(290.0f, 400.0f, 0.0f);
			glVertex3f(305.0f, 400.0f, 0.0f);
			glVertex3f(317.5f, 395.0f, 0.0f);
			glVertex3f(325.0f, 387.5f, 0.0f);
			glVertex3f(325.0f, 362.5f, 0.0f);
			glVertex3f(317.5f, 355.0f, 0.0f);
			glVertex3f(305.0f, 350.0f, 0.0f);
			glEnd();
			// O
			glBegin(GL_LINE_LOOP);
			glVertex3f(355.0f, 450.0f, 0.0f);
			glVertex3f(405.0f, 450.0f, 0.0f);
			glVertex3f(430.0f, 425.0f, 0.0f);
			glVertex3f(430.0f, 375.0f, 0.0f);
			glVertex3f(405.0f, 350.0f, 0.0f);
			glVertex3f(355.0f, 350.0f, 0.0f);
			glVertex3f(330.0f, 375.0f, 0.0f);
			glVertex3f(330.0f, 425.0f, 0.0f);
			glEnd();
			// T
			glBegin(GL_LINES);
			glVertex3f(447.5f, 350.0f, 0.0f);
			glVertex3f(447.5f, 450.0f, 0.0f);
			glVertex3f(410.0f, 450.0f, 0.0f);
			glVertex3f(490.0f, 450.0f, 0.0f);
			glEnd();
			// T
			glBegin(GL_LINES);
			glVertex3f(532.5f, 350.0f, 0.0f);
			glVertex3f(532.5f, 450.0f, 0.0f);
			glVertex3f(495.0f, 450.0f, 0.0f);
			glVertex3f(570.0f, 450.0f, 0.0f);
			glEnd();
			// O
			glBegin(GL_LINE_LOOP);
			glVertex3f(580.0f, 450.0f, 0.0f);
			glVertex3f(630.0f, 450.0f, 0.0f);
			glVertex3f(655.0f, 425.0f, 0.0f);
			glVertex3f(655.0f, 375.0f, 0.0f);
			glVertex3f(630.0f, 350.0f, 0.0f);
			glVertex3f(580.0f, 350.0f, 0.0f);
			glVertex3f(555.0f, 375.0f, 0.0f);
			glVertex3f(555.0f, 425.0f, 0.0f);
			glEnd();
			// M
			glBegin(GL_LINE_STRIP);
			glVertex3f(660.0f, 350.0f, 0.0f);
			glVertex3f(660.0f, 450.0f, 0.0f);
			glVertex3f(695.0f, 370.0f, 0.0f);
			glVertex3f(695.0f, 450.0f, 0.0f);
			glVertex3f(730.0f, 350.0f, 0.0f);
			glEnd();


		}/*
		/*glBegin(GL_POLYGON);
		
		for(k = 0;k < 360; k = k + 10)
		{
			double x = ((double)target_x + 70*cos(M_PI * k/180.0));
			double y = ((double)target_y + 70*sin(M_PI * k/180.0));
			glVertex3f(x, y, 0);
		}
		glEnd();*/
	}
	if(GOAL == 0)
	{
		target_x = 500;
		target_y = 300;
		
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		
		glBegin(GL_POLYGON);
		
		for(k = 0;k < 360; k = k + 10)
		{
			double x = ((double)target_x + 70*cos(M_PI * k/180.0));
			double y = ((double)target_y + 70*sin(M_PI * k/180.0));
			glVertex3f(x, y, 0);
		}
		glEnd();
	}
	else if(GOAL != 0 && GOAL != 1)
	{
		// draw the cursor
		if (rotation_degree > 0)
		{
			glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
		}
		else if (rotation_degree < 0)
		{
			glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
		}
		else
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
		glBegin(GL_POLYGON);
		for(k = 0;k < 360; k = k + 10)
		{
			double x = ((double)cursor_pos_x + 10*cos(M_PI * k/180.0));
			double y = ((double)cursor_pos_y + 10*sin(M_PI * k/180.0));
			glVertex3f(x, y, 0);
		}
		glEnd();
		
		/*if (rotation_degree < 0 && target_idx != NUM_TARGETS)
		{
			target_locations[1][target_idx] = DISTANCE_15;
		}
		else if (rotation_degree > 0 && target_idx != NUM_TARGETS)
		{
			target_locations[1][target_idx] = DISTANCE_12;
		}*/

		// draw the target
		//printf("Trial:%d Target: %d Angle: %d Dist: %d Rotate: %d Step: %d Time: %d\n", trials[0][trial], trials[1][trial], trials[2][trial], trials[3][trial], trials[4][trial], step, glutGet(GLUT_ELAPSE_TIME));
		if (step < 2)
		{
			target_x = X_OFFSET + 0 * cos(M_PI * 0 / 180.0);
			target_y = Y_OFFSET + 0 * sin(M_PI * 0 / 180.0);

			if (obs_hit == 1)
			{
				if (trials[1][trial] < 5)
				{
					glColor4f(1.0f,0.0f,0.0f,1.0f);
					glBegin(GL_QUADS);
					glVertex3f(0.0f, 257.5f, 0.0f);
					glVertex3f(512.0f, 257.5f, 0.0f);
					glVertex3f(512.0f, 239.13f, 0.0f);
					glVertex3f(0.0f, 239.13f, 0.0f);
					glEnd();
				}
				else
				{
					glColor4f(1.0f,0.0f,0.0f,1.0f);
					glBegin(GL_QUADS);
					glVertex3f(1024.0f, 257.5f, 0.0f);
					glVertex3f(512.0f, 257.5f, 0.0f);
					glVertex3f(512.0f, 239.13f, 0.0f);
					glVertex3f(1024.0f, 239.13f, 0.0f);
					glEnd();
				}
			}
		}
		else
		{
			target_x = X_OFFSET + trials[3][trial] * cos(M_PI * trials[2][trial] / 180.0);
			target_y = Y_OFFSET + trials[3][trial] * sin(M_PI * trials[2][trial] / 180.0);

			if (trials[1][trial] < 5)
			{
				glColor4f(1.0f,0.2f,1.0f,1.0f);
				glBegin(GL_QUADS);
				glVertex3f(0.0f, 257.5f, 0.0f);
				glVertex3f(512.0f, 257.5f, 0.0f);
				glVertex3f(512.0f, 239.13f, 0.0f);
				glVertex3f(0.0f, 239.13f, 0.0f);
				glEnd();
			}
			else
			{
				glColor4f(1.0f,0.2f,1.0f,1.0f);
				glBegin(GL_QUADS);
				glVertex3f(1024.0f, 257.5f, 0.0f);
				glVertex3f(512.0f, 257.5f, 0.0f);
				glVertex3f(512.0f, 239.13f, 0.0f);
				glVertex3f(1024.0f, 239.13f, 0.0f);
				glEnd();
			}
		}
		//target_y = 384 + target_locations[1][target_idx] * sin(M_PI * target_locations[0][target_idx] / 180.0);

		if(target_x == X_OFFSET && target_y == Y_OFFSET)
		{
			//printf("purple ");
			glColor4f(0.0f, 1.0f, 0.6f, 1.0f);
		}
		else
		{
			//printf("yellow ");
			glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		}
		glBegin(GL_POLYGON);

		
		for(k = 0;k < 360; k = k + 10)
		{
			double x = ((double)target_x + TARGET_RADIUS*cos(M_PI * k/180.0));
			double y = ((double)target_y + TARGET_RADIUS*sin(M_PI * k/180.0));
			glVertex3f(x, y, 0);
		}
		glEnd();
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    // since this is double buffered, swap the buffers to display what just got drawn.
//printf("before: %d\n", glutGet(GLUT_ELAPSED_TIME) - last_time);
//last_time = glutGet(GLUT_ELAPSED_TIME);
	glutSwapBuffers();
//printf("after: %d\n", glutGet(GLUT_ELAPSED_TIME) - last_time);
//last_time = glutGet(GLUT_ELAPSED_TIME);
}// end DrawGLScene

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// Clear The Background Color To Blue 
    glClearDepth(1.0);						// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);					// The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);				// Enables Depth Testing
    glShadeModel(GL_SMOOTH);				// Enables Smooth Color Shading
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();						// Reset The Projection Matrix

	gluOrtho2D( 0.0, Width, 0.0, Height );
        
    glMatrixMode(GL_MODELVIEW);
}


int timer;
int start_outward_x;
int start_outward_y;
void idle()
{
//	printf("%d\n", step);
//printf("idle %d\n", step);
	// idle holds all other processes!!!
	if(trial >= NUM_TARGETS*NUM_REPEATS)
	{
		cleanup(1);
	}
	switch(step)
	{
		// step 0: move to center target. 
		//		|-> this state is finished when they mouse first enters the center target
		case 0:	// Move to Center target
//printf("\tstep 0\n");
			if(inside_target(mouse_pos_x, mouse_pos_y, 360) && GOAL != 0)
			{
//printf("\tinside target\n");
				step = 1;
				timer = glutGet(GLUT_ELAPSED_TIME);
			}
			break;
		// step 1: wait on center target
		//		|-> this state is finished if they have remained in the center for CENTER_HOLD_TIME
		//			|-> if they leave the center then they return to step 0
		case 1://wait on center target
			//printf("Goal is %d\n And the time thing is %d", GOAL, (glutGet(GLUT_ELAPSED_TIME) - timer));
			if(!inside_target(mouse_pos_x, mouse_pos_y, 360) && (GOAL == 2 || GOAL == 3))
			{
				step = 0;
				timer = 0;
			}
			else if(glutGet(GLUT_ELAPSED_TIME) - timer > CENTER_HOLD_TIME_2 && GOAL == 3)
			{
				/*printf("%d)Flag for %d is %d at %d\n", trial%10, order[trial], flag[order[trial]], target_locations[1][order[trial]]);
				if(flag[order[trial]] == 0)
				{
					printf("%d)We have not visited circle %d yet\n", trial%10, order[trial]);
					flag[order[trial]] = 1;

					if(rand() % 2 == 0)
					{
						printf("Chaning rotation degree\n");
						rotation = ROTATION_CW;
						rotation_degree = ROTATION_DEGREE_CW;
						dir = 1;
					}
					else
					{
						printf("Chaning rotation degree\n");
						rotation = ROTATION_CCW;
						rotation_degree = ROTATION_DEGREE_CCW;
						dir = -1;
					}
					r[order[trial]] = rotation;
					rd[order[trial]] = rotation_degree;
				}
				else
				{
					printf("Chaning rotation degree\n");
					printf("%d)We already were here so reversing previous values\n", trial%10);
					rotation = -r[order[trial]];
					rotation_degree = -rd[order[trial]];
					dir *= -1;
				}

				if(trial%20 == 0)
				{
					printf("reset all flags\n");
					flag[5] = 0;
				}
				*/
				//printf("blank\n");
				GOAL = 0;
				timer = glutGet(GLUT_ELAPSED_TIME);
				glutPostRedisplay();// update display
			}
			else if(glutGet(GLUT_ELAPSED_TIME) - timer > BLANK && GOAL == 0)
			{
				/*if(rotation_degree == 30)
					printf("%d)direction is counterclockwise\n", trial%10);
				else
					printf("%d)direction is clockwise\n", trial%10);
				*/
				GOAL = 1;
				trial++;
				timer = glutGet(GLUT_ELAPSED_TIME);
				glutPostRedisplay();// update display
			}
			else if(glutGet(GLUT_ELAPSED_TIME) - timer > FLASH && GOAL == 1)
			{
				//printf("center\n");
				GOAL = 2;
				timer = glutGet(GLUT_ELAPSED_TIME);
				glutPostRedisplay();// update display
			}
			else if(glutGet(GLUT_ELAPSED_TIME) - timer > CENTER_HOLD_TIME && GOAL == 2)
			{
				/*printf("%d)target is %d\n", trial%10, order[trial]);

				if(rotation_degree == 30)
					printf("%d)rotating to the counterclockwise\n\n", trial%10);
				else
					printf("%d)rotating to the clockwise\n\n", trial%10);
				*/

				GOAL = 3;
				// time to move on.
				//printf(" time to move on goal is %d", GOAL);
				step = 2;
				start_outward_x = mouse_pos_x;
				start_outward_y = mouse_pos_y;
				// change the target idx to the actual target.
				target_idx = 360;
				glutPostRedisplay();// update display
				report();
			}
			break;
		// step 2: wait for outward movement
		//		|-> this state is finished when they start moving out to the target
		case 2: // wait for outward movement
			if(mouse_pos_x != start_outward_x || mouse_pos_y != start_outward_y)
			{
				step = 3;
			}
			
			break;
		// step 3: move to target.
		//		|-> this state is finished when they enter the target
		case 3:
			//printf("x: %d\t y: %d\t x: %d\t y: %d\n", mouse_pos_x, mouse_pos_y, cursor_pos_x, cursor_pos_y);
			if(inside_target(mouse_pos_x, mouse_pos_y, trials[0][trial]))
			{
				step = 4;
				timer = glutGet(GLUT_ELAPSED_TIME);
			}
			else if (trials[1][trial] <= 4)
			{
				if(cursor_pos_x <= 520.0 && cursor_pos_y >= 230.33 && cursor_pos_y <= 266.33)
				{
					GOAL = 2;
					obs_hit = 1;
					error++;
					step = -1 * error;
					timer = glutGet(GLUT_ELAPSED_TIME);
					report();
					step = 0;
				}
			}
			else if (trials[1][trial] >= 5)
			{
				if (cursor_pos_x >= 504.0 && cursor_pos_y >= 230.33 && cursor_pos_y <= 266.33)
				{
					GOAL = 2;
					obs_hit = 1;
					error++;
					step = -1 * error;
					timer = glutGet(GLUT_ELAPSED_TIME);
					report();
					step = 0;
				}
			}
			break;
		// step 4: wait on target
		//		|-> this state is finished if they have remained in the center for TARGET_HOLD_TIME
		//			when they are finished holding then we move to the next target
		case 4://wait on outer target
			if(!inside_target(mouse_pos_x, mouse_pos_y, trials[0][trial]))
			{
				// if they
//				step = 3;
				timer = glutGet(GLUT_ELAPSED_TIME);
				//if(GOAL == 4)
				//{
				//	step = 0;
				//	target_idx = 360;
				//	glutPostRedisplay();
				//}
			}
			else if(glutGet(GLUT_ELAPSED_TIME) - timer > TARGET_HOLD_TIME)
			{
				//printf("back to center\n");
				// time to move on.
				step = 0;
				obs_hit = 0;
				//GOAL = 4;
				// change the target idx to the actual target.
				target_idx = 360;
				glutPostRedisplay();
			}
			break;
}
//	printf("idle\n");
//	int start = glutGet(GLUT_ELAPSED_TIME);
//	
//	while(glutGet(GLUT_ELAPSED_TIME) - start < 5000)
//	{
//		printf("wait!!!!\n");
//	};
}

int window;
int main(int argc, char **argv) 
{
	// setup all the trial data.
	char subject[200];
	char file_extension[20]="_3_train_dual.txt";
	int i;
	int j;
	int counter;
	int m;
	int k;
	int l;
	int trial_number;
	int index;
	int index2;
	int temp;
	int temp1;
	int temp2;
	int temp3;
	int s;

	SYSTEMTIME time;
	
	printf("Enter subject code:");
	gets(subject);
	
	// init random number generator with current time
	GetSystemTime(&time);
	srand(time.wMilliseconds);

	//Output file
	strcat(subject,file_extension);
	log_file = fopen(subject, "w");
	if(log_file == NULL)
	{
		printf("Failed to open subject file [%s]\n", subject);
		return -1;
	}
	target_locations[0] = (int *)malloc(sizeof(int) * (NUM_TARGETS+1));	// the x coord
	target_locations[1] = (int *)malloc(sizeof(int) * (NUM_TARGETS+1));	// the y coord
	target_locations[2] = (int *)malloc(sizeof(int) * (NUM_TARGETS+1));	
	target_locations[3] = (int *)malloc(sizeof(int) * (NUM_TARGETS+1));	
	for(i = 0; i < NUM_TARGETS/2; i++)
	{
		// init the x first
		target_locations[0][i] = i;
		target_locations[0][i+5] = i+5;
		target_locations[1][i] = i*15 + 60;
		target_locations[1][i+5] = i*15 + 60;

		// XXX temporary place holder
		//Length of movement in pixel
		target_locations[2][i] = DISTANCE_12;
		target_locations[3][i] = ROTATION_DEGREE_CW;
		target_locations[2][i+5] = DISTANCE_12;
		target_locations[3][i+5] = ROTATION_DEGREE_CCW;
		//printf("Target: %d Angle: %d\n Target: %d Angle: %d\n", target_locations[0][i], target_locations[1][i], target_locations[0][i+5], target_locations[1][i+5]);
	}

	trials[0] = (int *)malloc(sizeof(int) * 360);	
	trials[1] = (int *)malloc(sizeof(int) * 360);
	trials[2] = (int *)malloc(sizeof(int) * 360);
	trials[3] = (int *)malloc(sizeof(int) * 360);
	trials[4] = (int *)malloc(sizeof(int) * 360);
	for (i = 0; i < NUM_REPEATS; i++)
	{
		for (j = 0; j < NUM_TARGETS; j++)
		{
			k = i*10+j;
			//printf("Trial:%d Target: %d Angle: %d\n", k, target_locations[0][j], target_locations[1][j]);
			trials[0][k] = k; 
			trials[1][k] = target_locations[0][j]; 
			trials[2][k] = target_locations[1][j]; 
			trials[3][k] = target_locations[2][j];
			trials[4][k] = target_locations[3][j];
			//printf("Trial:%d Angle: %d Target: %d Rotate: %d\n", trials[0][k], trials[1][k], trials[2][k], trials[3][k]);
			//printf("Target:%d Angle:%d\n", trials[1][k], trials[3][k]);
		} 
	}



	// the center target
	target_locations[0][NUM_TARGETS] = 0; 
	target_locations[1][NUM_TARGETS] = 0;


	trial_number = 0;
	//Initializing all trials' targets while randomizing
	for(m = 0; m < NUM_REPEATS ; m++)
	{	//Randomizing
		for(j = 0; j < 2500 ; j++)
		{
			// pick two random index between [0..NUM_TARGETS-1]
			index = (rand() % NUM_TARGETS) + (m * 10);
			index2 = (rand() % NUM_TARGETS) + (m * 10);
			// flip order[i] and order[flip_with]
			temp = trials[1][index];
			temp1 = trials[2][index];
			temp2 = trials[3][index];
			temp3 = trials[4][index];
			trials[1][index] = trials[1][index2];
			trials[2][index] = trials[2][index2];
			trials[3][index] = trials[3][index2];
			trials[4][index] = trials[4][index2];
			trials[1][index2] = temp;
			trials[2][index2] = temp1;
			trials[3][index2] = temp2;
			trials[4][index2] = temp3;
			//printf("Index: %d Index2: %d\n", index, index2);
			//printf("temp1: %d temp2: %d temp3: %d\n", temp1, temp2, temp3);
		}
		//Intitilizing without repetiotion.Each target selected once
		//for (s = 0; s < NUM_TARGETS; s++)
		//{
		//	order[trial_number] = temp_order[s];
		//	trial_number++;
		//}
	}

	for (i = 0; i < NUM_REPEATS; i++)
	{
		for (j = 0; j < NUM_TARGETS; j++)
		{
			k = i*10+j;
			printf("Trial:%d Target: %d Angle: %d Dist: %d Rotate: %d\n", trials[0][k], trials[1][k], trials[2][k], trials[3][k], trials[4][k]);
		}
	}


	trial = -1;
	step = 1;
	mouse_pos_x = 0;
	mouse_pos_y = 0;
	temp_mouse_x = 0;
	temp_mouse_y = 0;
	cursor_pos_x = 0;
	cursor_pos_y = 0;
	target_idx = 360; 
	result = 0;
	
	counter = ShowCursor(FALSE);
	while (counter>=0) 
	{
		counter = ShowCursor(FALSE);
	}


    glutInit(&argc, argv);  
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  

// initialize fullscreen mode: 1024 by 768 with 16 bit pixel depth and 60 hurts refresh rate
    glutGameModeString( "1024x768:24@60" );
	// start fullscreen game mode
    glutEnterGameMode();
    glutInitWindowSize(1024, 768);  
//    // the window starts at the upper left corner of the screen
//    glutInitWindowPosition(0, 0);  
//    // Open a window
//    window = glutCreateWindow("temp");  
	
	InitGL(1024, 768);
    
	glutDisplayFunc(&DrawGLScene);  

    // Register the function called when the keyboard is pressed.
    glutKeyboardFunc(&keyPressed);
	glutMouseFunc(&mousePressed);
	glutPassiveMotionFunc(&mouseMoved);
	glutIdleFunc(&idle);

    /* Start Event Processing Engine */  
    glutMainLoop();  


	return 1;
}