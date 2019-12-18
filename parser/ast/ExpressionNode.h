#pragma once
#include "SimpleExpressionNode.h"

class ExpressionNode {
public:
	explicit ExpressionNode(const SimpleExpressionNode* _rhs);
	explicit ExpressionNode(const SimpleExpressionNode* _rhs, TokenType _sign, const SimpleExpressionNode* _lhs);

	const SimpleExpressionNode* rhs;
	const TokenType* sign;
	const SimpleExpressionNode* lhs;
};