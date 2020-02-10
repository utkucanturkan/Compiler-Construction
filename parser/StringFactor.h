#pragma once

#include "Factor.h"
#include <string>

class StringFactor : public Factor {
public:
	explicit StringFactor(const std::string _value);
	const std::string value;
};