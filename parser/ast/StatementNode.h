#pragma once

#include "SelectorNode.h"

class StatementNode {

};

class IfStatementNode :public StatementNode {
public:
	const ExpressionNode* expression;
	std::vector<const StatementNode*> statements;
	std::vector<const ElseIfNode*> elseIfNodes;
	const ElseNode* elseNode;
};

class ElseIfNode {
public:
	const ExpressionNode* expression;
	std::vector<const StatementNode*> statements;
};

class ElseNode {
public:
	std::vector<const StatementNode*> statements;
};

class WhileStatementNode : public StatementNode {
public:
	const ExpressionNode* expression;
	std::vector<const StatementNode*> statements;
};

class AssignmentStatementNode : public StatementNode {
public:
	std::string ident;
	std::vector<const SelectorNode*> selectors;
	const ExpressionNode* expression;
};

class ProcedureCallStatementNode : public StatementNode {
public:
	explicit ProcedureCallStatementNode(std::string _procdeureName);

	std::string procedureName;
	std::vector<const SelectorNode*> selectors;
	std::vector<const ExpressionNode*> actualParameters;
};