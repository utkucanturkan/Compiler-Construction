#include "Factor.h"
#include "BooleanFactor.h"
#include "StringFactor.h"
#include "NumberFactor.h"
#include "NotFactor.h"
#include "ExpressionFactor.h"
#include "VariableFactor.h"


Factor::Factor(PrimitiveType _type) : type(_type) { }

BooleanFactor::BooleanFactor(const bool _value) : Factor(PrimitiveType::Boolean), value(_value) { }

NumberFactor::NumberFactor(const int _value): Factor(PrimitiveType::Number), value(_value) { }

StringFactor::StringFactor(const std::string _value): Factor(PrimitiveType::String), value(_value) { }

NotFactor::NotFactor(std::shared_ptr<const Factor> _factor) : Factor(_factor->type), factor(_factor) { }

VariableFactor::VariableFactor(std::shared_ptr<const Variable> _variable) : Factor(_variable->primitiveType), variable(_variable) { }

ExpressionFactor::ExpressionFactor(std::shared_ptr<const Expression> _expression) : Factor(_expression->type), expression(_expression) { }