#ifndef CCD_H
#define CCD_H

#include "bone.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/vector_angle.hpp"
#include <cstdio>
#include <iostream>
using namespace glm;
using namespace std;

#define PI 3.14159265
#define ANGLE(rad) (float)rad*180.0/PI
#define RAD(angle) (float)angle*PI/180.0

void ccd(Bone* endEffector, vec3 target, int iterations=3000)
{
	bool found = false;
	while (!found && iterations--)					// δ�ҵ����ڵ���������1000����֮��
	{												// ��ʼ����
		Bone* currentBone = endEffector;			// ��ǰ��������ĩ��Ӱ����
		while (currentBone->parent != NULL)			// ��ǰ�������Ǹ�
		{
			vec4 endPosition = endEffector->getEndPosition();				// �õ���������ĩ��
			vec4 currentEndPosition = currentBone->getEndPosition();		// �õ���ǰ������ĩ��
			vec4 startPosition = currentBone->parent->getEndPosition();		// �õ���ǰ������ǰ�ˣ�����������ĩ�ˣ�
			vec3 toTarget = normalize(vec3(target.x - startPosition.x, target.y - startPosition.y, target.z - startPosition.z));				// �õ�����ǰ�˵�Ŀ������������ģ����һ��
			vec3 toEnd = normalize(vec3(endPosition.x - startPosition.x, endPosition.y - startPosition.y, endPosition.z - startPosition.z));	// �õ�������ǰ�˵�ĩ�˵���������ģ����һ��

			float cosine1 = dot(toEnd, toTarget);	// ���������λ�����õ��н�
			if (cosine1 < 1)						// cos(0)=1��Ҳ��������üн���0~360��
			{
				vec3 crossResult = cross(toEnd, toTarget);		// �õ�һ����ֱ������������������

				float angle = glm::angle(toTarget, toEnd);		// �õ������������нǵ���ֵ

				quat rotation = angleAxis(angle, crossResult);	// ������Ԫ����������ά��ת����ת�ĽǶȼ�Ϊangle����ת�����Ǵ�ֱ����
				rotation = normalize(rotation);					// ��һ����Ԫ��
				vec3 euler = glm::eulerAngles(rotation);		// ת����ŷ���ǣ��洢�����������ת����
				currentBone->rotateMax(euler.x, euler.y, euler.z);	// ���øù����������ת�޶�
			}

			// �ж��ҵ�Ŀ���
			vec3 temp = vec3(endEffector->getEndPosition());
			temp.x -= target.x;
			temp.y -= target.y;
			temp.z -= target.z;
			if (dot(temp, temp) < 0.0000001)			// ͬ������˵õ�����
			{
				found = true;
				cout << "reached" << endl;
			}
			// ����
			currentBone = currentBone->parent;
		}
	}

}


#endif
