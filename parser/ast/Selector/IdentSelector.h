#pragma once

#include <string>

#include "ast/Selector/Selector.h"
#include "ast/Expression/Expression.h"

class IdentSelector : public Selector {
public:
	explicit IdentSelector(std::string _ident);
	const std::string ident;
};