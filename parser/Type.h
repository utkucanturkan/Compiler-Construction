#pragma once

enum class PrimitiveType
{
	Null,
	Number,
	String,
	Boolean
};

class Type { 
public:
	explicit Type(PrimitiveType _primitiveType);
	PrimitiveType primitiveType;
};