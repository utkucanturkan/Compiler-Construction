#pragma once

#include "Factor.h"
#include "Expression.h"

class ExpressionFactor : public Factor {
public:
	ExpressionFactor(std::shared_ptr<const Expression> _expression);
	std::shared_ptr<const Expression> expression;
};