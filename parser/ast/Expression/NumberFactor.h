#pragma once

#include "ast/Expression/Factor.h"

class NumberFactor : public Factor {
public:
	explicit NumberFactor(int _integer);
	int integer;
};