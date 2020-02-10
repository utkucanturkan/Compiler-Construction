#pragma once

#include "Factor.h"

class BooleanFactor : public Factor {
public:
	explicit BooleanFactor(const bool _value);
	const bool value;
};