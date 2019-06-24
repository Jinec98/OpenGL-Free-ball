#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

#define G 9.8

GLfloat BLACK[] = { 0, 0, 0 };
GLfloat WHITE[] = { 1, 1, 1 };
GLfloat RED[] = { 1, 0, 0 };

double t = 0.05;//��λʱ��

class Ball
{
public:
	double radius;//��İ뾶
	GLfloat* color;//�����ɫ
	double posHeight;//�����߸߶�
	double x;
	double y;
	double z;
	double v;//����ٶ�
	int direction;//����˶�����

	Ball(double r, GLfloat* c, double x, double y, double z)
	{
		this->radius = r;
		this->color = c;
		this->posHeight = y;
		this->x = x;
		this->y = y;
		this->z = z;
		this->direction = 1;
		this->v = 0;
	}

	//С��λ�ø��²�����
	void update()
	{
		if (direction == 1)//�����ƶ�
		{
			y -= v * t + 0.5*G*t*t;
			v += G * t;
			if (y < radius)
			{
				y = radius;
				direction = -1;
			}
		}
		else if (direction == -1)//�����ƶ�
		{
			y += v * t - 0.5*G*t*t;
			v -= G * t;
			if (y > posHeight)
			{
				y = posHeight;
				direction = 1;
			}
		}

		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		glTranslated(x, y, z);
		glutSolidSphere(radius, 30, 30);
		glPopMatrix();
	}
};

class CheckerBoard
{
public:
	int displayListId;
	int width;
	int length;
	double centerX;
	double centerY;

	CheckerBoard(int width, int length)
	{
		this->width = width;
		this->length = length;
		this->centerX = width / 2;
		this->centerY = length / 2;
	}

	void create()
	{
		displayListId = glGenLists(1);
		glNewList(displayListId, GL_COMPILE);

		GLfloat lightPosition[] = { 5, 10, 5,1 };//��Դλ��
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

		glBegin(GL_QUADS);
		glNormal3d(0, 1, 0);//����������
		for (int x = 0; x < width - 1; x++)
		{
			for (int z = 0; z < length - 1; z++)
			{
				//����ÿ�����ӵĲ���
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (x + z) % 2 == 0 ? BLACK : WHITE);

				glVertex3d(x, 0, z);
				glVertex3d(x + 1, 0, z);
				glVertex3d(x + 1, 0, z + 1);
				glVertex3d(x, 0, z + 1);
			}
		}
		glEnd();
		glEndList();
	}

	void draw()
	{
		glCallList(displayListId);
	}
};

class Camera {
public:
	double theta;      //ȷ��x��z��λ��
	double y;          //yλ��
	double dTheta;     //�Ƕ�����
	double dy;         //����y����
public:
	//�๹�캯����Ĭ�ϳ�ʼ���÷�
	Camera() : theta(10), y(10), dTheta(0.04), dy(0.2) {}
	//�෽��
	double getX() { return 15 * cos(theta); }
	double getY() { return y; }
	double getZ() { return 15 * sin(theta); }
	void moveRight() { theta += dTheta; }
	void moveLeft() { theta -= dTheta; }
	void moveUp() { y += dy; }
	void moveDown() { if (y > dy) y -= dy; }
};

Camera camera;
CheckerBoard checkerboard(10, 10);
Ball balls[] = {
	Ball(0.8, WHITE, 4, 6, 6),
	Ball(0.5, RED, 4, 8, 3)
};

void init()
{
	glEnable(GL_DEPTH_TEST);
	//����ɢ�䡢���淴���ǰ����ĸ߹⾵�淴��Ϊ�׹�
	glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
	glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);
	//����ƹ�
	glEnable(GL_LIGHTING);
	//��0#��
	glEnable(GL_LIGHT0);

	checkerboard.create();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(camera.getX(), camera.getY(), camera.getZ(),    //�����λ��
		checkerboard.centerX, 0.0, checkerboard.centerY,   //��������
		0.0, 1.0, 0.0);

	//��������
	checkerboard.draw();
	//����С��
	for (int i = 0; i < sizeof balls / sizeof(Ball); i++)
	{
		balls[i].update();
	}

	glutSwapBuffers();
}

void reshape(GLint w, GLint h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, GLfloat(w) / GLfloat(h), 1.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
}

void timer(int v) 
{
	//����ʱ������ʱ�����õĺ���
	glutPostRedisplay();
	//������һ�μ�ʱ���Ĳ���
	glutTimerFunc(10, timer, v);
}

void onKey(int key, int, int) 
{
	switch (key) 
	{
	case GLUT_KEY_LEFT: camera.moveLeft(); break;
	case GLUT_KEY_RIGHT: camera.moveRight(); break;
	case GLUT_KEY_UP: camera.moveUp(); break;
	case GLUT_KEY_DOWN: camera.moveDown(); break;
	}
	glutPostRedisplay();
}
int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Free Falldown Balls");

	glutDisplayFunc(display);   //������ʾ����
	glutReshapeFunc(reshape);   //���ô��ڵ�����С�ĺ���
	glutSpecialFunc(onKey);   //���ð���������
	glutTimerFunc(10, timer, 0);  //���ü�ʱ������--ÿ10ms������1��
	
	init();
	glutMainLoop();

	return 0;
}