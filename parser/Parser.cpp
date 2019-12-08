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

				// Node control


				//token = *scanner_->nextToken(); ----> ?
				if (token.getType() == TokenType::kw_begin)
				{
					token = *scanner_->nextToken();
					statement_sequence();
					// not next token
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
			return nullptr;
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
	Token token = *scanner_->peekToken();
	switch (token.getType())
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
		return nullptr;
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
	bool shouldMoveNext = false;
	Token token = *scanner_->peekToken();
	TokenType nextToken = TokenType::kw_const;
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
			case TokenType::kw_const:
				shouldMoveNext = true;
				nextToken = TokenType::const_ident;
				break;
			case TokenType::const_ident:
				shouldMoveNext = true;
				nextToken = TokenType::op_eq;
				break;
			case TokenType::op_eq:
				expression();
				// MOVES NEXT TOKEN
				shouldMoveNext = false;
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
	bool shouldMoveNext = false;
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
				shouldMoveNext = true;
				break;
			case TokenType::const_ident:
				shouldMoveNext = true;
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
				break;
			default:
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
	bool shouldMoveNext = false;
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
				shouldMoveNext = true;
				nextToken = TokenType::const_ident;
				break;
			case TokenType::const_ident:
				ident_list();
				nextToken = TokenType::colon;
				shouldMoveNext = true;
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
	if (token.getType() == TokenType::semicolon)
	{
		procedure_body();
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
	//MOVES NEXT TOKEN
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::op_eq || token.getType() == TokenType::op_neq || token.getType() == TokenType::op_lt || token.getType() == TokenType::op_leq || token.getType() == TokenType::op_gt || token.getType() == TokenType::op_geq)
	{
		simple_expression();
		//MOVES NEXT TOKEN
	}
	return nullptr;
}

const Node* Parser::simple_expression() {
	/*
	 ["+"|"-"] -> Optional
	 term
	 {("+"|"-" | "OR") term} -> repetition
	*/
	bool shouldRepeat = true;
	Token token = *scanner_->peekToken();
	term();
	// MOVES NEXT TOKEN
	while (shouldRepeat)
	{
		if (token.getType() == TokenType::op_plus || token.getType() == TokenType::op_minus || token.getType() == TokenType::op_or)
		{
			term();
			// MOVES NEXT TOKEN
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

	factor();
	// MOVES NEXT TOKEN

	// Control factor return nullptr or not

	Token token = *scanner_->peekToken();
	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		if (token.getType() == TokenType::op_times || token.getType() == TokenType::op_div || token.getType() == TokenType::op_mod || token.getType() == TokenType::op_and)
		{
			factor();
			// MOVES NEXT TOKEN
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
	Token token = *scanner_->nextToken();
	switch (token.getType())
	{
	case TokenType::const_ident:
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::period || token.getType() == TokenType::lbrack)
		{
			selector();
		}
		else {
			// NO SELECTOR
		}
		break;
	case TokenType::const_number:
		// NUMBER
		token = *scanner_->nextToken();
		break;
	case TokenType::lparen:
		expression();
		// MOVES NEXT TOKEN
		if (token.getType() != TokenType::rparen)
		{
			// SYNTAX ERROR
			return nullptr;
		}
		else if (token.getType() != TokenType::lparen) {
			token = *scanner_->nextToken();
		}
		break;
	case TokenType::op_not:
		factor();
		break;
	default:
		// SYNTAX ERROR
		return nullptr;
		break;
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
		// MOVES NEXT TOKEN
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
	Token token = *scanner_->peekToken();
	bool shouldRepeat = true;
	if (token.getType() == TokenType::const_ident)
	{
		while (shouldRepeat)
		{
			token = *scanner_->nextToken();
			if (token.getType() != TokenType::comma)
			{
				shouldRepeat = false;
			}
		}
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
	bool shouldMoveNext = false;
	TokenType nextToken = TokenType::kw_procedure;
	Token token = *scanner_->peekToken();
	while (shouldProceed)
	{
		if (token.getType() == nextToken)
		{
			if (shouldMoveNext)
			{
				token = *scanner_->nextToken();
			}
			switch (token.getType())
			{
			case TokenType::kw_procedure:
				nextToken = TokenType::const_ident;
				shouldMoveNext = true;
				break;
			case TokenType::const_ident:
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::lparen)
				{
					formal_parameters();
				}
				shouldProceed = false;
				break;
			default:
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

const Node* Parser::procedure_body() {
	/*
	declarations
	["BEGIN" StatementSequence] -> optional
	"END" ident
	*/
	declarations();
	Token token = *scanner_->nextToken();
	if (token.getType() == TokenType::kw_begin)
	{
		statement_sequence();
		token = *scanner_->nextToken();
	}
	if (token.getType() == TokenType::kw_end)
	{
		token = *scanner_->nextToken();
		if (token.getType() != TokenType::const_ident)
		{
			// SYNTAX ERROR
		}
		else {
			// check whether the names are equal on end and begin position
		}
	}
	else {
		// SYNTAX ERROR
	}

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

const Node* Parser::statement_sequence() {
	/*
	statement
	{";" statement} -> Repetation
	*/


	statement();

	//Token token = *scanner_->nextToken();
	//if (token.getType() == TokenType::semicolon)
	//{
	//	statement_sequence();
	//}
	//else {
	//	return nullptr;
	//}


	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		Token token = *scanner_->nextToken();
		if (token.getType() != TokenType::semicolon)
		{
			shouldRepeat = false;
		}
		else {
			statement();
		}
	}
	return nullptr;
}

const Node* Parser::statement() {
	/*
	[assignment | ProcedureCall | IfStatement | WhileStatement] -> Optional
	*/
	Token token = *scanner_->peekToken();
	switch (token.getType())
	{
	case TokenType::kw_if:
		if_statement();
		break;
	case TokenType::kw_while:
		while_statement();
		break;
	case TokenType::const_ident:
		assignment();
		if (assignment() == nullptr)
		{
			procedure_call();
		}
		break;
	default:
		// SYNTAX ERROR
		break;
	}
	return nullptr;
}

const Node* Parser::assignment() {
	/*
	ident selector ":=" expression
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::const_ident)
	{
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::period || token.getType() == TokenType::lbrack)
		{
			selector();
			// MOVES NEXT TOKEN
		}
		else {
			// NO SELECTOR
		}

		if (token.getType() != TokenType::op_becomes)
		{
			// SYNTAX ERROR
			return nullptr;
		}
		else {
			expression();
			// MOVES NEXT TOKEN
		}
	}
	else {
		return nullptr;
	}


	return nullptr;
}

const Node* Parser::procedure_call() {
	/*
	ident selector
	[ActualParameters] -> Optional
	*/
	Token token = *scanner_->nextToken();
	if (token.getType() != TokenType::lparen)
	{
		actual_parameters();
	}
	else {
		// SYNTAX ERROR
	}
	return nullptr;
}

const Node* Parser::if_statement() {
	/*
	"IF" expression "THEN" StatementSequence
	{"ELSIF" expression "THEN" StatementSequence} -> repetition
	["ELSE" StatementSequence] -> Optional
	"END"
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_if)
	{
		expression();
		//MOVES NEXT TOKEN
		if (token.getType() == TokenType::kw_then)
		{
			statement_sequence();
			bool shouldRepeat = true;
			while (shouldRepeat)
			{
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::kw_elsif)
				{
					expression();
					// MOVES NEXT TOKEN
					if (token.getType() == TokenType::kw_then)
					{
						statement_sequence();
					}
					else {
						shouldRepeat = false;
						// SYNTAX ERROR
					}
				}
				else {
					shouldRepeat = false;
				}
			}
			if (token.getType() == TokenType::kw_else)
			{
				statement_sequence();
				token = *scanner_->nextToken();
			}
			if (token.getType() != TokenType::kw_end) {
				// SYNTAX ERROR
			}
		}
		else {
			// SYNTAX ERROR
		}
	}
	return nullptr;
}

const Node* Parser::while_statement() {
	/*
	"WHILE" expression "DO" StatementSequence "END"
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_while)
	{
		expression();
		// MOVES NEXT TOKEN
		if (token.getType() == TokenType::kw_do)
		{
			statement_sequence();
			token = *scanner_->nextToken();
			if (token.getType() != TokenType::kw_end)
			{
				// SYNTAX ERROR
			}
		}
		else {
			// SYNTAX ERROR
		}
	}
	return nullptr;
}

const Node* Parser::actual_parameters() {
	/*
	"("
	[expression
	{ "," expression } -> Repetition
	] -> Optional
	")"
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::lparen)
	{
		if (expression() != nullptr) // MOVES NEXT TOKEN
		{
			bool shouldRepeat = true;
			while (shouldRepeat)
			{
				if (token.getType() == TokenType::comma)
				{
					expression();
					// MOVES NEXT TOKEN
				}
				else {
					shouldRepeat = false;
				}
			}
		}
	}
	else {
		// SYNTAX ERROR
		// return nullptr;
	}
	if (token.getType() != TokenType::rparen)
	{
		// SYNTAX ERROR
		// return nullptr;
	}
	return nullptr;
}

const Node* Parser::selector() {
	/*
	{"." ident | "[" expression "]"} -> repetition
	*/
	Token token = *scanner_->peekToken();
	TokenType nextToken = token.getType();
	bool shouldRepeat = true;
	bool shouldMoveNext = false;
	while (shouldRepeat)
	{
		if (shouldMoveNext)
		{
			token = *scanner_->nextToken();
		}
		if (token.getType() == nextToken)
		{
			switch (token.getType())
			{
			case TokenType::period:
				nextToken = TokenType::const_ident;
				shouldMoveNext = true;
				break;
			case TokenType::lbrack:
				expression();
				// MOVES NEXT TOKEN
				nextToken = TokenType::rbrack;
				shouldMoveNext = false;
				break;
			case TokenType::rbrack:
			case TokenType::const_ident:
				token = *scanner_->nextToken();
				shouldMoveNext = false;
				if (token.getType() == TokenType::period || token.getType() == TokenType::lbrack)
				{
					nextToken = token.getType();
				}
				else {
					// OTHER TOKEN
					shouldRepeat = false;
				}
				break;
			}
		}
		else {
			// SYNTAX ERROR
		}
	}


	return nullptr;
}
