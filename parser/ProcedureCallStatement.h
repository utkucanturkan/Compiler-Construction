#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Expression.h"
#include "Statement.h"
#include "Variable.h"

class ProcedureCallStatement : public Statement {
public:
	explicit ProcedureCallStatement(std::shared_ptr<const Variable> _variable);
	std::shared_ptr<const Variable> variable;
	std::vector<std::shared_ptr<const Expression>> actualParameters;
};