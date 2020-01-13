#pragma once

#include "ast/Statement/Statement.h"
#include "ast/Statement/ElseIf.h"
#include "ast/Statement/Else.h"
#include "ast/Expression/Expression.h"

class IfStatement :public Statement {
public:
	const Expression* expression;
	std::vector<const Statement*> statements;
	std::vector<const ElseIf*> elseIfNodes;
	const Else* elseNode;
};