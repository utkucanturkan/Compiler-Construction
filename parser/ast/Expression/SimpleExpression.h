#pragma once

#include "ast/Expression/SimpleExpressionElement.h"
#include "ast/Expression/ExpressionType.h"

class SimpleExpression {
public:
	ExpressionType type;
	std::vector<const SimpleExpressionElement*> elements;
};