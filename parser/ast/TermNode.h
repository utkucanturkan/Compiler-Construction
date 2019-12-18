#pragma once

#include<vector>
#include "FactorNode.h"

class TermNode {
public:
	std::vector<const TermNodeElement*> elements;
};

class TermNodeElement {
public:
	explicit TermNodeElement(TokenType _sign, const FactorNode* _factor);
	TokenType sign;
	const FactorNode* factor;
};