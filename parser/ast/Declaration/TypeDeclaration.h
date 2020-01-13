#pragma once

#include "ast/Type/Type.h"
#include "ast/Declaration/Declaration.h"

class TypeDeclaration : public Declaration {
public:
	explicit TypeDeclaration(std::string _ident);
	const Type* type;
};