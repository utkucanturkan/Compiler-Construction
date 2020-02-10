#include "Statement.h"
#include "AssignmentStatement.h"
#include "WhileStatement.h"
#include "IfStatement.h"
#include "ProcedureCallStatement.h"

AssignmentStatement::AssignmentStatement(std::shared_ptr<const Variable> _variable, std::shared_ptr<const Expression> _expression) : expression(_expression), variable(_variable) { }

ProcedureCallStatement::ProcedureCallStatement(std::shared_ptr<const Variable> _variable): variable(_variable) {}

WhileStatement::WhileStatement(std::shared_ptr<const Expression> _expression) : expression(_expression) {}

IfStatement::IfStatement(std::shared_ptr<const Expression> _expression): expression(_expression) { }

ElseIf::ElseIf() {}

Else::Else() {}