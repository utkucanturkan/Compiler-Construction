#pragma once

#include<vector>
#include "TermNode.h"

class SimpleExpressionNode {
public:
	std::vector<const SimpleExpressionElement*> elements;
};

class SimpleExpressionElement {
public:
	explicit SimpleExpressionElement(TokenType _sign, const TermNode* _term);
	TokenType sign;
	const TermNode* term;
};