#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <vector>
#include <cstdio>
#include <iostream>

#include "bone.h"

// ������
class Animation {
private:
	// map:ָ������--��Ӧ�Ƕȣ�id:֡���
	std::vector< std::map<unsigned long long, glm::vec3> > sequence;
	// �Ѿ��ƶ��ľ���
	std::vector< std::map<unsigned long long, glm::vec3> > moved;
	// ֡���
	int position;

public:
	// ����һ���µĿյĶ�������
	Animation();
	// �趨����Ҫ��ת����ԽǶ�
	Animation* set(Bone* bone, glm::vec3 angles);
	// ���ڸ����������趨
	// �����õ���ת�ǵ�ʱ��ݹ����ù����е�ÿһ��
	Animation* set(Bone* root);

	// �趨�ؼ�֡
	// ��������ȫ���ƶ���¼��һ��������֡�position�Զ�����
	Animation* keyframe();
	// ����λ��
	Animation* start();
	// �����һ��λ��
	bool next();

	// ������һ֡�����ڵ�ǰ֡��ÿ������������ת��
	// fill: ����
	bool frame(float fill, Bone* root);

	// ����֡���
	int framesCount();
};

#endif
