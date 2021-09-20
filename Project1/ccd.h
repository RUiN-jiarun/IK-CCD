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
	while (!found && iterations--)					// 未找到且在迭代次数（1000？）之内
	{												// 开始迭代
		Bone* currentBone = endEffector;			// 当前骨骼——末端影响器
		while (currentBone->parent != NULL)			// 当前骨骼不是根
		{
			vec4 endPosition = endEffector->getEndPosition();				// 得到骨骼链的末端
			vec4 currentEndPosition = currentBone->getEndPosition();		// 得到当前骨骼的末端
			vec4 startPosition = currentBone->parent->getEndPosition();		// 得到当前骨骼的前端（即父骨骼的末端）
			vec3 toTarget = normalize(vec3(target.x - startPosition.x, target.y - startPosition.y, target.z - startPosition.z));				// 得到骨骼前端到目标点的向量——模长归一化
			vec3 toEnd = normalize(vec3(endPosition.x - startPosition.x, endPosition.y - startPosition.y, endPosition.z - startPosition.z));	// 得到骨骼链前端到末端的向量——模长归一化

			float cosine1 = dot(toEnd, toTarget);	// 点乘两个单位向量得到夹角
			if (cosine1 < 1)						// cos(0)=1，也就是如果该夹角在0~360度
			{
				vec3 crossResult = cross(toEnd, toTarget);		// 得到一个垂直于这两个向量的向量

				float angle = glm::angle(toTarget, toEnd);		// 得到这两个向量夹角的数值

				quat rotation = angleAxis(angle, crossResult);	// 构造四元数来进行三维旋转：旋转的角度即为angle，旋转的轴是垂直向量
				rotation = normalize(rotation);					// 归一化四元数
				vec3 euler = glm::eulerAngles(rotation);		// 转化到欧拉角，存储三个方向的旋转分量
				currentBone->rotateMax(euler.x, euler.y, euler.z);	// 设置该骨骼的最大旋转限度
			}

			// 判断找到目标点
			vec3 temp = vec3(endEffector->getEndPosition());
			temp.x -= target.x;
			temp.y -= target.y;
			temp.z -= target.z;
			if (dot(temp, temp) < 0.0000001)			// 同向量点乘得到距离
			{
				found = true;
				cout << "reached" << endl;
			}
			// 迭代
			currentBone = currentBone->parent;
		}
	}

}


#endif
