#pragma once

#include "ast/Expression/Factor.h"

class StringFactor : public Factor {
public:
	explicit StringFactor(std::string _value);
	std::string value;
};