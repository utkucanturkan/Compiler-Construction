#pragma once

#include <vector>

#include "Variable.h"
#include "Statement.h"

class ProcedureVariable : public Variable {
public:
	explicit ProcedureVariable(std::string _identifier);
	std::vector<std::shared_ptr<const Variable>> parameters;
	std::vector<std::shared_ptr<const Variable>> declarations;
	std::vector<std::shared_ptr<const Statement>> statements;
};

class ProcedureHead {
public:
	explicit ProcedureHead(std::string _identifier);
	const std::string identifier;
	std::vector<std::shared_ptr<const Variable>> parameters;
};

class ProcedureBody {
public:
	ProcedureBody();
	std::vector<std::shared_ptr<const Variable>> declarations;
	std::vector<std::shared_ptr<const Statement>> statements;
};