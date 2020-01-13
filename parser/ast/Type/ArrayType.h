#pragma once

#include "ast/Type/Type.h"
#include "ast/Expression/Expression.h"

class ArrayType : public Type {
public:
	explicit ArrayType(const Expression* _expression, const Type* _type);
	const Expression* expression;
	const Type* type;
};