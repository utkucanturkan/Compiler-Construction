#pragma once

#include "ast/Expression/Factor.h"
#include "Token.h"

class TermElement {
public:
	explicit TermElement(TokenType _sign, const Factor* _factor);
	TokenType sign;
	const Factor* factor;
};