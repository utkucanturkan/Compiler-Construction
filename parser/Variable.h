#pragma once

#include <string>
#include <memory>

#include "Type.h"
#include "Selector.h"
#include "ast/Node.h"

class Variable{
public:
	explicit Variable(std::string _identifier);
	explicit Variable(std::string _identifier, std::shared_ptr<const Type> _type);
	std::string identifier;
	std::shared_ptr<const Type> type;
	std::vector<std::shared_ptr<const Selector>> selectors;
	PrimitiveType primitiveType;
};