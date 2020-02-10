#pragma once

#include <memory>

#include "Token.h"
#include "Term.h"

class SimpleExpressionElement {
public:
	explicit SimpleExpressionElement(const TokenType _sign, std::shared_ptr<const Term> _term);
	const TokenType sign;
	std::shared_ptr<const Term> term;
};