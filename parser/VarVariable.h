#pragma once

#include "Variable.h"

class VarVariable : public Variable {
public:
	explicit VarVariable(std::string _identifier, std::shared_ptr<const Type> _type);
	std::shared_ptr<const Type> type;
};
