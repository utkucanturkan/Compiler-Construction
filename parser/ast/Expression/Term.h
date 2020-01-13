#pragma once

#include "ast/Expression/TermElement.h"
#include "ast/Expression/ExpressionType.h"

class Term {
public:
	ExpressionType type;
	std::vector<const TermElement*> elements;
};