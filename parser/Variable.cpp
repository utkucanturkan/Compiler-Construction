#include "Variable.h"
#include "ConstVariable.h"
#include "TypeVariable.h"
#include "VarVariable.h"
#include "ProcedureVariable.h"

Variable::Variable(std::string _identifier) : identifier(_identifier), Node(NodeType::variable) { }

Variable::Variable(std::string _identifier, std::shared_ptr<const Type> _type) : identifier(_identifier), type(_type), Node(NodeType::variable) { }


ConstVariable::ConstVariable(std::string _identifier, std::shared_ptr<const Expression> _expression) : Variable(_identifier), expression(_expression) {
	nodeType_ = NodeType::constant_reference;
	primitiveType = _expression->type;
}

TypeVariable::TypeVariable(std::string _identifier, std::shared_ptr<const Type> _type) : Variable(_identifier), type(_type) { 
	nodeType_ = NodeType::type_reference;
	primitiveType = _type->primitiveType;
}

VarVariable::VarVariable(std::string _identifier, std::shared_ptr<const Type> _type) : Variable(_identifier), type(_type) { 
	nodeType_ = NodeType::variable_reference;
	primitiveType = _type->primitiveType;
}

ProcedureVariable::ProcedureVariable(std::string _identifier) : Variable(_identifier) { 
	nodeType_ = NodeType::procedure;
}

ProcedureHead::ProcedureHead(std::string _identifier) : identifier(_identifier) { }

ProcedureBody::ProcedureBody() { }