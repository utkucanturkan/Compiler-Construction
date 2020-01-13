#pragma once

#include "ast/Statement/Statement.h"
#include "ast/Expression/Expression.h"

class WhileStatement : public Statement {
public:
	const Expression* expression;
	std::vector<const Statement*> statements;
};