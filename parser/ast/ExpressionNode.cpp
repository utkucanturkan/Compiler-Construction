#include "ExpressionNode.h"
#include "../../scanner/Token.h"

ExpressionNode::ExpressionNode(const SimpleExpressionNode* _rhs) : rhs(_rhs) {

}

ExpressionNode::ExpressionNode(const SimpleExpressionNode* _rhs, TokenType _sign, const SimpleExpressionNode* _lhs) : rhs(_rhs), sign(_sign), lhs(_lhs) {

}