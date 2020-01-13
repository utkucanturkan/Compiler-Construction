#pragma once

#include "ast/Expression/SimpleExpression.h"
#include "Token.h"
#include "ast\Expression\ExpressionType.h"

class Expression {
public:
	explicit Expression(const SimpleExpression* _lhs);
	explicit Expression(const SimpleExpression* _lhs, TokenType _sign, const SimpleExpression* _rhs);
	ExpressionType type;
	const SimpleExpression* lhs;
	const TokenType* sign;
	const SimpleExpression* rhs;
};