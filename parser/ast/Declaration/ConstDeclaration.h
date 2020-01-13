#pragma once

#include <string>
#include "ast/Expression/Expression.h"
#include "ast/Declaration/Declaration.h"

class ConstDeclaration : public Declaration {
public:
	explicit ConstDeclaration(std::string _ident);
	const Expression* expression;
};