#include "Expression.h"
#include "SimpleExpression.h"
#include "SimpleExpressionElement.h"

Expression::Expression(std::shared_ptr<const SimpleExpression> _lhs) : lhs(_lhs), op(TokenType::null), type(_lhs.get()->type) { }

Expression::Expression(std::shared_ptr<const SimpleExpression> _lhs, TokenType _op, std::shared_ptr<const SimpleExpression> _rhs) : lhs(_lhs), op(_op), rhs(_rhs), type(PrimitiveType::Boolean) { }

SimpleExpression::SimpleExpression() { }

SimpleExpressionElement::SimpleExpressionElement(const TokenType _sign, std::shared_ptr<const Term> _term) : sign(_sign), term(_term) { }
