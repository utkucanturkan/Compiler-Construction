#include "Term.h"
#include "TermElement.h"

Term::Term() { }

TermElement::TermElement(const TokenType _sign, std::shared_ptr<const Factor> _factor) : sign(_sign), factor(_factor) { }
