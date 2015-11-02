#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <list>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>

#define ROWS 270 
#define PI 3.1415
#define COLS 480 
#define RANGE 140
#define PI 3.1415
using namespace std;

typedef struct player
{
        int x;
        int y;
        int m=1;        //mobility
        int s=5;        //strength
        int c;
}player;

player team[6];

//Initialize players

int footx,footy;        	//coordinates of the ball
int control=-1;                 //controlling player
int goalA=0,goalB=0;        	//Goal count
int half = 1;			//First half


class cell
{
public:
    int r = 0, g = 110, b = 0;
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

void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius)
{
        int i;
        int triangleAmount = 20; //# of triangles used to draw circle
        //GLfloat radius = 0.8f; //radius
        GLfloat twicePi = 2.0f * PI;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // center of circle
        for(i = 0; i <= triangleAmount;i++)
         {
                glVertex2f(
                x + (radius * cos(i * twicePi / triangleAmount)),
                y + (radius * sin(i * twicePi / triangleAmount))
                );
        }
glEnd();
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
            if( (i == ROWS/2-60 || i == ROWS/2+60) && ( (j >= 2 && j <= 20) || (j <
COLS-2 && j > COLS-21) ) )
                glColor3ub( 255, 255, 255 );
            if( (i > ROWS/2-60 && i < ROWS/2+60) && ( j == 2 || j == COLS-3) )
                glColor3ub( 255, 255, 255 );
            if( (i > ROWS/2-80 && i < ROWS/2+80) && ( j == 90 || j == COLS-90) )
                glColor3ub( 255, 255, 255 );
            if( (i == ROWS/2-80 || i == ROWS/2+80) && ( (j >= 20 && j <= 90) || (j <
COLS-20 && j >= COLS-90) ) )
                glColor3ub( 255, 255, 255 );


            if( fabs((double)(i-ROWS/2)*(i-ROWS/2) + (double)(j-COLS/2)*(j-COLS/2) -
900.0) <= 35.0  )
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

void GoalKeeper(int a)        //function for moving the goal keeper in bot mode
{        
        glutPostRedisplay();
        glutTimerFunc(3000, GoalKeeper, 0);
	
	int i;
	
	//Which half is the ball now in ?
	if(footx<240)
	   i = 1;	//ball is in the first half
	else
	   i = 2;	//ball is in the second half
	
	
        if(footy>135)
	{
	   if(i==1)
	      team[0].y = min(footy,195);
           else
	      team[3].y = min(footy,195);
        }
	else
        {
	   if(i==1)
	      team[0].y = max(footy,70);
	   else
	      team[3].y = max(footy,70);
	}        
}

void Goal()
{
        if((footx>=456)&&(footy<=195)&&(footy>=70))
        {
		if(half==1)
                   goalA++;
		else
		   goalB++;
		Initialize();
        }
        if((footx<=23)&&(footy<=195)&&(footy>=70))
        {
                if(half==1)
                   goalB++;
		else
		   goalA++;
                Initialize();
        }
        
        //cout<<"Goals:-"<<goalA<<goalB<<endl;
}

int dist(int x1, int y1, int x2, int y2)
{
        int d = pow((x1-x2),2) + pow((y1-y2),2);
        return d;
}


int CheckCollision(int p, int i, int j)
{
	//Will moving to (i,j) cause collison for player 'p'? 
	int k,d;
	for(k=0;k<6;++k)    
	{
		if(k==p)
		   continue;
		d = dist(i,j,team[k].x,team[k].y);
		if(d<100)
		   return 0;	//Collision
	}
	return 1;		//No collision
}



void SpecialKeys(int key,int a,int b)
{
        switch(key)
        {
                case(GLUT_KEY_UP):
                {
                        footy--;
                        if( (team[1].y-3>=10) && (CheckCollision(1,team[1].x,team[1].y-3)) )
                           team[1].y -= 3;
                        break;
                }
                case(GLUT_KEY_DOWN): 
                {
                        footy++;                
                        if(((team[1].y+3)<=255) && (CheckCollision(1,team[1].x,team[1].y+3)) )
                           team[1].y+=3;
                        break;
                }
                case(GLUT_KEY_LEFT):
                {        footx--;
                        if(((team[1].x-3)>=25) && (CheckCollision(1,team[1].x-3,team[1].y)) )
                           team[1].x -= 3;
                        break;
                }
                case(GLUT_KEY_RIGHT):
                {        footx++;
                        if(((team[1].x+3)<=450) && (CheckCollision(1,team[1].x+3,team[1].y)) )
                           team[1].x += 3;
                        break;
                }        
        }        //End of switch case
}        //end of function SpecialKeys




void Control()        ///Arun
{
        int i,p=0,temp=0;
        int d,mind=INT_MAX;
        for(i=0;i<6;i++)
        {
           d = dist(footx,footy,team[i].x,team[i].y);
           if(d<RANGE)
           {
                temp = 1;
             //cout<<"Player "<<i<<" has ball in his range. d = "<<d<<endl ;
             if(d<mind)
             {
                mind = d;
                p = i;
             }
             if(d==mind)        //tie-break;
             {
                if(team[p].s<team[i].s)
                   p = i;
                else
                {
                        //distance and strength are same.
                        int tb = rand()%2;
                        if(tb==1)
                           p = i;
                }
             }
           }
        }
        
        if(temp==1)
           control = p;
        else
           control = -1;

	if(control%3==0)
	        cout<<control<<endl;
}


void PersonDraw()
{
        int i;
        //cout<<team[0].x<<" - a"<<endl;
        for(i=0;i<=2;i++)
        {
		if(half==1)
                   glColor3f(1.0f,0.0f,1.0f);
                else
		   glColor3f(1.0f,1.0f,0.0f);
	 	drawFilledCircle(team[i].x,team[i].y,5);
                team[i].c=0;
        }
        for(i=3;i<=5;i++)
        {
 		if(half==1)
                   glColor3f(1.0f,1.0f,0.0f);
                else
		   glColor3f(1.0f,0.0f,1.0f);
		drawFilledCircle(team[i].x,team[i].y,5);
                team[i].c=0;
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
        Control();
        glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
        switch(key)
        {
                case('w'):
                {
                         team[4].y -= 3;
                         break;
                }
                case('s'): 
                {
                         team[4].y+=3;
                         break;
                }
                case('a'):
                {
                         team[4].x -= 3;
                         break;
                }
                case('d'):
                {
                         team[4].x += 3;
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
                        footx = team[4].x;
                        footy = team[4].y;
                        break;
                }
                default:
                        exit(0);
                        break;
        }        //end of switch
}        //end of fn

void Initialize()
{
        //Set the ball
        
        footx = 240;
        footy = 135;        

        //Set team A players
        team[0].x =  40;        //goal keeper  
        team[0].y =  135; 
        team[1].x =  200; 
        team[1].y =  15;
        team[2].x =  200;              
        team[2].y =  135;

        //Set team B players
        team[3].x =  430;	//goal keeper
        team[3].y =  135;
        team[4].x =  280;
        team[4].y =  255;
        team[5].x =  280;        
        team[5].y =  135;

	//Set their strengths and mobilities
	team[1].s = 10;
	team[4].s = 10;
	team[2].m = 3;
	team[5].m = 3;
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
