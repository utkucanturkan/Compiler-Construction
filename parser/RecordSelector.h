#pragma once

#include "Selector.h"
#include "Variable.h"

class RecordSelector : public Selector {
public:
	explicit RecordSelector(std::shared_ptr<const Variable> _variable);
	std::shared_ptr<const Variable> variable;
};