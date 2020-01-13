#pragma once

#include "ast/Expression/Factor.h"
#include "ast/Expression/Expression.h"

class ExpressionFactor :public Factor {
public:
	explicit ExpressionFactor(const Expression* _expression);
	const Expression* expression;
};