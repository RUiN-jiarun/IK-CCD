#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <vector>
#include <cstdio>
#include <iostream>

#include "bone.h"

// 动画类
class Animation {
private:
	// map:指定骨骼--对应角度，id:帧序号
	std::vector< std::map<unsigned long long, glm::vec3> > sequence;
	// 已经移动的距离
	std::vector< std::map<unsigned long long, glm::vec3> > moved;
	// 帧序号
	int position;

public:
	// 创建一个新的空的动画序列
	Animation();
	// 设定骨骼要旋转的相对角度
	Animation* set(Bone* bone, glm::vec3 angles);
	// 对于根骨骼单独设定
	// 在设置的旋转角的时候递归设置骨骼中的每一节
	Animation* set(Bone* root);

	// 设定关键帧
	// 将骨骼的全部移动记录到一个单独的帧里，position自动调整
	Animation* keyframe();
	// 重设位置
	Animation* start();
	// 标记下一个位置
	bool next();

	// 计算下一帧（对于当前帧的每个骨骼进行旋转）
	// fill: 弧长
	bool frame(float fill, Bone* root);

	// 返回帧序号
	int framesCount();
};

#endif
