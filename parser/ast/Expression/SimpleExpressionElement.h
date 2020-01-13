#pragma once

#include "Token.h"
#include "ast/Expression/Term.h"

class SimpleExpressionElement {
public:
	explicit SimpleExpressionElement(TokenType _sign, const Term* _term);
	TokenType sign;
	const Term* term;
};
