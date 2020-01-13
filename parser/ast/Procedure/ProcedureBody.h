#pragma once

#include "ast/Declaration/Declaration.h"
#include "ast/Statement/Statement.h"

class ProcedureBody {
public:
	std::vector<const Declaration*> declarations;
	std::vector<const Statement*> statements;
	std::string ident;
};