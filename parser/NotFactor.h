#pragma once

#include "Factor.h"

class NotFactor: public Factor {
public:
	NotFactor(std::shared_ptr<const Factor> _factor);
	std::shared_ptr<const Factor> factor;
};