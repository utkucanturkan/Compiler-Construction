#pragma once

#include "DeclarationNode.h"
#include "TypeNode.h"

class VarDeclarationNode :public DeclarationNode {
public:
	explicit VarDeclarationNode(std::string _ident);
	const TypeNode* type;
};