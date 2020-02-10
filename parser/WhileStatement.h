#pragma once

#include <memory>
#include <vector>

#include "Expression.h"
#include "Statement.h"

class WhileStatement : public Statement {
public:
	explicit WhileStatement(std::shared_ptr<const Expression> _expression);
	std::shared_ptr<const Expression> expression;
	std::vector<std::shared_ptr<const Statement>> statements;
};