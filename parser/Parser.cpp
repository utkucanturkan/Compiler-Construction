//
// Created by Michael Grossniklaus on 11/20/18.
//

#include <iostream>
#include "Parser.h"
#include <IdentToken.h>

Parser::Parser(Scanner* scanner, Logger* logger) :
	scanner_(scanner), logger_(logger) {
}

Parser::~Parser() = default;

const std::unique_ptr<const Node> Parser::parse() {
	return std::unique_ptr<const Node>(module());
}

const std::string Parser::ident() {	
	IdentToken* token = dynamic_cast<IdentToken*>(const_cast<Token*>(scanner_->peekToken()));
	return token->getValue();
}

const ModuleNode* Parser::module()
{
	//
	// "MODULE" ident ";" declarations ["BEGIN" StatementSequence] "END" ident "."
	//

	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_module)
	{
		FilePos filePos = token.getPosition();
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::const_ident)
		{
			std::string name = ident();
			ModuleNode moduleNode(name, filePos);
			token = *scanner_->nextToken();
			if (token.getType() == TokenType::semicolon)
			{
				token = *scanner_->nextToken();
				try
				{
					for (const DeclarationNode* declarationNode : declarations()) {
						moduleNode.addDeclaration(declarationNode);
					}
				}
				catch (const std::exception&)
				{
					logger_->error(token.getPosition(), "- SEMANTIC ERROR; A declaration name has been already used");
				}	
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::kw_begin)
				{
					token = *scanner_->nextToken();
					for (auto& statementNode : statement_sequence()) {
						moduleNode.addStatement(statementNode);
					}
				}
				if (token.getType() == TokenType::kw_end)
				{
					token = *scanner_->nextToken();
					if (token.getType() == TokenType::const_ident)
					{
						name = ident();
						if (moduleNode.checkName(name))
						{
							token = *scanner_->nextToken();
							if (token.getType() == TokenType::period)
							{
								return &moduleNode;
							}
							else
							{
								logger_->error(token.getPosition(), "- SYNTAX ERROR; \".\" is missing");
							}
						}
						else {
							logger_->error(token.getPosition(), "- SEMANTIC ERROR; Module name is not correct");
						}
					}
					else {
						logger_->error(token.getPosition(), "- SYNTAX ERROR; Module name is not identified");
					}
				}
				else {
					logger_->error(token.getPosition(), "- SYNTAX ERROR; \"END\" keyword is missing");
				}
			}
			else {
				logger_->error(token.getPosition(), "- SYNTAX ERROR; \";\" is missing");
			}
		}
		else {
			logger_->error(token.getPosition(), "- SYNTAX ERROR; Module name is not identified");
		}
	}
	else {
		return nullptr;
	}
}

const std::vector<const DeclarationNode*> Parser::declarations() {
	std::vector<const DeclarationNode*> declarations;
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
		for (auto& constNode : const_declarations()) {
			declarations.push_back(constNode);
		}
	case TokenType::kw_type:
		return type_declarations();
	case TokenType::kw_var:
		return var_declarations();
	case TokenType::kw_procedure:
		return procedure_declaration();
		break;
	}

	return declarations;
}

const std::vector<const ConstDeclarationNode*> Parser::const_declarations() {
	/*
		"CONST"
		{ident "=" expression ";"} -> repetation
	*/
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
				expression(); // MOVES NEXT TOKEN
				token = *scanner_->peekToken();
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
	Token token = *scanner_->peekToken();
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
	simple_expression(); // MOVES NEXT TOKEN
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::op_eq || token.getType() == TokenType::op_neq || token.getType() == TokenType::op_lt || token.getType() == TokenType::op_leq || token.getType() == TokenType::op_gt || token.getType() == TokenType::op_geq)
	{
		simple_expression(); // MOVES NEXT TOKEN
	}
	return nullptr;
}

const Node* Parser::simple_expression() {
	/*
	 ["+"|"-"] -> Optional
	 term
	 {("+"|"-" | "OR") term} -> repetition
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::op_plus || token.getType() == TokenType::op_minus) {
		token = *scanner_->peekToken();
	}

	bool shouldRepeat = true;
	term(); // MOVES NEXT TOKEN
	while (shouldRepeat)
	{
		token = *scanner_->peekToken();
		if (token.getType() == TokenType::op_plus || token.getType() == TokenType::op_minus || token.getType() == TokenType::op_or)
		{
			term(); // MOVES NEXT TOKEN
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
			factor(); // MOVES NEXT TOKEN
			token = *scanner_->peekToken();
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
			// MOVES NEXT TOKEN
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
		expression(); // MOVES NEXT TOKEN
		token = *scanner_->peekToken();
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
	Token token = *scanner_->peekToken();
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
		// nullptr
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
		expression(); // MOVES NEXT TOKEN
		token = *scanner_->peekToken();
		if (token.getType() == TokenType::kw_of)
		{
			type();
		}
		else {
			// SYNTAX ERROR
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
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::const_ident)
		{
			field_list();
			token = *scanner_->peekToken();
			bool shouldRepeat = true;
			while (shouldRepeat)
			{
				if (token.getType() == TokenType::semicolon)
				{
					field_list();
					token = *scanner_->peekToken();
				}
				else {
					shouldRepeat = false;
				}
			}
		}

		if (token.getType() != TokenType::kw_end)
		{
			// SYNTAX ERROR
		}
		else {
			*scanner_->nextToken();
		}
	}
	return nullptr;
}

const Node* Parser::field_list() {
	/*
	[IdentList ":" type] -> optional
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::const_ident)
	{
		token = *scanner_->nextToken();
		ident_list();
		token = *scanner_->peekToken();
		if (token.getType() == TokenType::colon)
		{
			token = *scanner_->nextToken();
			type();
		}
		else {
			// SYNTAX ERROR
		}
	}
	else {
		// NO FIELD_LIST
		return nullptr;
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
	bool shouldMoveNext = false;
	if (token.getType() == TokenType::const_ident)
	{
		TokenType nextToken = token.getType();
		while (shouldRepeat)	// REPETITION
		{
			if (shouldMoveNext)
			{
				token = *scanner_->nextToken();
			}
			if (token.getType() == nextToken)
			{
				if (token.getType() == TokenType::comma)
				{
					nextToken = TokenType::const_ident;
				}
				else if (token.getType() == TokenType::const_ident) {
					nextToken = TokenType::comma;
					shouldMoveNext = true;
				}
				else {
					shouldRepeat = false;
				}
			}
			else {
				shouldRepeat = false;
				if (nextToken == TokenType::comma)
				{
					// TOKEN MIGHT BE ")" 
					// NODE ?
				}
				else {
					// SYNTAX ERROR		
					// nextToken IS CONST_IDENT SO THAT TOKEN IS ","
					// AFTER "," TOKEN MUST BE AN IDENT IF NOT IT IS SYNTAX ERROR
					return nullptr;
				}
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
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_begin)
	{
		statement_sequence();
		token = *scanner_->peekToken();
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
		if (token.getType() == TokenType::kw_var || token.getType() == TokenType::const_ident)
		{
			if (fp_section() != nullptr) {	// MOVES NEXT TOKEN
				bool shouldRepeat = true;
				while (shouldRepeat)
				{
					token = *scanner_->peekToken();
					if (token.getType() == TokenType::semicolon)
					{
						if (token.getType() == TokenType::kw_var || token.getType() == TokenType::const_ident)
						{
							if (fp_section() == nullptr) {
								shouldRepeat = false;
							}
						}
					}
					else {
						shouldRepeat = false;
					}
				}
			}
			else {
				token = *scanner_->peekToken();
			}
		}
		else {
			token = *scanner_->nextToken();
		}

		if (token.getType() != TokenType::rparen)
		{
			// SYNTAX ERROR
		}
		else {
			*scanner_->nextToken();
		}
	}
	else {
		// SYNTAX ERROR
	}
	return nullptr;
}

const Node* Parser::fp_section() {
	/*
	["VAR"]					-> Optional
	IdentList ":" type
	*/
	Token token = *scanner_->peekToken();

	// Optional ["VAR"]
	if (token.getType() == TokenType::kw_var)
	{
		token = *scanner_->nextToken();
	}

	if (token.getType() == TokenType::const_ident)
	{
		ident_list();
		token = *scanner_->peekToken();
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
	{";" statement}				-> Repetation
	*/
	statement();
	bool shouldRepeat = true;
	while (shouldRepeat)	// REPETATION
	{
		Token token = *scanner_->peekToken();
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
		assignment();					// **!!**!!**!!
		procedure_call();				// **!!**!!**!!
		break;
	default:
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
			selector(); // MOVES NEXT TOKEN
			token = *scanner_->peekToken();
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
			expression(); // MOVES NEXT TOKEN
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
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::const_ident)
	{
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::period || token.getType() == TokenType::lbrack)
		{
			selector(); // MOVES NEXT TOKEN
			token = *scanner_->peekToken();
		}
		else {
			// NO SELECTOR
		}

		if (actual_parameters() == nullptr)	// MOVES NEXT TOKEN
		{
			// NO PARAMETERS
		}
		else {
			// PARAMETERS
			*scanner_->nextToken();
		}

	}
	else {
		// ERROR
		// 
		return nullptr;
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
		expression(); //MOVES NEXT TOKEN
		token = *scanner_->peekToken();
		if (token.getType() == TokenType::kw_then)
		{
			token = *scanner_->nextToken();
			statement_sequence();
			bool shouldRepeat = true;
			while (shouldRepeat) // REPETITION
			{
				token = *scanner_->peekToken();
				if (token.getType() == TokenType::kw_elsif)
				{
					expression(); // MOVES NEXT TOKEN
					token = *scanner_->peekToken();
					if (token.getType() == TokenType::kw_then)
					{
						token = *scanner_->nextToken();
						statement_sequence();
					}
					else {
						shouldRepeat = false;
						// SYNTAX ERROR
						return nullptr;
					}
				}
				else {
					shouldRepeat = false;
				}
			}
			if (token.getType() == TokenType::kw_else)
			{
				token = *scanner_->nextToken();
				statement_sequence();
				token = *scanner_->peekToken();
			}
			if (token.getType() != TokenType::kw_end) {
				// SYNTAX ERROR
				return nullptr;
			}
			else {
				*scanner_->nextToken();
			}
		}
		else {
			// SYNTAX ERROR
		}
	}
	else {
		// ERROR
		// It has to start "IF"
		return nullptr;
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
		expression(); // MOVES NEXT TOKEN
		token = *scanner_->peekToken();
		if (token.getType() == TokenType::kw_do)
		{
			token = *scanner_->nextToken();
			statement_sequence();
			token = *scanner_->peekToken();
			if (token.getType() != TokenType::kw_end)
			{
				// SYNTAX ERROR
				return nullptr;
			}
			else {
				*scanner_->nextToken();
			}
		}
		else {
			// SYNTAX ERROR
		}
	}
	else {
		// ERROR
		// While statement have to start "WHILE"
		return nullptr;
	}
	return nullptr;
}

const Node* Parser::actual_parameters() {
	/*
	"("
	[expression
		{ "," expression }				-> Repetition
	]									-> Optional
	")"
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::lparen)
	{
		// Optional
		if (expression() != nullptr) // MOVES NEXT TOKEN
		{
			bool shouldRepeat = true;
			// Repetiton
			while (shouldRepeat)
			{
				token = *scanner_->peekToken();
				if (token.getType() == TokenType::comma)
				{
					if (expression() == nullptr) // MOVES NEXT TOKEN
					{
						shouldRepeat = false;
					}
				}
				else {
					shouldRepeat = false;
				}
			}
		}
		else {
			token = *scanner_->peekToken();
		}

		if (token.getType() != TokenType::rparen)
		{
			// SYNTAX ERROR
			// actual_parameters have to finish ")"
			// return nullptr;
		}
		else {
			*scanner_->nextToken();
		}
	}
	else {
		// ERROR
		// actual_parameters have to start "("
		return nullptr;
	}
	return nullptr;
}

const Node* Parser::selector() {
	/*
	{"." ident | "[" expression "]"} -> repetition
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::period || token.getType() == TokenType::lbrack)
	{
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
					expression(); // MOVES NEXT TOKEN
					token = *scanner_->peekToken();
					shouldMoveNext = false;
					nextToken = TokenType::rbrack;
					break;
				case TokenType::rbrack:
				case TokenType::const_ident:
					//
					// SELECTOR NODE ?
					//
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
	}
	else {
		// ERROR
		// selector have to start with "." or "["
		return nullptr;
	}
	return nullptr;
}
