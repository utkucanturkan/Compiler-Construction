#pragma once

#include "ast/Expression/Factor.h"

class NotFactor :public Factor {
public:
	explicit NotFactor(const Factor* _factor);
	const Factor* factor;
};
