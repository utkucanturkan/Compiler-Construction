#pragma once

#include "ast/Type/Type.h"
#include "ast/Declaration/Declaration.h"

class VarDeclaration : public Declaration {
public:
	explicit VarDeclaration(std::string _ident);
	const Type* type;
};