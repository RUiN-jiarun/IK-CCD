#include "animation.h"

#include <algorithm>

// 定义一个符号函数
glm::vec3 sgn(glm::vec3 a, glm::vec3 b) {
	return glm::vec3(
		a.x >= b.x ? -1 : 1,
		a.y >= b.y ? -1 : 1,
		a.z >= b.z ? -1 : 1
	);
}

Animation::Animation() {
	// 初始化
	position = 0;
	sequence.resize(1);
}

Animation* Animation::set(Bone* bone, glm::vec3 angles) {
	// 设定sequence中骨骼对应的角度
	sequence[position][bone->id()] = angles;

	return this;
}


Animation* Animation::set(Bone* root) {
	// 从骨骼链的根开始，迭代调用set()设定每一节的角度
	sequence[position][root->id()] = glm::vec3(root->rotation);

	for (std::vector<Bone*>::iterator it = root->bones.begin(); it != root->bones.end(); ++it) {
		set(*it);
	}

	return this;
}

Animation* Animation::keyframe() {
	position++;
	// 对帧序列的map扩容——增加一帧来记录骨骼的移动
	sequence.resize(position+1);
	// 调整position
	sequence[position] = sequence[position-1];

	return this;
}


Animation* Animation::start() {
	// 重新开始，初始化position与移动的记录moved
	position = 0;
	moved.clear();
	moved.resize(sequence.size());

	// 根据帧序列迭代，记录到moved里
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
		// 如果超过了帧序列的总长，返回失败
		return false;
	}
	int c = 0;
	int d = 0;
	Bone* current;
	// 遍历帧序列
	for (std::map<unsigned long long,glm::vec3>::iterator kv = sequence[position].begin(); kv != sequence[position].end(); ++kv) {
		// 两帧运动向量差
		glm::vec3 dv = glm::abs(sequence[position-1][kv->first] - kv->second);

		glm::vec3 s = sgn(sequence[position-1][kv->first], kv->second);

		// fill是弧长的百分数形式
		// 这里计算运动向量
		dv.x *= s.x * fill;
		dv.y *= s.y * fill;
		dv.z *= s.z * fill;

		// 将运动向量dv保存到moved
		moved[position][kv->first] += glm::abs(dv);
		current = root->bone(kv->first);

		d += 3;

		// 比较三个方向的偏移量，相应移动
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


