#pragma once

#include <memory>
#include <vector>

#include "Type.h"
#include "SimpleExpressionElement.h"

class SimpleExpression {
public:
	explicit SimpleExpression();
	PrimitiveType type;
	std::vector<std::shared_ptr<const SimpleExpressionElement>> elements;
};