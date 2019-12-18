#pragma once

#include "DeclarationNode.h"

class ProcedureDeclarationNode :public DeclarationNode {
public:
	explicit ProcedureDeclarationNode(const ProcedureHeadingNode* _heading, const ProcedureBodyNode* _body);
	const ProcedureHeadingNode* heading;
	const ProcedureBodyNode* body;
};

class ProcedureHeadingNode {
public:
	explicit ProcedureHeadingNode(std::string _ident);
	std::string ident;
	std::vector<const FormalParameterNode*> formalParameters;
};

class FormalParameterNode {
public:
	std::vector<std::string> identList;
	const TypeNode* type;
};

class ProcedureBodyNode {
public:
	std::vector<const DeclarationNode*> declarations;
	std::vector<const StatementNode*> statements;
	std::string ident;
};