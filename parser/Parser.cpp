//
// Created by Michael Grossniklaus on 11/20/18.
//

#include <iostream>
#include "Parser.h"

Parser::Parser(Scanner* scanner, Logger* logger) :
	scanner_(scanner), logger_(logger) {
}

Parser::~Parser() = default;

const std::unique_ptr<const Node> Parser::parse() {
	return std::unique_ptr<const Node>(module());
}

const std::string ident() {

	return "";
}

const Node* Parser::module()
{
	/*
	"MODULE" ident ";"

	// -> GOTO DECLERATIONS METHOD

	declarations

	["BEGIN" StatementSequence]

	"END" ident "."
	*/

	std::string name;
	bool isEnd = false;
	bool shouldProceed = true;
	TokenType nextTokenType = TokenType::kw_module;

	while (shouldProceed)
	{
		Token token = *scanner_->nextToken();
		if (token.getType() == nextTokenType)
		{
			switch (nextTokenType)
			{
			case TokenType::kw_module:
				nextTokenType = TokenType::const_ident;
				break;
			case TokenType::const_ident:
				if (isEnd) {
					if (false /*name != token.getValue()*/) {
						logger_->error(token.getPosition(), " - Syntax Error");
						shouldProceed = false;
					}
					else {
						nextTokenType = TokenType::period;
					}
				}
				else {
					// name = token.getValue();
					nextTokenType = TokenType::semicolon;
				}
				break;
			case TokenType::semicolon:
				declarations();
				nextTokenType = TokenType::kw_begin;
				break;
			case TokenType::kw_begin:
				statement_sequence();
				nextTokenType = TokenType::kw_end;
				break;
			case TokenType::kw_end:
				isEnd = true;
				nextTokenType = TokenType::const_ident;
				break;
			case TokenType::period:
				nextTokenType = TokenType::eof;
				break;
			case TokenType::eof:
				shouldProceed = false;
				break;
			default:
				logger_->error(token.getPosition(), " - Syntax Error");
				shouldProceed = false;
				break;
			}
		}
		else {
			logger_->error(token.getPosition(), " - Syntax Error");
			shouldProceed = false;
		}
	}
	return nullptr;
}

const Node* Parser::declarations() {

	/*
	DECLARATION MIGHT BE CONST, TYPE, VAR, PROCEDURE
	-> GOTO CORRESPONDING METHOD BY NEXT TOKEN

	["CONST" {ident "=" expression ";"}]
	["TYPE" {ident "=" type ";"}]
	["VAR" {IdentList ":" type ";"}]
	{ProcedureDeclaration ";"}.
	*/
	Token token = *scanner_->nextToken();
	switch (token.getType())
	{
	case TokenType::kw_const:
		const_declarations();
		break;
	case TokenType::kw_type:
		type_declarations();
		break;
	case TokenType::kw_var:
		var_declarations();
		break;
	case TokenType::kw_procedure:
		procedure_declaration();
		break;
	default:
		logger_->error(token.getPosition(), " - Syntax Error");
		break;
	}
	return nullptr;
}
const Node* Parser::const_declarations() {
	/*
		"CONST" {ident "=" expression ";"}
	*/
	;
	bool shouldProceed = true;
	TokenType nextToken = TokenType::kw_const;
	while (shouldProceed)
	{
		Token token = *scanner_->nextToken();
		if (token.getType() == nextToken)
		{
			switch (nextToken)
			{
			case TokenType::kw_const:
				nextToken = TokenType::const_ident;
				break;
			case TokenType::const_ident:
				nextToken = TokenType::op_eq;
				break;
			case TokenType::op_eq:
				expression();
				nextToken = TokenType::semicolon;
				break;
			case TokenType::semicolon:
				shouldProceed = false;
				break;
			default:
				logger_->error(token.getPosition(), " - Syntax Error");
				shouldProceed = false;
				break;
			}
		}
		else {
			logger_->error(token.getPosition(), " - Syntax Error");
			shouldProceed = false;
		}
	}

	return nullptr;
}

const Node* Parser::type_declarations() {
	/*
		"TYPE" {ident "=" type ";"}
	*/
	bool shouldProceed = true;
	TokenType nextToken = TokenType::kw_type;
	while (shouldProceed)
	{
		Token token = *scanner_->nextToken();
		if (token.getType() == nextToken)
		{
			switch (nextToken)
			{
			case TokenType::kw_type:
				nextToken = TokenType::const_ident;
				break;
			case TokenType::const_ident:
				nextToken = TokenType::op_eq;
				break;
			case TokenType::op_eq:
				type();
				nextToken = TokenType::semicolon;
				break;
			case TokenType::semicolon:
				shouldProceed = false;
				break;
			default:
				logger_->error(token.getPosition(), " - Syntax Error");
				shouldProceed = false;
				break;
			}
		}
		else {
			logger_->error(token.getPosition(), " - Syntax Error");
			shouldProceed = false;
		}
	}
	return nullptr;
}

const Node* Parser::var_declarations() {
	/*
		["VAR" {IdentList ":" type ";"}]
		IdentList = ident {"," ident}
	*/
	bool shouldProceed = true;
	TokenType nextToken = TokenType::kw_var;
	while (shouldProceed)
	{
		Token token = *scanner_->nextToken();
		if (token.getType() == nextToken)
		{
			switch (nextToken)
			{
			case TokenType::kw_var:
				nextToken = TokenType::const_ident;
				break;
			case TokenType::const_ident:
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::comma)
				{
					nextToken = TokenType::const_ident;
				}
				else if (token.getType() == TokenType::colon) {
					type();
					nextToken = TokenType::semicolon;
				}
				break;
			case TokenType::semicolon:
				shouldProceed = false;
				break;
			default:
				logger_->error(token.getPosition(), " - Syntax Error");
				shouldProceed = false;
				break;
			}
		}
	}
	return nullptr;
}

const Node* Parser::procedure_declaration() {
	return nullptr;
}

const Node* Parser::expression() {
	/*
	SimpleExpression [("=" | "#" | "<" | "<=" | ">" | ">=") SimpleExpression]
	*/
	simple_expression();
	Token token = *scanner_->nextToken();
	if (token.getType() == TokenType::op_eq || token.getType() == TokenType::op_neq || token.getType() == TokenType::op_lt || token.getType() == TokenType::op_leq || token.getType() == TokenType::op_gt || token.getType() == TokenType::op_geq)
	{
		simple_expression();
	}
	else {
		logger_->error(token.getPosition(), " - Syntax Error");
	}
	return nullptr;
}

const Node* Parser::simple_expression() {
	Token token = *scanner_->nextToken();
	if (token.getType() == TokenType::op_plus || token.getType() == TokenType::op_minus)
	{
		term();
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::op_plus || token.getType() == TokenType::op_minus || token.getType() == TokenType::op_or)
		{
			term();
		}
		else {
			logger_->error(token.getPosition(), " - Syntax Error");
		}
	}
	else {
		logger_->error(token.getPosition(), " - Syntax Error");
	}
	return nullptr;
}

const Node* Parser::term() {
	/*
	factor {("*" | "DIV" | "MOD" | "&") factor}
	*/
	factor();
	Token token = *scanner_->nextToken();
	if (token.getType() == TokenType::op_times || token.getType() == TokenType::op_div || token.getType() == TokenType::op_mod || token.getType() == TokenType::op_and)
	{
		factor();
	}
	return nullptr;
}
const Node* Parser::factor() {
	/*
	ident selector | integer | "(" expression ")" | "~" factor
	*/
	Token token = *scanner_->nextToken();
	if (token.getType() == TokenType::const_ident)
	{
		
	}
	return nullptr;
}

const Node* Parser::type() {
	return nullptr;
}

const Node* Parser::statement_sequence() {
	return nullptr;
}

