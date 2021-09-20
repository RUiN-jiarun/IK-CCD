#include <stdio.h> 
#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <time.h>
#include "bone.h"
#include "animation.h"
#include "ccd.h"
#include <iostream>

using namespace std;


float speed_x = 0; 
float speed_y = 0; 
int lastTime = 0;
float angle_x = -30;
float angle_y = 0;
float zoom = 15.0f;
float cdx, cdy;


glm::vec3 target;
Animation* animation = new Animation();
bool animation_running = false;
float animation_fill = 0.0f;
bool root_set = false;
unsigned long long effector = 0;

GLfloat pod_color[] = { 0.0, 1.0, 1.0, 0.2 };
GLfloat plain_color[] = { 0.86, 0.86, 0.86, 0.3 };
GLfloat bone_color[] = { 0.0, 0.0, 1.0, 0.95 };
GLfloat joint_color[] = {1.0, 0.0, 0.0, 0.95 };
GLfloat tip_color[] = { 1.0,1.0,0.0, 0.95 };
GLfloat ball_color[] = {1.0,1.0,1.0, 1.0 };


Bone* root;

// �������Ŀ���
void randomizeTarget() {
	target.x = (float(rand() % 25000)-10000) / 5000.0;
	target.y = (float(rand() % 25000)-10000) / 5000.0;
	target.z = (float(rand() % 25000)-10000) / 5000.0;

	GLfloat light_position[] = { target.x, target.y, target.z, 1.0 };


	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	ball_color[3] = 0.5;
	glLightfv(GL_LIGHT1, GL_DIFFUSE, ball_color);
	ball_color[3] = 1.0;

	delete animation;
	animation = new Animation();

}

// ���ƹ���
void drawBones(Bone* b) {
	glPushMatrix();
	GLfloat s[] = { 0 };
	GLfloat t[] = { 0.0,0.0, 0.0,1.0 };
	glm::mat4 previous = b->M;

	if (b->parent != NULL) {
		b->M = glm::translate(b->M, glm::vec3(0.0f, 0.0f, b->parent->length));

		b->M = glm::rotate(b->M, b->rotation.x, glm::vec3(b->M*glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
		b->M = glm::rotate(b->M, b->rotation.y, glm::vec3(b->M*glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
		b->M = glm::rotate(b->M, b->rotation.z, glm::vec3(b->M*glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));

		b->M = b->parent->M * b->M;
	}
	else {
		b->M = glm::rotate(b->M, b->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		b->M = glm::rotate(b->M, b->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		b->M = glm::rotate(b->M, b->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glLoadMatrixf(glm::value_ptr(b->M));

	// ���ƹؽ�
	if (b->parent != NULL) {
		s[0] = 250;
		glMaterialfv(GL_FRONT, GL_SPECULAR, joint_color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, joint_color);
		glMaterialfv(GL_FRONT, GL_SHININESS, s);
		glMaterialfv(GL_FRONT, GL_EMISSION, t);
		glutSolidSphere(0.1f, 32, 32);
	}

	// ���ƹ���
	glMaterialfv(GL_FRONT, GL_SPECULAR, bone_color);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, bone_color);
	s[0] = 11;
	glMaterialfv(GL_FRONT, GL_SHININESS, s);
	glMaterialfv(GL_FRONT, GL_EMISSION, t);

	GLUquadricObj* q = gluNewQuadric();
	if (b->bones.empty())
		gluCylinder(q, 0.05f, 0.05f, b->length - 0.5f, 32, 32);
	else
		gluCylinder(q, 0.05f, 0.05f, b->length, 32, 32);
	gluDeleteQuadric(q);

	glPopMatrix();

	// �����������Բ׶��ͷ
	if (b->bones.empty()) {
		glPushMatrix();

		glLoadMatrixf(glm::value_ptr(glm::translate(b->M, glm::vec3(0.0f, 0.0f, b->length - 0.5f))));
		glMaterialfv(GL_FRONT, GL_SPECULAR, tip_color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, tip_color);
		s[0] = 250;
		glMaterialfv(GL_FRONT, GL_SHININESS, s);
		glMaterialfv(GL_FRONT, GL_EMISSION, t);
		gluCylinder(q, 0.1f, 0.0f, 0.3f, 32, 32);

		glPopMatrix();
	}

	// �ݹ�
	for (std::vector<Bone*>::iterator it = b->bones.begin(); it != b->bones.end(); it++) {
		drawBones(*it);
	}

	b->M = previous;
}


// չʾ֡��ʵʱ���Ƴ�����
void displayFrame(void) {
	// ����֡
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 cameraCenter = glm::vec3(0.0f + cdx, 2.0f + cdy, 0.0);
	glm::vec3 cameraEye = glm::vec3(0.0f, 1.0f, -zoom);
	glm::vec3 cameraNose = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 V = glm::lookAt(cameraEye, cameraCenter, cameraNose);
	V = glm::rotate(V, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
	V = glm::rotate(V, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 P = glm::perspective(50.0f, 1.0f, 1.0f, 50.0f);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(P));
	glMatrixMode(GL_MODELVIEW);


	glm::mat4 M = glm::mat4(1.0f);
	glLoadMatrixf(glm::value_ptr(V*M));

	// ����Ŀ��㣨��Դ��
	GLfloat s[] = { 128 };
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_SPECULAR, ball_color);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ball_color);
	glMaterialfv(GL_FRONT, GL_SHININESS, s);
	ball_color[3] = 0.5;
	glMaterialfv(GL_FRONT, GL_EMISSION, ball_color);
	ball_color[3] = 1.0;
	glLoadMatrixf(glm::value_ptr(glm::translate(M*V, target)));
	glutSolidSphere(0.2f, 32, 32);
	glPopMatrix();

	// ���Ƶ���
	GLfloat t[] = { 0.0,0.0, 0.0,1.0 };
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_SPECULAR, pod_color);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, pod_color);
	s[0] = 0;
	glMaterialfv(GL_FRONT, GL_SHININESS, s);
	glMaterialfv(GL_FRONT, GL_EMISSION, t);
	glScalef(0.75f, 0.05f, 0.75f);
	glTranslatef(0.0f, -0.1f, 0.0f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// ���Ƶ���
	//GLfloat t[] = { 0.0,0.0, 0.0,1.0 };
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_SPECULAR, plain_color);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, plain_color);
	s[0] = 0;
	glMaterialfv(GL_FRONT, GL_SHININESS, s);
	glMaterialfv(GL_FRONT, GL_EMISSION, t);
	glScalef(10.0f, 0.05f, 10.0f);
	glTranslatef(0.0f, -70.0f, 0.0f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// ������ת����
	root->M = glm::rotate(M*V, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	root->setRotate(root->rotation.x, root->rotation.y, root->rotation.z);

	// ���ƹ���
	drawBones(root);


	glutSwapBuffers();
}


// ��ʾ��һ֡
void nextFrame(void) {
	//cout << "nextframe" << endl;
	int actTime = glutGet(GLUT_ELAPSED_TIME);
	float interval = actTime - lastTime;		// ʱ����������lastTime��ʼ��Ϊ0
	lastTime = actTime;
	angle_x += speed_x * interval / 500.0;		// ����仯�Ƕ�
	angle_y += speed_y * interval / 500.0;

	// չʾ����
	// ������������frame()�����и����궯������֮������animation_runningΪtrue
	if (animation_running) {
		animation_fill += interval / 500.0;
		if (!animation->frame(interval / 500.0, root)) {
			// ���������е���һ֡����ʧ�ܣ���ֹͣ
			animation_fill = 0.0;
			if (!animation->next()) {
				animation_running = false;
			}
		}
	}

	if (angle_x>360) angle_x -= 360;
	if (angle_x>360) angle_x += 360;
	if (angle_y>360) angle_y -= 360;
	if (angle_y>360) angle_y += 360;

	glutPostRedisplay();
}

// �����¼�
void specKeyDown(int c, int x, int y) {
	if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
		switch (c) {
		case GLUT_KEY_LEFT:
			cdx -= 0.25;
			break;
		case GLUT_KEY_RIGHT:
			cdx += 0.25;
			break;
		case GLUT_KEY_UP:
			cdy -= 0.25;
			break;
		case GLUT_KEY_DOWN:
			cdy += 0.25;
			break;
		}
	}
	else {
		switch (c) {
		case GLUT_KEY_LEFT:
			speed_y = 60;
			break;
		case GLUT_KEY_RIGHT:
			speed_y = -60;
			break;
		case GLUT_KEY_UP:
			speed_x = 60;
			break;
		case GLUT_KEY_DOWN:
			speed_x = -60;
			break;
		}
	}
}

void specKeyUp(int c, int x, int y) {
	switch (c) {
	case GLUT_KEY_LEFT:
		speed_y = 0;
		break;
	case GLUT_KEY_RIGHT:
		speed_y = -0;
		break;
	case GLUT_KEY_UP:
		speed_x = 0;
		break;
	case GLUT_KEY_DOWN:
		speed_x = -0;
		break;
	}
}

void keyDown(unsigned char c, int x, int y) {
	switch (c) {
		// ��ͷ����
		case '-':
			zoom += 0.5f;
			break;
		case '=':
		case '+':
			zoom -= 0.5f;
			break;
		// �ƶ�Ŀ���
		case 'q':
			target.z -= 0.2f;
			break;
		case 'e':
			target.z += 0.2f;
			break;
		case 'w':
			target.y += 0.2;
			break;
		case 's':
			target.y -= 0.2;
			break;
		case 'a':
			target.x -= 0.2;
			break;
		case 'd':
			target.x += 0.2;
			break;
		case ' ':
			randomizeTarget();
			break;
	}
}


void keyUp(unsigned char c, int x, int y) {
	switch (c) {
	case 'o':
		ccd(root->bone(effector), target);
		break;

	case 'p':
		// ��һ����ʱ����
		Bone *b = new Bone(*root);
		// ��CCD�����趨��ÿһ���ǽڵ���ת�Ƕ�
		ccd(b->bone(effector), target);
		// ����һ���µĶ�������
		delete animation;
		animation = new Animation();
		// ����λ�ò����Ӷ���֡
		animation->set(root)->keyframe();		// ����λ��ȷ������������һ֡
		for (int z = effector; z != 0; z /= 10) {
			// ����Ӱ��������ÿ������Ҫ�����λ��
			// ��չ��������
			animation->set(b->bone(z), b->bone(z)->rotation)->keyframe();
		}
		// ɾ����ʱ����
		delete b;
		// ���¶������У��趨��ʼ����
		animation->start()->next();
		animation_running = true;

		break;
	}
}


int main(int argc, char* argv[]) {

	srand(time(0));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(720, 720);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("IK_CCD");
	glutDisplayFunc(displayFrame);
	glutIdleFunc(nextFrame);

	glutSpecialFunc(specKeyDown);
	glutSpecialUpFunc(specKeyUp);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);

	glShadeModel(GL_SMOOTH);

	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat light_position[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_specular[] = { 0.5, 0.5, 0.5, 1.0 };


	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	randomizeTarget();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);

	// ����һ��4�ǽڵĹ�����
	root = new Bone(0.0f);

	root->add(new Bone(1))->rotate(20, 0, 0)
		->add(new Bone(1))->rotate(0, 20, 0)
		->add(new Bone(1))->rotate(20, 0, 0)
		->add(new Bone(1))->rotate(0, 20, 0);

	effector = 1111;

	glutMainLoop();

	delete root;
	delete animation;

	return 0;
}
