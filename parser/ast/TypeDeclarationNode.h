#pragma once

#include "DeclarationNode.h"
#include "TypeNode.h"

class TypeDeclarationNode : public DeclarationNode {
public:
	explicit TypeDeclarationNode(std::string _ident);
	const TypeNode* type;
};