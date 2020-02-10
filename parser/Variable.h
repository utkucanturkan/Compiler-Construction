#pragma once

#include <string>
#include <memory>

#include "Type.h"

class Variable {
public:
	explicit Variable(std::string _identifier);
	std::string identifier;
	std::shared_ptr<const Type> type;
};