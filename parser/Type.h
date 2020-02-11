#pragma once

enum class PrimitiveType
{
	Array,
	Record,
	Number,
	String,
	Boolean
};

class Type { 
public:
	explicit Type(PrimitiveType _primitiveType);
	PrimitiveType primitiveType;
};