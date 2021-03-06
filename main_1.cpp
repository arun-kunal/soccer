#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <list>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#define ROWS 270 
#define PI 3.1415
#define COLS 480 

using namespace std;

typedef struct player
{
	int x;
	int y;
	int m;	//mobility
	int s;	//strength
	int c;	//control
}player;

player teamA[3];
player teamB[3];

int footx,footy;
int goalA=0,goalB=0;

class cell
{
public:
    int r = 0, g = 150, b = 0;
};

cell map[ROWS][COLS];

void Initialize(void);

void init(void)
{
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);
    glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
}

void drawBitmapText(char *string,float x,float y) 
{
  char *c;
  glRasterPos2f(x, y); 
  int l=1; 
  for (c=string; (*c != ' '&&l<=5) ; c++,l++)
  {
	 glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
  }
}
void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glOrtho ( 0.0, COLS, ROWS, 0.0, 1.5, 200.0);
   glMatrixMode (GL_MODELVIEW);
}

//    don't interfere with this function, it renders the map on the display
void generate_map() {
    for( int i = 0; i < ROWS; i++ ) {
        for( int j = 0; j < COLS; j++ ) {
            cell &c = map[i][j];

            if( i % 2 && j % 2 )
                glColor3ub( c.r, c.g, c.b );
            else if( j % 2 )
                glColor3ub( c.r, c.g, c.b );
            else if( i % 2 )
                glColor3ub( c.r, c.g, c.b );
            else
                glColor3ub( c.r, c.g, c.b );

            if( (i == 5 || i == ROWS-6) && (j > 20 && j < COLS-21) )
                glColor3ub( 255, 255, 255 );
            if( (j == 20 || j == COLS-21) && (i >= 5 && i <= ROWS-6) )
                glColor3ub( 255, 255, 255 );
            if( j == COLS/2 && (i >= 5 && i <= ROWS-6) )
                glColor3ub( 255, 255, 255 );
            if( (i == ROWS/2-60 || i == ROWS/2+60) && ( (j >= 2 && j <= 20) || (j < COLS-2 && j > COLS-21) ) )
                glColor3ub( 255, 255, 255 );
            if( (i > ROWS/2-60 && i < ROWS/2+60) && ( j == 2 || j == COLS-3) )
                glColor3ub( 255, 255, 255 );
            if( (i > ROWS/2-80 && i < ROWS/2+80) && ( j == 90 || j == COLS-90) )
                glColor3ub( 255, 255, 255 );
            if( (i == ROWS/2-80 || i == ROWS/2+80) && ( (j >= 20 && j <= 90) || (j < COLS-20 && j >= COLS-90) ) )
                glColor3ub( 255, 255, 255 );


            if( fabs((double)(i-ROWS/2)*(i-ROWS/2) + (double)(j-COLS/2)*(j-COLS/2) - 900.0) <= 35.0  )
                glColor3ub( 255, 255, 255 );
            glBegin(GL_QUADS);
                glVertex3f(   j,   i, 0);
                glVertex3f( j+1,   i, 0);
                glVertex3f( j+1, i+1, 0);
                glVertex3f(   j, i+1, 0);
            glEnd();
        }
    }
}
void DrawCircle(float cx,float cy,float r ,int num_segments)
{
	glBegin(GL_LINE_LOOP);
	glColor3ub( 1, 0, 0 );
	for(int ii=0;ii<num_segments;ii++)
	{
		float theta=2.0f*3.1415926f * float(ii)/float(num_segments);//get the current angle
		float x=r*cos(theta);
		float y=r*sin(theta);
		glVertex2f(x+cx,y+cy);

	}
	glEnd();
}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity ();
    gluLookAt (0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    generate_map();
    //DrawCircle(20,20,5,100);
    glutSwapBuffers();
    glutPostRedisplay();
}

void GoalKeeper(int a)	//function for moving the goal keeper in bot mode
{	
	//teamA[2].x
	glutPostRedisplay();
	glutTimerFunc(3000, GoalKeeper, 0);
	if(footy>135)
	   teamA[2].y = min(footy,195);
	else
	   teamA[2].y = max(footy,70);
	
}

void Goal()
{
	if((footx>=456)&&(footy<=195)&&(footy>=70))
	{
		goalA++;
		Initialize();
	}
	if((footx<=23)&&(footy<=195)&&(footy>=70))
	{
		goalB++;
		Initialize();
	}
	
	//cout<<"Goals:-"<<goalA<<goalB<<endl;
}


void SpecialKeys(int key,int x,int y)
{
	switch(key)
	{
    		case(GLUT_KEY_UP):
		{
			footy--;
			if((teamA[0].y-3)>=10)
			   teamA[0].y -= 3;
			break;
		}
    		case(GLUT_KEY_DOWN): 
		{
			footy++;		
			if((teamA[0].y-3)<=250)
			   teamA[0].y+=3;
			break;
		}
    		case(GLUT_KEY_LEFT):
		{	footx--;
			if((teamA[0].x-3)>=10)
			   teamA[0].x -= 3;
			break;
		}
    		case(GLUT_KEY_RIGHT):
		{	footx++;
			if((teamA[0].x-3)<=440)
			   teamA[0].x += 3;
			break;
		}	
	}	//End of switch case
}	//end of function SpecialKeys

float dist(int x1, int y1, int x2, int y2)
{
	float d = pow((x1-x2),2) + pow((y1-y2),2);
	return d;
}


void Control()	///Arun
{
	
}


void PersonDraw()
{
	int i;
	//cout<<teamA[0].x<<" - a"<<endl;
	for(i=0;i<=2;i++)
	{
		glColor3f(1.0f,0.0f,1.0f);	
            	glBegin(GL_TRIANGLES);
		glVertex3f(teamA[i].x,teamA[i].y,0);
                glVertex3f(teamA[i].x+20,teamA[i].y,0);
                glVertex3f(teamA[i].x+10,teamA[i].y+10,0);
	   	glEnd();
		teamA[i].c = 0;	//control mode
	}
	//glFlush();
	//cout<<teamB[0].x<<" - b"<<endl;
	for(i=0;i<=2;i++)
	{
		glColor3f(1.0f,1.0f,0.0f);	
            	glBegin(GL_TRIANGLES);
		glVertex3f(teamB[i].x,teamB[i].y,0);
                glVertex3f(teamB[i].x+20,teamB[i].y,0);
                glVertex3f(teamB[i].x+10,teamB[i].y+10,0);
	   	glEnd();
		teamB[i].c = 0;	//control mode
	}
	//glFlush();
}

void BallDraw()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glColor3f(1,1,1);
	glTranslated(footx,footy,0);
	glutSolidSphere(5,50,50);
	glPushMatrix();
	//glFlush();
}



void Draw()
{
	//cout<<"Hi";
	//++footy;
	PersonDraw();
 	BallDraw();
	GoalKeeper(3000);
	Goal();
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case('w'):
		{
			teamB[0].y -= 3;
			teamB[0].x+=0;
			break;
		}
		case('s'): 
		{
			teamB[0].y+=3;
			teamB[0].x+=0;
			break;
		}
		case('a'):
		{
			teamB[0].y+=0;
			teamB[0].x -= 3;
			break;
		}
		case('d'):
		{
			 teamB[0].x += 3;
			 teamB[0].y+=0;
			 break;
		}	
                case('c'): 
		{
			cout<<"x - "<<footx<<endl;
			cout<<"y - "<<footy<<endl;
			break;
		} 
		case('0'):
		{
			footx = teamB[1].x;
			footy = teamB[1].y;
			break;
		}
		default:
			exit(0);
			break;
	}	//end of switch
}	//end of fn

void Initialize()
{
	//Set the ball
	
	footx = 240;
	footy = 135;	

	//Set team A players
	teamA[0].x =  200;	
	teamA[0].y =  130; 
	teamA[1].x =  200; 
	teamA[1].y =  10;
	teamA[2].x =  30;	//goal keeper	
	teamA[2].y =  130;

	//Set team B players
	teamB[0].x =  280;
	teamB[0].y =  130;
	teamB[1].x =  280;
	teamB[1].y =  240;
	teamB[2].x =  430;	//goal keeper
	teamB[2].y =  130;
}


int main(int argc, char** argv)
{

	Initialize(); 
	while(1)
	{
		glutInit(&argc, argv);
		glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
		glutInitWindowPosition (0, 0);
		glutCreateWindow (argv[0]);
		glutFullScreen();
		init ();
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);
		glutIdleFunc(Draw);
		glutSpecialFunc(SpecialKeys);
		glutTimerFunc(3000, GoalKeeper, 0);
		// sleep(1000);
		glutSwapBuffers();
		glutMainLoop();
	}
    	return 0;
}
