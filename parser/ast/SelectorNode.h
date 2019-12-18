#pragma once
#include "ExpressionNode.h"

class SelectorNode {

};

class IdentSelectorNode : public SelectorNode {
public:
	explicit IdentSelectorNode(std::string _ident);
	const std::string ident;
};

class IndexSelectorNode : public SelectorNode {
public:
	explicit IndexSelectorNode(const ExpressionNode* _expression);
	const ExpressionNode* expression;
};