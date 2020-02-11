#pragma once

#include <memory>
#include <vector>

#include "Expression.h"
#include "Statement.h"

class Else {
public:
	explicit Else();
	std::vector<std::shared_ptr<const Statement>> statements;
};


class ElseIf {
public:
	explicit ElseIf(std::shared_ptr<const Expression> _expression);
	std::shared_ptr<const Expression> expression;
	std::vector<std::shared_ptr<const Statement>> statements;
};

class IfStatement :public Statement {
public:
	explicit IfStatement(std::shared_ptr<const Expression> _expression);
	std::shared_ptr<const Expression> expression;
	std::vector<std::shared_ptr<const Statement>> statements;
	std::vector<std::shared_ptr<const ElseIf>> elseIfNodes;
	std::shared_ptr<const Else> elseNode;
};

