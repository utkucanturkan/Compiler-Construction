#pragma once

#include "ast/Type/Type.h"

class FieldList {
public:
	std::vector<std::string> identList;
	const Type* typeNode;
};