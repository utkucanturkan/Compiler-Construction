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
	bool shouldProceed = false;
	TokenType nextTokenType;
	if (scanner_->peekToken()->getType() == TokenType::kw_module)
	{
		shouldProceed = true;
		nextTokenType = TokenType::const_ident;
	}
	while (shouldProceed)
	{
		Token token = *scanner_->nextToken();
		if (token.getType() == nextTokenType)
		{
			switch (nextTokenType)
			{
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
				logger_->error(token.getPosition() ," - Syntax Error");
				shouldProceed = false;
				break;
			}
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
		break;
	}

	for (int i = 1; i <= 5; i++)
	{
		scanner_->nextToken();
	}
	return nullptr;
}
const Node* Parser::const_declarations() {
	scanner_->peekToken();
	/*
	"CONST" {ident "=" expression ";"}
	*/
	return nullptr;
}

const Node* Parser::type_declarations() {
	return nullptr;
}

const Node* Parser::var_declarations() {
	return nullptr;
}

const Node* Parser::procedure_declaration() {
	return nullptr;
}

const Node* Parser::statement_sequence() {
	for (int i = 1; i <= 5; i++)
	{
		scanner_->nextToken();
	}
	return nullptr;
}

