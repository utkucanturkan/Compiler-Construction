#pragma once
#include "FieldListNode.h"

class TypeNode {

};

class IdentTypeNode : public TypeNode {
public:
	explicit IdentTypeNode(std::string _ident);
	std::string ident;
};

class ArrayTypeNode : public TypeNode {
public:
	explicit ArrayTypeNode(const ExpressionNode* _expression, const TypeNode* _type);
	const ExpressionNode* expression;
	const TypeNode* type;
};

class RecordTypeNode : public TypeNode {
public:
	std::vector<const FieldListNode*> fieldListNodes;
};