#pragma once

#include "ast/Node.h"

enum class PrimitiveType
{
	Array,
	Record,
	Number,
	String,
	Boolean
};

class Type: public Node { 
public:
	explicit Type(PrimitiveType _primitiveType);
	PrimitiveType primitiveType;
};