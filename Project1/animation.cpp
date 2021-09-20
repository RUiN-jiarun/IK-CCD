#include "animation.h"

#include <algorithm>

// ����һ�����ź���
glm::vec3 sgn(glm::vec3 a, glm::vec3 b) {
	return glm::vec3(
		a.x >= b.x ? -1 : 1,
		a.y >= b.y ? -1 : 1,
		a.z >= b.z ? -1 : 1
	);
}

Animation::Animation() {
	// ��ʼ��
	position = 0;
	sequence.resize(1);
}

Animation* Animation::set(Bone* bone, glm::vec3 angles) {
	// �趨sequence�й�����Ӧ�ĽǶ�
	sequence[position][bone->id()] = angles;

	return this;
}


Animation* Animation::set(Bone* root) {
	// �ӹ������ĸ���ʼ����������set()�趨ÿһ�ڵĽǶ�
	sequence[position][root->id()] = glm::vec3(root->rotation);

	for (std::vector<Bone*>::iterator it = root->bones.begin(); it != root->bones.end(); ++it) {
		set(*it);
	}

	return this;
}

Animation* Animation::keyframe() {
	position++;
	// ��֡���е�map���ݡ�������һ֡����¼�������ƶ�
	sequence.resize(position+1);
	// ����position
	sequence[position] = sequence[position-1];

	return this;
}


Animation* Animation::start() {
	// ���¿�ʼ����ʼ��position���ƶ��ļ�¼moved
	position = 0;
	moved.clear();
	moved.resize(sequence.size());

	// ����֡���е�������¼��moved��
	for (int i = 0; i < sequence.size(); i++) {
		for (std::map<unsigned long long,glm::vec3>::iterator kv = sequence[i].begin(); kv != sequence[i].end(); ++kv) {
			moved[i][kv->first] = glm::vec3(0.0f);
		}
	}

	return this;
}

bool Animation::next() {
	if (position >= framesCount())
		return false;

	position++;

	return true;
}

bool Animation::frame(float fill, Bone* root) {
	//std::cout << framesCount() << std::endl;
	if (position >= framesCount()) {
		// ���������֡���е��ܳ�������ʧ��
		return false;
	}
	int c = 0;
	int d = 0;
	Bone* current;
	// ����֡����
	for (std::map<unsigned long long,glm::vec3>::iterator kv = sequence[position].begin(); kv != sequence[position].end(); ++kv) {
		// ��֡�˶�������
		glm::vec3 dv = glm::abs(sequence[position-1][kv->first] - kv->second);

		glm::vec3 s = sgn(sequence[position-1][kv->first], kv->second);

		// fill�ǻ����İٷ�����ʽ
		// ��������˶�����
		dv.x *= s.x * fill;
		dv.y *= s.y * fill;
		dv.z *= s.z * fill;

		// ���˶�����dv���浽moved
		moved[position][kv->first] += glm::abs(dv);
		current = root->bone(kv->first);

		d += 3;

		// �Ƚ����������ƫ��������Ӧ�ƶ�
		if (glm::abs(moved[position][kv->first]).x >= glm::abs(sequence[position-1][kv->first] - kv->second).x) {
			c++;
		} else {
			current->rotate(dv.x, 0, 0);
		}

		if (glm::abs(moved[position][kv->first]).y >= glm::abs(sequence[position-1][kv->first] - kv->second).y) {
			c++;
		} else {
			current->rotate(0, dv.y, 0);
		}

		if (glm::abs(moved[position][kv->first]).z >= glm::abs(sequence[position-1][kv->first] - kv->second).z) {
			c++;
		} else {
			current->rotate(0, 0, dv.z);
		}
	}

	//std::cout << c << " " << d << std::endl;
	return c < d;
}



int Animation::framesCount() {
	return sequence.size();
}


