#pragma once

#include "ast/Type/Type.h"

class IdentType : public Type {
public:
	explicit IdentType(std::string _ident);
	std::string ident;
};