#pragma once

#include "ast/Statement/Statement.h"
#include "ast/Expression/Expression.h"
#include "ast/Selector/Selector.h"

class AssignmentStatement : public Statement {
public:
	std::string ident;
	std::vector<const Selector*> selectors;
	const Expression* expression;
};