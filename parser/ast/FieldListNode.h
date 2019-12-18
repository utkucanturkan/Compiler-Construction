#pragma once

#include<vector>
#include<string>
#include "TypeNode.h"

class FieldListNode {
public:
	std::vector<std::string> identList;
	const TypeNode* typeNode;
};