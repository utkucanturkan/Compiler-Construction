#pragma once

#include <string>
#include <memory>

#include "Type.h"

class Variable {
public:
	explicit Variable(std::string _identifier);
	explicit Variable(std::string _identifier, std::shared_ptr<const Type> _type);
	std::string identifier;
	std::shared_ptr<const Type> type;
	PrimitiveType primitiveType;
};