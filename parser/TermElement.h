#pragma once

#include <memory>

#include "Factor.h"
#include "Token.h"

class TermElement {
public:
	explicit TermElement(const TokenType _sign, std::shared_ptr<const Factor> _factor);
	const TokenType sign;
	std::shared_ptr<const Factor> factor;
};
