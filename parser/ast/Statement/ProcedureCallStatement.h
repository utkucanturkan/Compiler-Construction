#pragma once


#include "ast/Statement/Statement.h"
#include "ast/Expression/Expression.h"
#include "ast/Selector/Selector.h"

class ProcedureCallStatement : public Statement {
public:
	explicit ProcedureCallStatement(std::string _procdeureName);

	std::string procedureName;
	std::vector<const Selector*> selectors;
	std::vector<const Expression*> actualParameters;
};