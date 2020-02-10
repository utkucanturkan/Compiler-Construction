#pragma once

#include <memory>

#include "SimpleExpression.h"
#include "Token.h"
#include "Type.h"

class Expression {
public:
	Expression(std::shared_ptr<const SimpleExpression> _lhs);
	Expression(std::shared_ptr<const SimpleExpression> _lhs, TokenType _op, std::shared_ptr<const SimpleExpression> _rhs);
	std::shared_ptr<const SimpleExpression> lhs;
	const TokenType op;
	std::shared_ptr<const SimpleExpression> rhs;
	PrimitiveType type;
};