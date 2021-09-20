#ifndef BONE_H
#define BONE_H

#include <vector>
#include <algorithm>
#include "glm/glm.hpp"

#include <exception>


#define PI 3.14159265
#define ANGLE(rad) (float)rad*180.0/PI
#define RAD(angle) (float)angle*PI/180.0


class Bone {
public:
	Bone* parent;					// 父骨骼
	std::vector<Bone*> bones;		// 骨骼组合
	float length;					// 骨骼长度

	glm::mat4 M;					// 变换矩阵
	glm::vec3 rotation;				// 当前旋转角

	glm::vec3 coordinates;			// 当前坐标
	glm::vec3 constraint[2];		// 位置的限制，分别是最小和最大


	// 增加子骨骼
	Bone* add(Bone* b);	
	// 移除子骨骼
	void remove(Bone* b);
	// 分离自身与父关节
	void detach();
	// 设置最大位置限制
	Bone* constraints(float nx, float mx, float ny, float my, float nz, float mz);
	// 检查是否在限制范围内
	bool checkConstraints(float x, float y, float z);

	// 设置绝对旋转角
	Bone* setRotate(float x, float y, float z);
	// 设置相对旋转角（相对当前位置各个方向的变化）
	Bone* rotate(float dx, float dy, float dz);
	// 旋转最大角度
	Bone* rotateMax(float dx, float dy, float dz);


	// 得到骨骼的末端
	glm::vec4 getEndPosition();
	// 得到转换矩阵
	glm::mat4 getTransform();

	/**
	 * Find Bone using its unique identifier. On every joint there
	 * may be maximum 8 bones identified using this method. Should
	 * be used once in the while to obtain the pointer to the bone.
	 *
	 * The rightmost digit represents the current joint index
	 * (i.e. 1 will return first bone, 4 will return 4th bone - vector index 3).
	 * If id is longer then one digit, it is read recusively
	 * from right to left (i.e. 31 will return 3rd bone on 1st bone joint,
	 * 231 will return 2nd bone on previously mentioned bone joint).
	 *
	 * @param id Bone identifier (every digit must be [1;9])
	 * @return Bone
	 */
	// 根据末端影响器的编号来找到骨骼
	Bone* bone(unsigned long long id);

	unsigned long long id();


	// 根据给定长度创建骨骼
	Bone(float l);

	Bone(const Bone& b);


	~Bone();
};

#endif
