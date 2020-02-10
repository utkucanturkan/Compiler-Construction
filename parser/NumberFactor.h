#pragma once

#include "Factor.h"

class NumberFactor : public Factor {
public:
	NumberFactor(const int _value);
	const int value;
};