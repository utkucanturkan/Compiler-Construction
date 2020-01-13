#pragma once

#include "ast/Expression/Factor.h"

class IdentSelectorFactor : public Factor {
public:
	explicit IdentSelectorFactor(std::string _ident);
	std::string ident;
	std::vector<const Selector*> selectors;
};