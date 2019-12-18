#pragma once

#include<string>

#include "SelectorNode.h"


class FactorNode {

};

class IdentSelectorFactorNode : public FactorNode {
public:
	explicit IdentSelectorFactorNode(std::string _ident);
	std::string ident;
	std::vector<const SelectorNode*> selectors;
};

class NotFactorNode :public FactorNode {
public:
	explicit NotFactorNode(const FactorNode* _factor);
	const FactorNode* factor;
};

class IntegerFactorNode : public FactorNode {
public:
	explicit IntegerFactorNode(int _integer);
	int integer;
};

class ExpressionFactorNode :public FactorNode {
public:
	explicit ExpressionFactorNode(const ExpressionNode* _expression);
	const ExpressionNode* expression;
};