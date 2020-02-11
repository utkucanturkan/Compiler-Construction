#include "Variable.h"
#include "ConstVariable.h"
#include "TypeVariable.h"
#include "VarVariable.h"
#include "ProcedureVariable.h"

Variable::Variable(std::string _identifier) : identifier(_identifier) { }

Variable::Variable(std::string _identifier, std::shared_ptr<const Type> _type) : identifier(_identifier), type(_type) { }

ConstVariable::ConstVariable(std::string _identifier, std::shared_ptr<const Expression> _expression) : Variable(_identifier), expression(_expression) { 
	primitiveType = _expression->type;
}

TypeVariable::TypeVariable(std::string _identifier, std::shared_ptr<const Type> _type) : Variable(_identifier), type(_type) { }

VarVariable::VarVariable(std::string _identifier, std::shared_ptr<const Type> _type) : Variable(_identifier), type(_type) { }

ProcedureVariable::ProcedureVariable(std::string _identifier) : Variable(_identifier) { }

ProcedureHead::ProcedureHead(std::string _identifier) : identifier(_identifier) { }

ProcedureBody::ProcedureBody() { }
