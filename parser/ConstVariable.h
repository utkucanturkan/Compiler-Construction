#pragma once

#include "Variable.h"
#include "Expression.h"

class ConstVariable : public Variable {
public:
	explicit ConstVariable(std::string _identifier, std::shared_ptr<const Expression> _expression);
	std::shared_ptr<const Expression> expression;
	// getValue
};
