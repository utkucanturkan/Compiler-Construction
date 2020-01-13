#pragma once

#include "ast/Selector/Selector.h"
#include "ast/Expression/Expression.h"

class IndexSelector : public Selector {
public:
	explicit IndexSelector(const Expression* _expression);
	const Expression* expression;
};