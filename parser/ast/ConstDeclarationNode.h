#pragma once

#include "DeclarationNode.h"
#include "ExpressionNode.h"

class ConstDeclarationNode :public DeclarationNode {
public:
	explicit ConstDeclarationNode(std::string _ident);
	const ExpressionNode* expression;
};