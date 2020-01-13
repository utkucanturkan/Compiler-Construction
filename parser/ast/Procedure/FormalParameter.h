#pragma once

#include <string>
#include <vector>

#include "ast/Type/Type.h"

class FormalParameter {
public:
	std::vector<std::string> identList;
	const Type* type;
};
