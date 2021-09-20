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
	Bone* parent;					// ������
	std::vector<Bone*> bones;		// �������
	float length;					// ��������

	glm::mat4 M;					// �任����
	glm::vec3 rotation;				// ��ǰ��ת��

	glm::vec3 coordinates;			// ��ǰ����
	glm::vec3 constraint[2];		// λ�õ����ƣ��ֱ�����С�����


	// �����ӹ���
	Bone* add(Bone* b);	
	// �Ƴ��ӹ���
	void remove(Bone* b);
	// ���������븸�ؽ�
	void detach();
	// �������λ������
	Bone* constraints(float nx, float mx, float ny, float my, float nz, float mz);
	// ����Ƿ������Ʒ�Χ��
	bool checkConstraints(float x, float y, float z);

	// ���þ�����ת��
	Bone* setRotate(float x, float y, float z);
	// ���������ת�ǣ���Ե�ǰλ�ø�������ı仯��
	Bone* rotate(float dx, float dy, float dz);
	// ��ת���Ƕ�
	Bone* rotateMax(float dx, float dy, float dz);


	// �õ�������ĩ��
	glm::vec4 getEndPosition();
	// �õ�ת������
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
	// ����ĩ��Ӱ�����ı�����ҵ�����
	Bone* bone(unsigned long long id);

	unsigned long long id();


	// ���ݸ������ȴ�������
	Bone(float l);

	Bone(const Bone& b);


	~Bone();
};

#endif
