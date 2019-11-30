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

// logger_->error(token.getPosition(), " - Syntax Error");
const Node* Parser::module()
{
	/*
	"MODULE" ident ";"

	declarations

	["BEGIN" StatementSequence] -> Optional

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
						// SYNTAX ERROR
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
				token = *scanner_->nextToken();
				declarations();
				/*
				------ WARNING!!!!!!!---
				nextToken()
				*/
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::kw_begin)
				{
					statement_sequence();
					nextTokenType = TokenType::kw_end;
				}
				else if (token.getType() == TokenType::kw_end) {
					isEnd = true;
					nextTokenType = TokenType::const_ident;
				}
				else {
					// SYNTAX ERROR
					shouldProceed = false;
				}
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
	["CONST" {ident "=" expression ";"}]
	["TYPE" {ident "=" type ";"}]
	["VAR" {IdentList ":" type ";"}]
	{ProcedureDeclaration ";"}.
	*/
	switch (scanner_->peekToken()->getType())
	{
	case TokenType::kw_const:
		const_declarations();
		declarations();
		break;
	case TokenType::kw_type:
		type_declarations();
		declarations();
		break;
	case TokenType::kw_var:
		var_declarations();
		declarations();
		break;
	case TokenType::kw_procedure:
		procedure_declaration();
		declarations();
		break;
	default:
		break;
	}
	return nullptr;
}

const Node* Parser::const_declarations() {
	/*
		"CONST"
		{ident "=" expression ";"} -> repetation
	*/
	;
	bool shouldProceed = true;
	bool shouldMoveNext = true;
	Token token = *scanner_->peekToken();
	TokenType nextToken = TokenType::const_ident;
	while (shouldProceed)
	{
		if (shouldMoveNext)
		{
			token = *scanner_->nextToken();
		}
		if (token.getType() == nextToken)
		{
			switch (nextToken)
			{
			case TokenType::const_ident:
				nextToken = TokenType::op_eq;
				break;
			case TokenType::op_eq:
				expression();
				nextToken = TokenType::semicolon;
				break;
			case TokenType::semicolon:
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::const_ident)
				{
					shouldMoveNext = false;
					nextToken = TokenType::const_ident;
				}
				else {
					shouldProceed = false;
				}
				break;
			default:
				// SYNTAX ERROR
				shouldProceed = false;
				break;
			}
		}
		else {
			// SYNTAX ERROR
			shouldProceed = false;
		}
	}

	return nullptr;
}

const Node* Parser::type_declarations() {
	/*
		"TYPE"
		{ident "=" type ";"} -> repetition
	*/
	bool shouldMoveNext = true;
	bool shouldProceed = true;
	Token token = *scanner_->peekToken();
	TokenType nextToken = TokenType::kw_type;
	while (shouldProceed)
	{
		if (shouldMoveNext)
		{
			token = *scanner_->nextToken();
		}
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
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::const_ident)
				{
					shouldMoveNext = false;
					nextToken = TokenType::const_ident;
				}
				else {
					shouldProceed = false;
				}
				shouldProceed = false;
				break;
			default:
				// SYNTAX ERROR
				shouldProceed = false;
				break;
			}
		}
		else {
			// SYNTAX ERROR
			shouldProceed = false;
		}
	}
	return nullptr;
}

const Node* Parser::var_declarations() {
	/*
		"VAR"
		{IdentList ":" type ";"} -> repetation
	*/
	bool shouldProceed = true;
	bool shouldMoveNext = true;
	Token token = *scanner_->peekToken();
	TokenType nextToken = TokenType::kw_var;
	while (shouldProceed)
	{
		if (shouldMoveNext)
		{
			token = *scanner_->nextToken();
		}
		if (token.getType() == nextToken)
		{
			switch (nextToken)
			{
			case TokenType::kw_var:
				nextToken = TokenType::const_ident;
				break;
			case TokenType::const_ident:
				ident_list();
				nextToken = TokenType::colon;
				break;
			case TokenType::colon:
				type();
				nextToken = TokenType::semicolon;
				break;
			case TokenType::semicolon:
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::const_ident)
				{
					shouldMoveNext = false;
					nextToken = TokenType::const_ident;
				}
				else {
					shouldProceed = false;
				}
				break;
			default:
				// SYNTAX ERROR
				shouldProceed = false;
				break;
			}
		}
		else {
			// SYNTAX ERROR
			shouldProceed = false;
		}
	}
	return nullptr;
}

const Node* Parser::procedure_declaration() {
	/*
	ProcedureHeading ";" ProcedureBody
	*/
	procedure_heading();
	Token token = *scanner_->nextToken();
	if (token.getType() == TokenType::comma)
	{
		procedure_body();
	}
	else {
		// SYNTAX ERROR
	}
	return nullptr;
}

const Node* Parser::procedure_heading() {
	/*
	"PROCEDURE" ident
	[FormalParameters] -> Optional
	*/
	bool shouldProceed = true;
	TokenType nextToken = TokenType::kw_procedure;
	while (shouldProceed)
	{
		Token token = *scanner_->nextToken();
		switch (token.getType())
		{
		case TokenType::kw_procedure:
			nextToken = TokenType::const_ident;
			break;
		case TokenType::const_ident:
			shouldProceed = false;
			token = *scanner_->nextToken();
			if (token.getType() == TokenType::lparen)
			{
				formal_parameters();
			}
			break;
		default:
			shouldProceed = false;
			break;
		}
	}
	return nullptr;
}

const Node* Parser::procedure_body() {
	/*
	declarations 
	["BEGIN" StatementSequence] -> optional 
	"END" ident
	*/
	declarations();

	return nullptr;
}

const Node* Parser::formal_parameters() {
	/*
	"("
	[FPSection
		{";" FPSection} -> repetition
	] -> optional
	")"
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::lparen)
	{
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			token = *scanner_->nextToken();
			if (token.getType() == TokenType::kw_var || token.getType() == TokenType::const_ident)
			{
				fp_section();
				if (token.getType() != TokenType::semicolon)
				{
					shouldRepeat = false;
					// SYNTAX ERROR
				}
			}
			else {
				shouldRepeat = false;
			}
		}
		if (token.getType() != TokenType::rparen)
		{
			// SYNTAX ERROR
		}
	}
	return nullptr;
}

const Node* Parser::fp_section() {
	/*
	["VAR"] -> Optional
	IdentList ":" type
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_var || token.getType() == TokenType::const_ident)
	{
		ident_list();
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::colon)
		{
			type();
		}
		else {
			// SYNTAX ERROR
		}
	}
	else {
		// SYNTAX ERROR
	}

	return nullptr;
}

const Node* Parser::expression() {
	/*
	SimpleExpression
	[("=" | "#" | "<" | "<=" | ">" | ">=") SimpleExpression] -> Optional
	*/
	simple_expression();
	Token token = *scanner_->nextToken();
	if (token.getType() == TokenType::op_eq || token.getType() == TokenType::op_neq || token.getType() == TokenType::op_lt || token.getType() == TokenType::op_leq || token.getType() == TokenType::op_gt || token.getType() == TokenType::op_geq)
	{
		simple_expression();
	}
	return nullptr;
}

const Node* Parser::simple_expression() {
	/*
	 ["+"|"-"] -> Optional
	 term
	 {("+"|"-" | "OR") term} -> repetition
	*/
	Token token = *scanner_->nextToken();
	bool shouldRepeat = true;
	term();
	while (shouldRepeat)
	{
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::op_plus || token.getType() == TokenType::op_minus || token.getType() == TokenType::op_or)
		{
			term();
		}
		else {
			shouldRepeat = false;
		}
	}
	return nullptr;
}

const Node* Parser::term() {
	/*
	factor
	{("*" | "DIV" | "MOD" | "&") factor} -> repetition
	*/
	Token token = *scanner_->nextToken();
	bool shouldRepeat = true;
	factor();
	while (shouldRepeat)
	{
		if (token.getType() == TokenType::op_times || token.getType() == TokenType::op_div || token.getType() == TokenType::op_mod || token.getType() == TokenType::op_and)
		{
			factor();
		}
		else {
			shouldRepeat = false;
		}
	}
	return nullptr;
}

const Node* Parser::factor() {
	/*
	ident selector | integer | "(" expression ")" | "~" factor
	*/
	bool shouldRecursive = true;
	Token token = *scanner_->nextToken();
	if (token.getType() == TokenType::const_ident)
	{
		token = *scanner_->nextToken();
		switch (token.getType())
		{
		case TokenType::period:
			selector();
			break;
		case TokenType::const_number:
			// NUMBER
			break;
		case TokenType::lparen:
			expression();
			token = *scanner_->nextToken();
			if (token.getType() != TokenType::rparen)
			{
				shouldRecursive = false;
			}
			break;
		case TokenType::op_not:
			// ~
			break;
		default:
			shouldRecursive = false;
			break;
		}
		if (shouldRecursive)
		{
			factor();
		}
		else {
			// SYNTAX ERROR
		}
	}
	return nullptr;
}

const Node* Parser::selector() {
	/*
	{"." ident | "[" expression "]"} -> repetition
	*/
	bool shouldRepeat = false;
	while (shouldRepeat)
	{
		Token token = *scanner_->nextToken();
		switch (token.getType())
		{
		case TokenType::period:
			token = *scanner_->nextToken();
			if (token.getType() != TokenType::const_ident)
			{
				shouldRepeat = false;
			}
			break;
		case TokenType::lbrack:
			expression();
			token = *scanner_->nextToken();
			if (token.getType() != TokenType::rbrack)
			{
				shouldRepeat = false;
			}
			break;
		default:
			shouldRepeat = false;
			break;
		}
	}
	return nullptr;
}

const Node* Parser::type() {
	/*
	ident | ArrayType | RecordType
	*/
	Token token = *scanner_->nextToken();
	switch (token.getType())
	{
	case TokenType::const_ident:
		break;
	case TokenType::kw_array:
		array_type();
		break;
	case TokenType::kw_record:
		record_type();
		break;
	default:
		// SYNTAX ERROR
		break;
	}
	return nullptr;
}

const Node* Parser::array_type() {
	/*
	"ARRAY" expression "OF" type.
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_array)
	{
		expression();
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::kw_of)
		{
			type();
		}
	}
	return nullptr;
}

const Node* Parser::record_type() {

	/*
	"RECORD" FieldList
	{";" FieldList} -> repetition
	"END"
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_record)
	{
		field_list();
		token = *scanner_->nextToken();
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			token = *scanner_->nextToken();
			if (token.getType() == TokenType::semicolon)
			{
				field_list();
			}
			else {
				shouldRepeat = false;
			}
		}
		if (token.getType() != TokenType::kw_end)
		{
			// SYNTAX ERROR
		}
	}
	return nullptr;
}

const Node* Parser::field_list() {
	/*
	[IdentList ":" type] -> optional
	*/
	ident_list();
	Token token = *scanner_->nextToken();
	if (token.getType() == TokenType::colon)
	{
		type();
	}
	else {
		// SYNTAX ERROR
	}
	return nullptr;
}

const Node* Parser::ident_list() {
	/*
	ident
	{"," ident} -> repetition
	*/
	Token token = *scanner_->nextToken();
	bool shouldRepeat = true;
	if (token.getType() == TokenType::const_ident)
	{
		while (shouldRepeat)
		{
			if (token.getType() == TokenType::comma)
			{
				// IDENT
			}
			else {
				shouldRepeat = false;
			}
		}
	}
	else {
		// SYNTAX ERROR
	}
	return nullptr;
}

const Node* Parser::statement_sequence() {
	return nullptr;
}

