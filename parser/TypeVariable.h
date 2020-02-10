#pragma once

#include "Variable.h"

class TypeVariable : public Variable {
public:
	explicit TypeVariable(std::string _identifier, std::shared_ptr<const Type> _type);
	std::shared_ptr<const Type> type;
	//std::vector<std::shared_ptr<const Selector>> selector;
};
