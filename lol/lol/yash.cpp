#include<stdlib.h>
#include<GL/glut.h>
#include<stdio.h>
#include<math.h>
#include<iostream>
#include "LIB/glm/glm.h"
using namespace std;
#define pi 3.14

float checkx=0,checky=0, checkz=0;
float thetay[2]={0,180}, carx[2]={63,63}, carz[2]={0,-30}, boxspeed[2]={0.01,0.01}, boxtrans[2]={0,0.3}, speed[2]={0,0};
float turn[2]={0,0}, wheely[2]={0,0}, steerangle[2]={0,0};
float helD=10, heltheta=180, helfi=70, pressx=0, pressy=0;
float prevx[2]={0,0}, prevz[2]={0,0};
float ambI, userAMB=0, sunangle=268, sunangle_temp=268, yyy=0, zzz=0;
GLfloat xxx[]={0};
int camMode[2]={0,0}, UP[2]={0,0}, DOWN[2]={0,0}, RIGHT[2]={0,0}, LEFT[2]={0,0}, lightangle[2]={0,30}, wheelangle[2]={0,0}, press=0, coll[2]={0,0};
int shadeFlag=0, headlightFlag[2]={0,0}, torchFlag[2]={0,0}, I=0, collision_flag[200][400], stopSunAuto=0, windows=1;
GLuint tex_2d;
GLfloat e0[] = {0,0,0,1};
GLMmodel *b[3], *Xroad, *model, *Troad, *Sroad, *frontl, *frontr, *backl, *backr, *steer, *terrain, *model2, *palm, *chair, *house[8], *lih, *body2;


float mymod(float x)
{
	return (x<0?-x:x);
}

int zeroS(float &s)
{
	float x = s;
	x = (x<0 ? -x : x);
	if(x < 0.0001)
	{
		s = 0.0;
		return 1;
	}
	return 0;
}

void temp(float a,float b,float c)
{
	glBindTexture(GL_TEXTURE_2D,0);
	checkx = a;
	checky = b;
	checkz = c;

	glPushMatrix();
	glTranslatef(checkx,checky,checkz);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0,1,0);
	for(int i=0;i<300;i++)
	{   
		glVertex3f(0.5*cos(i*(pi/180)), 0, 0.5*sin(i*(pi/180)));
	} 
	glEnd();

	glColor3f(1,0,1);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(0,0,1);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glEnd();

	glPopMatrix();
}

void collisionx(float x, float z1, float z2)
{
	for(int i=z1-2;i<=z2+2;i++)
	{
		for(int j=-2;j<3;j++)
		{
			collision_flag[(int)(100+x+j)][(int)(200+i)]=1;
		}
	}
}

void collisionz(float x1, float x2, float z)
{
	for(int i=x1-2;i<=x2+2;i++)
	{
		for(int j=-2;j<3;j++)
		{
			collision_flag[(int)(100+i)][(int)(200+j+z)]=1;
		}
	}
}

void initRender()
{
	
	for(int i=0; i<200; i++)
		for(int j=0;j<400;j++)
			collision_flag[i][j]=0;

	glEnable(GL_NORMALIZE);

	terrain = glmReadOBJ("hell.obj");
	body2 = glmReadOBJ("body2.obj");
	model = glmReadOBJ("body.obj");
	frontl = glmReadOBJ("frontLeft.obj");
	frontr = glmReadOBJ("frontRight.obj");
	backl = glmReadOBJ("backLeft.obj");
	backr = glmReadOBJ("backRight.obj");
	steer = glmReadOBJ("steering.obj");
	Sroad = glmReadOBJ("st.obj");
	Troad = glmReadOBJ("T.obj");
	Xroad = glmReadOBJ("X.obj");
	if(windows)
	{
		palm = glmReadOBJ("pt1.obj");
		chair = glmReadOBJ("ch.obj");
	
		house[1] = glmReadOBJ("house.obj");
		house[2] = glmReadOBJ("h2.obj");
		house[0] = glmReadOBJ("Bld_16.obj");
		house[3] = glmReadOBJ("h3.obj");
		house[4] = glmReadOBJ("h4.obj");
	}
	
	/*collisionx(-89.5-2, (-14.5+(-40*3.4)), (40*3.4));
	collisionx(-75+2, (-14.5+(-40*3.4)), -14.5-1);
	collisionx(-75+2, (3.4+1), (40*3.4));

	collisionx(55.5-2, (-14.5-1+(-45*3.4)), -14.5);
	collisionx(55.5-2, (3.4), (45*3.4));
	collisionx(70+2, (-14.5-1+(-45*3.4)), -14.5);
	collisionx(70+2, (3.4), (45*3.4));

	collisionz(-73, 53.5, 3.4);
	collisionz(-73, 53.5, -14.5-3);

	collisionz(72, 94, 3.4);
	collisionz(72, 94, -14.5-3);*/
}


inline float calcambI(float x)
{
	x += 90;
	if(x>=0 && x<180)
		return (0.3 + ((x*0.7)/180.0));
	if(x>=180 && x<=360)
		return (0.3 + (((360-x)*0.7)/180));
}

inline void letThereBeLight()
{
	if(stopSunAuto == 0)
	{
		sunangle_temp += 0.5;
		sunangle = (int)sunangle_temp;
	}
	if(sunangle > 269)
	{
		sunangle = -90 + sunangle - 270;
		sunangle_temp = sunangle;
	}
	if(sunangle < -90)
	{
		sunangle = 270 + 90 + sunangle;
		sunangle_temp = sunangle;
	}
	ambI = calcambI(sunangle);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	GLfloat ambientColor[] = {userAMB+ambI,userAMB+ambI,userAMB+ambI,1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	GLfloat lightPos0[] = {500*cos(sunangle*(pi/180)),500*sin(sunangle*(pi/180)), 0, 1.0f};
	GLfloat lightColor0[] = {0.8, 0.8, 0.8, 1.0f}; 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0);

	GLfloat lightColor1[] = {0.7f, 0.3f, 0.3f, 1.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos0);
	
	GLfloat e1[] = {1,1,0,1};
	glDisable(GL_TEXTURE_2D);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, e1);
	glMaterialfv(GL_FRONT, GL_EMISSION, e1);
	glPushMatrix();
	glTranslatef(lightPos0[0],lightPos0[1],lightPos0[2]);
	glutSolidSphere(10, 32, 32);
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, e0);
}

inline void headlight()
{
	for(int i=0; i<2; i++)
	{
		if(headlightFlag[i] == 0)
			continue;

		float x1=0.65, y1=0.11, z1=1.5;

		(i == 0) ? glEnable(GL_LIGHT2) : glEnable(GL_LIGHT5);
		float alpha = atan(-x1/z1);
		float tyreD = sqrt((x1)*(x1) + (z1)*(z1));
		float tyrex = carx[i] + tyreD*sin((thetay[i])*(pi/180)+alpha), tyrez = carz[i] - tyreD*cos((thetay[i])*(pi/180)+alpha);
		float dirx = sin((thetay[i])*(pi/180)), dirz = -cos((thetay[i])*(pi/180));

		GLfloat lightPos2[] = {tyrex,y1,tyrez,1};
		GLfloat lightDir2[] = {dirx,0,dirz};
		GLfloat lightColor2[] = {0,1,0,1}; 
		(i==0) ? glLightfv(GL_LIGHT2, GL_DIFFUSE, lightColor2) : glLightfv(GL_LIGHT5, GL_DIFFUSE, lightColor2);
		(i==0) ? glLightfv(GL_LIGHT2, GL_POSITION, lightPos2) : glLightfv(GL_LIGHT5, GL_POSITION, lightPos2);
		(i==0) ? glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lightDir2) : glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, lightDir2);
		(i==0) ? glLighti(GL_LIGHT2, GL_SPOT_CUTOFF,10) : glLighti(GL_LIGHT5, GL_SPOT_CUTOFF,15);
		(i==0) ? glLighti(GL_LIGHT2, GL_SPOT_EXPONENT,127) : glLighti(GL_LIGHT5, GL_SPOT_EXPONENT,127);


		GLfloat e1[] = {0,1,0,1};
		glDisable(GL_TEXTURE_2D);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, e1);
		glMaterialfv(GL_FRONT, GL_EMISSION, e1);
		glPushMatrix();
		glTranslatef(tyrex,y1,tyrez);
		glRotatef(-thetay[i],0,1,0);
		glTranslatef(0,0,-0.1);
		glutSolidSphere(0.04, 12, 12);
		glTranslatef(0.13,0,-0.04);
		glutSolidSphere(0.047, 12, 12);
		glTranslatef(-0.24,0,0.05);
		glutSolidSphere(0.03, 12, 12);
		glPopMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, e0);

		(i==0) ? glEnable(GL_LIGHT3) : glEnable(GL_LIGHT6);
		alpha = atan(x1/z1);
		tyrex = carx[i] + tyreD*sin((thetay[i])*(pi/180)+alpha), tyrez = carz[i] - tyreD*cos((thetay[i])*(pi/180)+alpha);

		GLfloat lightPos3[] = {tyrex,y1,tyrez,1};
		GLfloat lightDir3[] = {dirx,0,dirz};
		GLfloat lightColor3[] = {0,0,1,1}; 
		(i==0) ? glLightfv(GL_LIGHT3, GL_DIFFUSE, lightColor3) : glLightfv(GL_LIGHT6, GL_DIFFUSE, lightColor3);
		(i==0) ? glLightfv(GL_LIGHT3, GL_POSITION, lightPos3) : glLightfv(GL_LIGHT6, GL_POSITION, lightPos3);
		(i==0) ? glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, lightDir3) : glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, lightDir3);
		(i==0) ? glLighti(GL_LIGHT3, GL_SPOT_CUTOFF,10) : glLighti(GL_LIGHT6, GL_SPOT_CUTOFF,15);
		(i==0) ? glLighti(GL_LIGHT3, GL_SPOT_EXPONENT,127) : glLighti(GL_LIGHT6, GL_SPOT_EXPONENT,127);

		e1[0]=0, e1[1]=0, e1[2]=1;
		glDisable(GL_TEXTURE_2D);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, e1);
		glMaterialfv(GL_FRONT, GL_EMISSION, e1);
		glPushMatrix();
		glTranslatef(tyrex,y1,tyrez);
		glRotatef(-thetay[i],0,1,0);
		glTranslatef(0,0,-0.1);
		glutSolidSphere(0.04, 12, 12);
		glTranslatef(-0.13,0,-0.04);
		glutSolidSphere(0.047, 12, 12);
		glTranslatef(0.24,0,0.05);
		glutSolidSphere(0.03, 12, 12);
		glPopMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, e0);
	}
}

inline void torchlight()
{
	for(int i=0; i<2; i++)
	{
		if(torchFlag[i] == 0)
			continue;
		float eyex = carx[i] - boxtrans[i]*cos(thetay[i]*(pi/180));
		float eyez = carz[i] - boxtrans[i]*sin(thetay[i]*(pi/180));
		float eyey = 0.9;
		float dirx = sin((thetay[i]-lightangle[i])*(pi/180));
		float dirz = -cos((thetay[i]-lightangle[i])*(pi/180));
		float diry = 0;

		(i==0) ? glEnable(GL_LIGHT4) : glEnable(GL_LIGHT4);
		GLfloat lightPos4[] = {eyex,eyey,eyez,1};
		GLfloat lightDir4[] = {dirx,diry,dirz};
		GLfloat lightColor4[] = {1,0,0,1}; 
		(i==0) ? glLightfv(GL_LIGHT4, GL_DIFFUSE, lightColor4) : glLightfv(GL_LIGHT7, GL_DIFFUSE, lightColor4);
		(i==0) ? glLightfv(GL_LIGHT4, GL_POSITION, lightPos4) : glLightfv(GL_LIGHT7, GL_POSITION, lightPos4);
		(i==0) ? glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, lightDir4) : glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, lightDir4);
		(i==0) ? glLighti(GL_LIGHT4, GL_SPOT_CUTOFF,5) : glLighti(GL_LIGHT7, GL_SPOT_CUTOFF,5);
		(i==0) ? glLighti(GL_LIGHT4, GL_SPOT_EXPONENT,127) : glLighti(GL_LIGHT7, GL_SPOT_EXPONENT,127);

		GLfloat e1[] = {1,0,0,1};
		glDisable(GL_TEXTURE_2D);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, e1);
		glMaterialfv(GL_FRONT, GL_EMISSION, e1);
		glPushMatrix();
		glTranslatef(eyex,eyey,eyez);
		glRotatef(lightangle[i]-thetay[i],0,1,0);
		glTranslatef(0,0.1,-0.1);
		glutSolidSphere(0.05, 12, 12);
		glPopMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, e0);
	}
}

inline void letTherebeDark()
{
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);
	glDisable(GL_LIGHTING);
}

inline void torchbase(int x)
{
	GLUquadricObj *b = gluNewQuadric();
	gluQuadricDrawStyle(b, GLU_FILL);
	glColor3f(0.7,0.5,0);
	glPushMatrix();
	glTranslatef(0.1, 0.8, 0);
	glRotatef(-90,1,0,0);
	gluCylinder(b,0.04,0.04,0.2,12,12);
	glPopMatrix();
}

inline void torch(int x)
{
	GLUquadricObj *light = gluNewQuadric();
	gluQuadricDrawStyle(light, GLU_FILL);

	glColor3f(165/255.0,42/255.0,42/255.0);
	glPushMatrix();
	glTranslatef(0.1,0.9, 0);
	glRotatef(-90+lightangle[x],0,1,0);
	gluCylinder(light,0,0.1,0.15,12,12);
	glPopMatrix();

	lightangle[x] = (lightangle[x] + 2 + 360)%360;
}

inline void sliderontop(int x)
{
	glPushMatrix();
	glColor3f(0,0,0);
	glTranslatef(0,0.7,0);

	glBegin(GL_QUADS);
	glVertex3f(-0.05,   0,0.5);
	glVertex3f( 0.05,   0,0.5);
	glVertex3f( 0.05, 0.1,0.5);
	glVertex3f(-0.05, 0.1,0.5);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(-0.05,   0,-0.5);
	glVertex3f( 0.05,   0,-0.5);
	glVertex3f( 0.05, 0.1,-0.5);
	glVertex3f(-0.05, 0.1,-0.5);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(-0.05,   0, 0.5);
	glVertex3f(-0.05, 0.1, 0.5);
	glVertex3f(-0.05, 0.1,-0.5);
	glVertex3f(-0.05,   0,-0.5);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(0.05,   0, 0.5);
	glVertex3f(0.05, 0.1, 0.5);
	glVertex3f(0.05, 0.1,-0.5);
	glVertex3f(0.05,   0,-0.5);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(-0.05, 0.1, 0.5);
	glVertex3f(0.05, 0.1, 0.5);
	glVertex3f(0.05, 0.1,-0.5);
	glVertex3f(-0.05, 0.1,-0.5);
	glEnd();

	glPopMatrix();
}

inline void box(int x)
{
	glPushMatrix();

	glTranslatef(-0.1,0.1,boxtrans[x]);

	if(boxtrans[x] >= 0.4)
		boxspeed[x] = -0.01;
	else if(boxtrans[x] <= -0.4)
		boxspeed[x] = 0.01;

	boxtrans[x] += boxspeed[x];

	glColor3f(119/255.0,136/255.0,153/255.0);
	glBegin(GL_QUADS);
	glVertex3f(  0, 0.6,0.1);
	glVertex3f(0.2, 0.6,0.1);
	glVertex3f(0.2, 0.8,0.1);
	glVertex3f(  0, 0.8,0.1);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(  0, 0.6,-0.1);
	glVertex3f(0.2, 0.6,-0.1);
	glVertex3f(0.2, 0.8,-0.1);
	glVertex3f(  0, 0.8,-0.1);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(  0, 0.8, 0.1);
	glVertex3f(  0, 0.8,-0.1);
	glVertex3f(0.2, 0.8,-0.1);
	glVertex3f(0.2, 0.8, 0.1);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(  0, 0.6, 0.1);
	glVertex3f(  0, 0.6,-0.1);
	glVertex3f(0.2, 0.6,-0.1);
	glVertex3f(0.2, 0.6, 0.1);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(  0, 0.6, 0.1);
	glVertex3f(  0, 0.6,-0.1);
	glVertex3f(  0, 0.8,-0.1);
	glVertex3f(  0, 0.8, 0.1);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(0.2, 0.6, 0.1);
	glVertex3f(0.2, 0.6,-0.1);
	glVertex3f(0.2, 0.8,-0.1);
	glVertex3f(0.2, 0.8, 0.1);
	glEnd();

	torchbase(x);
	torch(x);

	glPopMatrix();	
}

inline void wheels(int x)
{
	glPushMatrix();
	glTranslatef(0.841,-0.118,1.113);
	glRotatef(wheely[x],0,1,0);
	glRotatef(wheelangle[x],1,0,0);
	glmDraw(frontl, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.841,-0.118,1.113);
	glRotatef(wheely[x],0,1,0);
	glRotatef(wheelangle[x],1,0,0);
	glmDraw(frontr, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.831,-0.134,-1.578);
	glRotatef(wheelangle[x],1,0,0);
	glmDraw(backl, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.831,-0.134,-1.578);
	glRotatef(wheelangle[x],1,0,0);
	glmDraw(backr, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
	glPopMatrix();
}

inline void steering(int x)
{
	glPushMatrix();
	glTranslatef(0.389,0.251,0.32);
	glRotatef(14,1,0,0);
	glRotatef(-steerangle[x],0,0,1);
	glRotatef(-14,1,0,0);
	glmDraw(steer, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
	glPopMatrix();
}

inline void car(int x)
{
	
	glRotatef(-180,0,1,0);
	if(x==0)
		glmDraw(model, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
	else
		glmDraw(body2, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
	wheels(x);
	steering(x);
	
	glRotatef(90,0,1,0);
	sliderontop(x);
	box(x);
}

inline void carPut()
{
	for(int i=0; i<2; i++)
	{
		glPushMatrix();
		glTranslatef(carx[i],0,carz[i]);
		glRotatef(-thetay[i],0,1,0);
		car(i);         // warning push pop not there in car()
		glPopMatrix();
	}
}

void detect_collision()
{
	float x,z,xp,zp;
	for(int i=0; i<2; i++)
	{
		x=carx[i];
		z=carz[i];
		xp=prevx[i];
		zp=prevz[i];
		if(x<-65.94 && zp>=-100 && zp<=82 && xp>=-65.94)
		{
			x = -65.94 + 0.5; speed[i] /= 2;
		}
		if(x>-58.45 && xp<=-58.45 && zp>-100 && zp<=82 && (zp<=-10.83 || zp>=-3.38))
		{
			x = -58.45 - 0.5; speed[i] /= 2;
		}
		if(x<59.09 && xp>=59.09 && zp>=-171.03 && zp<=153 && (zp>=-3.38 || zp<=-10.83))
		{
			x = 59.09 + 0.5; speed[i] /= 2;
		}
		if(x>66.52 && xp<=66.52 && zp>=-171.03 && zp<=153 && (zp>=-3.38 || zp<=-10.83))
		{
			x = 66.52 - 0.5; speed[i] /= 2;
		}
		if(z<-10.83 && zp>=-10.83 && xp<=102.3 && (xp>=66.52 || xp<=59.09) && xp>=-58.45)
		{
			z = -10.83 + 0.5; speed[i] /= 2;
		}
		if(z>-3.38 && zp<=-3.38 && xp<=102.3 && (xp>=66.52 || xp<=59.09) && xp>=-58.45)
		{
			z = -3.38 - 0.5; speed[i] /= 2;
		}
		carx[i]=x;
		carz[i]=z;
		
	}	
	/*
	int mul;
	for(int i=0;i<2;i++)
	{
	//	float c = cos(thetay[i]*(pi/180));
		mul = 0;
		if(collision_flag[(int)(carx[i]-3.5+100)][(int)(carz[i]-4.5+200)])
		{
			carx[i]+=(0.5*mul);
			carz[i]+=(0.5*mul);
			mul=1;
//			printf("1\n");
		}
		if(collision_flag[(int)(carx[i]-3.5+100)][(int)(carz[i]+4.5+200)])
		{
			carx[i]+=(0.5*mul);
			carz[i]-=(0.5*mul);
			mul=1;
//			printf("2\n");
		}
		if(collision_flag[(int)(carx[i]+3.5+100)][(int)(carz[i]-4.5+200)])
		{
			carx[i]-=(0.5*mul);
			carz[i]+=(0.5*mul);
			mul=1;
//		printf("3\n");
		}
		if(collision_flag[(int)(carx[i]+3.5+100)][(int)(carz[i]+4.5+200)])
		{
			carx[i]-=(0.5*mul);
			carz[i]-=(0.5*mul);
			mul=1;
		}
		if(mul==1)
		{
			if(speed[i]>0.5 || speed[i]<-0.5)
				speed[i]-=(speed[i]/5);
			else
				speed[i]=0;
		}

	}
	*/
}

void sky()
{
	glEnable(GL_COLOR_MATERIAL);
	glPushMatrix();
	GLUquadricObj *sky = gluNewQuadric();
	//gluQuadricTexture(sky,GL_TRUE);
	gluQuadricDrawStyle(sky,GL_FILL);
	glColor3f(0.3,0.3,0.7);
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, tex_2d);
	gluSphere(sky,550,128,128);
	//glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
}

void try_sphere()
{
	glDisable(GL_COLOR_MATERIAL);
	GLfloat a1[] = {0,0,1,1};
	GLfloat b1[] = {0.2,0.2,0.2,1};
	GLfloat c1[] = {0.8,0.8,0.8,1};
	glPushMatrix();

	glTranslatef(74,16,3);
	glColor3f(0,0,1);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,a1);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,a1);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,xxx);
	(shadeFlag == 1) ? glShadeModel(GL_FLAT) : glShadeModel(GL_SMOOTH);
	glutSolidSphere(5,32,32);

	glColor3f(0.5,0.1,1);
	glTranslatef(0,-16,0);
	glRotatef(-90,1,0,0);
	GLUquadricObj *cyl = gluNewQuadric();
	gluQuadricDrawStyle(cyl,GL_FILL);
	gluCylinder(cyl,2,2,12,32,32);

	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,e0);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,b1);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,c1);

	glPopMatrix();
	//glDisable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
}

void camPosSteer()
{
	float eyex = carx[I] + (0.3)*sin((thetay[I]-130)*(pi/180)), eyey = 0.45, eyez = carz[I] - (0.3)*cos((thetay[I]-130)*(pi/180));
	float refx = carx[I] + 4*sin(thetay[I]*(pi/180)), refy = 0.45, refz = carz[I] - 4*cos(thetay[I]*(pi/180));
	gluLookAt(eyex,eyey,eyez, refx,refy,refz, 0,1,0);
}

void camRoof()
{
	float eyex = carx[I] + 0.1*sin(thetay[I]*(pi/180)), eyey = 0.75, eyez = carz[I] - 0.1*cos(thetay[I]*(pi/180));
	float refx = carx[I] + 4*sin(thetay[I]*(pi/180)), refy = 0.65, refz = carz[I] - 4*cos(thetay[I]*(pi/180));
	gluLookAt(eyex,eyey,eyez, refx,refy,refz, 0,1,0);
}

void camSky()
{
	gluLookAt(carx[I],100+yyy,carz[I],carx[I],0,carz[I],0,0,-1);
}

void camChase()
{
	float eyex = carx[I] + (7)*sin((thetay[I]+170)*(pi/180)), eyey = 2, eyez = carz[I] - (7)*cos((thetay[I]+170)*(pi/180));
	float refx = carx[I] + 3*sin((thetay[I]+10)*(pi/180)), refy = 1, refz = carz[I] - 3*cos((thetay[I]+10)*(pi/180));
	gluLookAt(eyex,eyey,eyez, refx,refy,refz, 0,1,0);
}

void camSlider()
{
	float eyex = carx[I] + 0.1*sin(thetay[I]*(pi/180)) - boxtrans[I]*cos(thetay[I]*(pi/180));
	float eyez = carz[I] - 0.1*cos(thetay[I]*(pi/180)) - boxtrans[I]*sin(thetay[I]*(pi/180));
	float eyey = 0.75;
	float refx = carx[I] + 4*sin(thetay[I]*(pi/180)), refy = 0.65, refz = carz[I] - 4*cos(thetay[I]*(pi/180));
	gluLookAt(eyex,eyey,eyez, refx,refy,refz, 0,1,0);

}

void camTorch()
{
	float eyex = carx[I] - boxtrans[I]*cos(thetay[I]*(pi/180)) + (0.01)*sin((thetay[I]-lightangle[I])*(pi/180));
	float eyez = carz[I] - boxtrans[I]*sin(thetay[I]*(pi/180)) - (0.01)*cos((thetay[I]-lightangle[I])*(pi/180));
	float eyey = 1;
	float refx = carx[I] - boxtrans[I]*cos(thetay[I]*(pi/180)) + (4)*sin((thetay[I]-lightangle[I])*(pi/180));
	float refz = carz[I] - boxtrans[I]*sin(thetay[I]*(pi/180)) - (4)*cos((thetay[I]-lightangle[I])*(pi/180));
	float refy = 1;
	gluLookAt(eyex,eyey,eyez, refx,refy,refz, 0,1,0);
}

void camWheel()
{
	float alpha = atan(0.841/1.113);
	float tyreD = sqrt((1.113)*(1.113) + (0.841)*(0.841));
	float tyrex = carx[I] + tyreD*sin((thetay[I])*(pi/180)+alpha), tyrez = carz[I] - tyreD*cos((thetay[I])*(pi/180)+alpha);
	float eyex = tyrex + (0.25)*sin((thetay[I]+90-wheely[I])*(pi/180)), eyey = 0, eyez = tyrez - (0.25)*cos((thetay[I]+90-wheely[I])*(pi/180));
	float refx = eyex + 4*sin((thetay[I]-wheely[I])*(pi/180)), refy = eyey, refz = eyez - 4*cos((thetay[I]-wheely[I])*(pi/180));
	gluLookAt(eyex,eyey,eyez, refx,refy,refz, 0,1,0);
}

void camhelicopter()
{
	float eyex = carx[I] + helD*sin(helfi*(pi/180))*sin((heltheta+thetay[I])*(pi/180));
	float eyey = helD*cos(helfi*(pi/180));
	float eyez = carz[I] - helD*sin(helfi*(pi/180))*cos((heltheta+thetay[I])*(pi/180));
	float refx = carx[I], refy = 0, refz = carz[I];
	gluLookAt(eyex,eyey,eyez, refx,refy,refz, 0,1,0);
}

void arrowControl()
{
	for(int i=0; i<2; i++)
	{
		if (UP[i] == 1)
		{
			if(speed[i] < 1.0)
				speed[i] += 0.01f;
		}
		else if (DOWN[i] == 1)
		{
			if(speed[i] > -0.3)
				speed[i] -= 0.01f;
		}

		if (LEFT[i]==1)
		{
			if(turn[i] > -1.5)
				turn[i] -= 0.1;
			if(!zeroS(speed[i]))
			{
				if(speed[i] > 0)
					thetay[i] += turn[i];
				else
					thetay[i] -= turn[i];
			}
		}
		else if (RIGHT[i]==1)
		{
			if(turn[i] < 1.5)
				turn[i] += 0.1;
			if(!zeroS(speed[i]))
			{
				if(speed[i] > 0)
					thetay[i] += turn[i];
				else
					thetay[i] -= turn[i];
			}
		}

		if(UP[i]==0 && DOWN[i]==0 && !zeroS(speed[i]))
			speed[i] -= 0.005f*( speed[i]>0?1:-1 );
		if(LEFT[i]==0 && RIGHT[i]==0 && !zeroS(turn[i]))
			turn[i] -= 0.1f*(turn[i]>0?1:-1);

		prevx[i] = carx[i];
		prevz[i] = carz[i];

		carz[i] -= speed[i]*cos(thetay[i]*(pi/180));
		carx[i] += speed[i]*sin(thetay[i]*(pi/180));

		wheelangle[i] = (wheelangle[i] + (int)((speed[i]*100*70)/150) + 360)%360;
		wheely[i] = -((turn[i]*45)/1.5);
		steerangle[i] = -((turn[i]*180)/1.5);
	}
}

void drawObjects(GLMmodel *obj, float x, float y, float z, float ang, float ax, float ay, float az, float sx, float sy, float sz)
{
	glPushMatrix();
	glTranslatef(x,y,z);
	glRotatef(ang, ax, ay, az);
	glScalef(sx,sy,sz);
	glmDraw(obj, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
	glPopMatrix();
}

void grid()
{
	glColor3f(0,0,0);
	glBegin(GL_LINE);
	for(int i=-100;i<101;i++)
	{
	glVertex3f(i, 7, 0);glVertex3f(i+1, 7, 0);
	glVertex3f(0, 7, i);glVertex3f(0, 7, i+1);
	}
	glColor3f(1,1,1);
	glEnd();
}

void road()
{
	float rdht = -0.5;
	//beach side road
	for(int i=-45;i<=0;i++)
	{
		drawObjects(Sroad, 70,rdht, -14.5+i*3.4, 90, 0, 1, 0, 0.001, 0.001, 0.004);
	}
	for(int i=0;i<45;i++)
	{
		drawObjects(Sroad, 70,rdht, 3.5+i*3.4, 90, 0, 1, 0, 0.001, 0.001, 0.004);
	}
	drawObjects(Xroad, 70,rdht-0.1,0 , 90, 0, 1, 0, 0.004, 0.004, 0.004);
	//mountain side road
	for(int i=-24;i<=0;i++)
	{
		drawObjects(Sroad, -55,rdht, -14.5+i*3.4, 90, 0, 1, 0, 0.001, 0.001, 0.004);
	}
	for(int i=0;i<24;i++)
	{
		drawObjects(Sroad, -55,rdht, 3.5+i*3.4, 90, 0, 1, 0, 0.001, 0.001, 0.004);
	}
	drawObjects(Troad, -69.39,rdht-0.1, -14.5 , -90, 0, 1, 0, 0.004, 0.004, 0.004);
	//mid cut the mountains
	for(int i=-17;i<30;i++)
	{
		drawObjects(Sroad, i*3.4,rdht, 0.09, 0, 0, 1, 0, 0.001, 0.001, 0.004);
	}
}


void display()
{
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0,1,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	arrowControl();
	detect_collision();
	if(camMode[I] == 0)
		camChase();
	else if(camMode[I] == 1)
		camRoof();
	else if(camMode[I] == 2)
		camSlider();
	else if(camMode[I] == 3)
		camTorch();
	else if(camMode[I] == 4)
		camWheel();
	else if(camMode[I] == 5)
		camhelicopter();
	else if(camMode[I] == 6)
		camSky();
	else if(camMode[I] == 7)
		camPosSteer();
		
	//-------sky----------------
	sky();

	letThereBeLight();
	headlight();
	torchlight();

	// ---------CAR -------
	carPut();
	try_sphere();
	
	// -------- TERRAIN -----------
	drawObjects(terrain,0,-3.2, 0, -90, 1, 0, 0, 0.05, 0.05, 0.05);

	road();
	if(windows)
	{
		//---------Palm Tree --------------
		drawObjects(palm,100,-1,0, -45, 0, 1, 0,1,1,1);
		drawObjects(palm,100,-1, 100, -45, 0, 1, 0,1,1,1);

		glPushMatrix();
		glTranslatef(100,-1,-80);
		glRotatef(45, 0, 1,0);
		glRotatef(-20, 0, 0,1);
		glmDraw(palm, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(100,-1,-200);
		glRotatef(65, 0, 1,0);
		glRotatef(10, 1, 0,0);
		glmDraw(palm, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		glPopMatrix();

	//-------------chair-------------
		glPushMatrix();
		glTranslatef(150,0.5,-20);
		glScalef(2,2,2);
		glmDraw(chair, GLM_SMOOTH | GLM_MATERIAL);
		glPopMatrix();
	
		glPushMatrix();
		glTranslatef(130,0.5,100);
		glRotatef(100, 0, 1,0);
		glScalef(2,2,2);
		glmDraw(chair, GLM_SMOOTH | GLM_MATERIAL);
		glPopMatrix();
	//-------------END--------
	
	//-----------house[0] ------------
		glPushMatrix();
		glTranslatef(37,4,18);
		glRotatef(-90,1,0,0);
		glRotatef(-90,0,0,1);
		glScalef(0.13,0.13,0.13);
		glmDraw(house[0], GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		glPopMatrix();
	
	//-----------house[1] ------------
		glPushMatrix();
		glTranslatef(100,-0.5,-20);
		glScalef(7,7,7);
		glmDraw(house[1], GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		glPopMatrix();

	//-----------house[2] ------------
		glPushMatrix();
		glTranslatef(22,-0.5,-31);
		glScalef(0.7,0.7,0.7);
		glmDraw(house[2], GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		glPopMatrix();
	
	//-----------house[3] ------------
		drawObjects(house[3], 29, -2, -100, 0, 1, 0, 0, 0.09, 0.09, 0.09);
	//-----------house[4] ------------
		drawObjects(house[4], 30, -1, 60, 0, 1, 0, 0, 0.18, 0.18, 0.18);
	}
	letTherebeDark();

	glutSwapBuffers();
}

void keyboard (unsigned char key, int x1, int y1)
{
	if((int)key == 27)
		exit(0);

	switch(key)
	{
		case 'q':
			I = 1 - I;
			break;
		case 'c':
			camMode[I] = (camMode[I]+1)%8;
			break;
		case '1':
			camMode[I] = 0;
			break;
		case '2':
			camMode[I] = 1;
			break;
		case '3':
			camMode[I] = 2;
			break;
		case '4':
			camMode[I] = 3;
			break;
		case '5':
			camMode[I] = 4;
			break;
		case '6':
			camMode[I] = 5;
			break;
		case '7':
			camMode[I] = 6;
			break;
		case '8':
			camMode[I] = 7;
			break;
		case '-':
			userAMB -= 0.1;
			break;
		case '=':
			userAMB += 0.1;
			break;
		case 'e':
			sunangle -= 1;
			break;
		case 'w':
			sunangle += 1;
			break;
		case 'a':
			if(xxx[0]>0){
			xxx[0] -= 5;
			}
			break;
		case 'z':
			if(xxx[0]<128){
			xxx[0] += 5;
			}
			break;
		case 'f':
			shadeFlag = 1-shadeFlag;
			break;
		case 'r':
			headlightFlag[I] = 1-headlightFlag[I];
			break;
		case 't':
			torchFlag[I] = 1-torchFlag[I];
			break;
		case 's':
			helD += 0.2;
			break;
		case 'x':
			helD -= 0.2;
			break;
		case 'd':
			stopSunAuto = 1 - stopSunAuto;
			break;
	}
}

void SpecialKey(int key,int,int)
{
	if (key == GLUT_KEY_UP)
	{
		DOWN[0] = 0;
		UP[0] = 1;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		UP[0] = 0;
		DOWN[0] = 1;
	}
	else if (key == GLUT_KEY_LEFT)
	{	
		RIGHT[0] =0;
		LEFT[0] = 1;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		LEFT[0] = 0;
		RIGHT[0] = 1;
	}
}
void SpecialKeyUp(int key,int,int)
{
	if (key == GLUT_KEY_UP)
		UP[0] = 0;
	else if (key == GLUT_KEY_DOWN)
		DOWN[0] = 0;
	else if (key == GLUT_KEY_LEFT)
		LEFT[0] = 0;
	else if (key == GLUT_KEY_RIGHT)
		RIGHT[0] = 0;
}

void mouse(int button,int state, int mx, int my)
{
	if(button == 3)
		helD -= 0.2;
	else if(button == 4)
		helD += 0.2;
	if(state==GLUT_DOWN && button==GLUT_LEFT_BUTTON)
	{
		press = 1;
		pressx = mx;
		pressy = my;
	}
	else if(state == GLUT_UP && button==GLUT_LEFT_BUTTON)
		press = 0;

}

void mouseMotion(int mx,int my)
{
	if(press = 1)
	{
		heltheta -= ((pressx-mx)*10)/1920;
		helfi -= ((my-pressy)*10)/1080;
		if(helfi < 5 || helfi > 90)
			helfi += ((my-pressy)*10)/1080;
	}
}

void mouse_passive(int x,int y)
{
	if(x<=640)
	{
		LEFT[1] = 1;
		RIGHT[1] = 0;
	}
	else if(x>=1280)
	{
		RIGHT[1] = 1;
		LEFT[1] = 0;
	}
	else
	{
		LEFT[1] = RIGHT[1] = 0;
	}

	if(y<=360)
	{
		UP[1] = 1;
		DOWN[1] = 0;
	}
	else if(y>=720)
	{
		DOWN[1] = 1;
		UP[1] = 0;
	}
	else
	{
		UP[1] = DOWN[1] = 0;
	}
}

void reshape (int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)width / (GLfloat)height, 0.1, 2000.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc,char **argv)
{
	glutInit(&argc,argv);
	glutInitWindowSize(640,480);
	glutInitWindowPosition(1,1);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Virtual World");

	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	initRender();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialKey);
	glutSpecialUpFunc(SpecialKeyUp);

	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(mouse_passive);

	glutFullScreen();

	glutMainLoop();
	char cChar;
	cin.get(cChar);
	return 0;
}
