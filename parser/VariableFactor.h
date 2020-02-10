#pragma once

#include "Factor.h"
#include "Variable.h"

class VariableFactor : public Factor {
public:
	explicit VariableFactor(std::shared_ptr<const Variable> _variable);
	std::shared_ptr<const Variable> variable;
};
