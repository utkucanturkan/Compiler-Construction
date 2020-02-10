#pragma once

#include <string>
#include <memory>

#include "Variable.h"
#include "Statement.h"
#include "Expression.h"

class AssignmentStatement : public Statement {
public:
	explicit AssignmentStatement(std::shared_ptr<const Variable> _variable, std::shared_ptr<const Expression> _expression);
	std::shared_ptr<const Variable> variable;
	std::shared_ptr<const Expression> expression;
};
