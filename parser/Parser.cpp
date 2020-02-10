//
// Created by Michael Grossniklaus on 11/20/18.
//

#include <iostream>
#include "Parser.h"
#include "IdentToken.h"
#include "StringToken.h"
#include "NumberToken.h"

#include "StringFactor.h"
#include "NumberFactor.h"
#include "BooleanFactor.h"
#include "ExpressionFactor.h"
#include "NotFactor.h"
#include "VariableFactor.h"

Parser::Parser(Scanner* scanner, Logger* logger) :
	scanner_(scanner), logger_(logger) {
}

Parser::~Parser() = default;

const std::unique_ptr<const Module> Parser::parse() {
	return module();
}

const std::string Parser::ident() {
	if (token_->getType() == TokenType::const_ident) {
		return static_cast<const IdentToken*>(token_.get())->getValue();
	}
	return "";
}

std::unique_ptr<const Module> Parser::module()
{
	// "MODULE" ident ";" declarations ["BEGIN" StatementSequence] "END" ident "."
	token_ = scanner_->nextToken();
	if (token_->getType() == TokenType::kw_module)
	{
		token_ = scanner_->nextToken();
		std::string identifier = ident();
		if (!identifier.empty())
		{
			token_ = scanner_->nextToken();
			if (token_->getType() == TokenType::semicolon)
			{
				token_ = scanner_->nextToken();

				auto _module = std::make_unique<Module>(identifier);
				logger_->info("", identifier + " module is created.");
				for (auto& declaration : declarations()) {
					if (!_module.get()->addDeclaration(declaration))
					{
						logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Same declaration name");
					}
					else {
						logger_->info("", "A declaration is added to " + identifier + " Module.");
					}
				}

				if (token_->getType() == TokenType::kw_begin)
				{
					token_ = scanner_->nextToken();
					for (auto& statement : statement_sequence()) {
						_module.get()->addStatement(statement);
						logger_->info("", "A statement is added to " + identifier + " Module.");
					}
				}
				if (token_->getType() == TokenType::kw_end)
				{
					token_ = scanner_->nextToken();
					identifier = ident();
					if (!identifier.empty())
					{
						if (_module.get()->checkName(identifier))
						{
							token_ = scanner_->nextToken();
							if (token_->getType() == TokenType::period)
							{
								token_ = scanner_->nextToken();
								return _module;
							}
							else
							{
								logger_->error(token_->getPosition(), "- SYNTAX ERROR; \".\" is missing.");
							}
						}
						else {
							logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Module name is not same at the end.");
						}
					}
					else {
						logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Module name is missing.");
					}
				}
				else {
					logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"END\" keyword is missing");
				}
			}
			else {
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; \";\" is missing");
			}
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; MODULE idetifier is not valid.");
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"MODULE\" keyword is missing.");
	}
	return nullptr;
}

const std::vector<std::shared_ptr<const Variable>> Parser::declarations() {
	// ["CONST" {ident "=" expression ";"}]
	// ["TYPE" {ident "=" type ";"}]
	// ["VAR" {IdentList ":" type ";"}]
	// {ProcedureDeclaration ";"}.
	std::vector<std::shared_ptr<const Variable>> declarationList;
	while (token_->getType() == TokenType::kw_const || token_->getType() == TokenType::kw_type || token_->getType() == TokenType::kw_var || token_->getType() == TokenType::kw_procedure)
	{
		if (token_->getType() == TokenType::kw_const)
		{
			for (auto& constantDeclaration : const_declarations()) {
				if (constantDeclaration != nullptr)
				{
					declarationList.emplace_back(constantDeclaration);
				}
			}
		}

		if (token_->getType() == TokenType::kw_type)
		{
			for (auto& typeDeclaration : type_declarations()) {
				if (typeDeclaration != nullptr)
				{
					declarationList.emplace_back(typeDeclaration);
				}
			}
		}

		if (token_->getType() == TokenType::kw_var)
		{
			for (auto& varDeclaration : var_declarations()) {
				if (varDeclaration != nullptr)
				{
					declarationList.emplace_back(varDeclaration);
				}
			}
		}

		if (token_->getType() == TokenType::kw_procedure)
		{
			auto procedureDeclaration = procedure_declaration();
			if (procedureDeclaration != nullptr)
			{
				declarationList.emplace_back(procedureDeclaration);
			}
		}
	}
	return declarationList;
}

const std::vector<std::shared_ptr<const ConstVariable>> Parser::const_declarations() {
	// "CONST" {ident "=" expression ";"}
	std::vector<std::shared_ptr<const ConstVariable>> constantDeclarations;
	token_ = scanner_->nextToken();
	std::string identifier = ident();
	if (!identifier.empty())
	{
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			token_ = scanner_->nextToken();
			if (token_->getType() == TokenType::op_eq)
			{
				token_ = scanner_->nextToken();
				auto _expression = expression();
				if (_expression.get() != nullptr)
				{
					if (token_->getType() == TokenType::semicolon)
					{
						auto constantVariable = std::make_shared<const ConstVariable>(identifier, _expression);
						constantDeclarations.emplace_back(constantVariable);

						logger_->info("CONST Declaration", "Name: " + identifier);
						token_ = scanner_->nextToken();
						identifier = ident();
						if (identifier.empty())
						{
							shouldRepeat = false;
						}
					}
					else {
						logger_->error(token_->getPosition(), "- SYNTAX ERROR; \";\" is missing.");
						shouldRepeat = false;
					}
				}
				else {
					// No Expression
					shouldRepeat = false;
				}
			}
			else {
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"=\" is missing.");
				shouldRepeat = false;
			}
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; Const declaration name is not valid.");
	}
	return constantDeclarations;
}

const std::vector<std::shared_ptr<const TypeVariable>> Parser::type_declarations() {
	// "TYPE" {ident "=" type ";"} -> repetition
	std::vector<std::shared_ptr<const TypeVariable>> typeDeclarations;
	bool shouldProceed = true;
	token_ = scanner_->nextToken();
	std::string identifier = ident();
	if (!identifier.empty())
	{
		while (shouldProceed)
		{
			token_ = scanner_->nextToken();
			if (token_->getType() == TokenType::op_eq)
			{
				token_ = scanner_->nextToken();
				auto _type = type();
				if (_type.get() != nullptr)
				{
					if (token_->getType() == TokenType::semicolon)
					{
						auto typeDeclaration = std::make_shared<const TypeVariable>(identifier, _type);
						typeDeclarations.emplace_back(typeDeclaration);
						logger_->info("TYPE Declaration", "Name: " + identifier);

						token_ = scanner_->nextToken();
						identifier = ident();
						if (identifier.empty())
						{
							shouldProceed = false;
						}
					}
					else {
						shouldProceed = false;
						logger_->error(token_->getPosition(), "- SYNTAX ERROR; \";\ is missing.");
					}
				}
				else {
					// No Type
					shouldProceed = false;
				}
			}
			else {
				shouldProceed = false;
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"=\" is missing.");
			}
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; Type declaration name is not valid.");
	}
	return typeDeclarations;
}

const std::vector<std::shared_ptr<const VarVariable>> Parser::var_declarations() {
	// "VAR" {IdentList ":" type ";"}
	std::vector<std::shared_ptr<const VarVariable>> varDeclarations;
	token_ = scanner_->nextToken();
	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		std::vector<std::string> identifier_list = ident_list();
		if (identifier_list.size() == 0)
		{
			if (token_->getType() == TokenType::colon)
			{
				token_ = scanner_->nextToken();
				auto _type = type();
				if (_type.get() != nullptr)
				{
					if (token_->getType() == TokenType::semicolon)
					{
						for (auto const& identifier : identifier_list) {
							auto varVariable = std::make_shared<const VarVariable>(identifier, _type);
							varDeclarations.emplace_back(varVariable);
						}
						token_ = scanner_->nextToken();
					}
					else {
						shouldRepeat = false;
						logger_->error(token_->getPosition(), "- SYNTAX ERROR; \";\ is missing");
					}
				}
				else {
					// No Type
					shouldRepeat = false;
				}
			}
			else {
				shouldRepeat = false;
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; \":\" is missing");
			}
		}
		else {
			// No identifiers
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; No valid identifier");
			shouldRepeat = false;
		}
	}
	return varDeclarations;
}

std::shared_ptr<const ProcedureVariable> Parser::procedure_declaration() {
	// ProcedureHeading ";" ProcedureBody
	procedure_heading();
	if (token_->getType() == TokenType::semicolon)
	{
		token_ = scanner_->nextToken();
		procedure_body();
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \";\" is missing");
	}
	return nullptr;
}

std::shared_ptr<const Expression> Parser::expression() {
	// SimpleExpression [("=" | "#" | "<" | "<=" | ">" | ">=") SimpleExpression] -> Optional

	auto lhs = simple_expression();
	if (lhs != nullptr)
	{
		if (token_->getType() == TokenType::op_eq || token_->getType() == TokenType::op_neq || token_->getType() == TokenType::op_lt || token_->getType() == TokenType::op_leq || token_->getType() == TokenType::op_gt || token_->getType() == TokenType::op_geq)
		{
			auto operand = token_->getType();
			token_ = scanner_->nextToken();
			auto rhs = simple_expression();
			if (rhs != nullptr)
			{
				return std::shared_ptr<const Expression>(new Expression(lhs, operand, rhs));
			}
			else {
				// No rhs simple expression
			}
		}
		else {
			return std::shared_ptr<const Expression>(new Expression(lhs));
		}
	}
	else {
		// No simple Expression
	}
	return nullptr;
}

std::shared_ptr<const SimpleExpression> Parser::simple_expression() {
	// ["+" | "-"] term {("+" | "-" | "OR") term}
	auto operand = TokenType::null;
	if (token_->getType() == TokenType::op_plus || token_->getType() == TokenType::op_minus) {
		operand = token_->getType();
		token_ = scanner_->nextToken();
	}
	auto _term = term();
	if (_term != nullptr)
	{
		if (operand != TokenType::null && _term->type != PrimitiveType::Number)
		{
			// Error: SEMANTIC + and - operands can not use without numbers
			return nullptr;
		}
		auto simpleExpression = std::make_shared<SimpleExpression>();
		simpleExpression->type = _term->type;
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			if (_term->type == simpleExpression->type)
			{
				auto element = std::make_shared<const SimpleExpressionElement>(operand, _term);
				simpleExpression->elements.emplace_back(element);
				if (token_->getType() == TokenType::op_plus || token_->getType() == TokenType::op_minus || token_->getType() == TokenType::op_or)
				{
					if ((_term->type == PrimitiveType::Number && (token_->getType() == TokenType::op_plus || token_->getType() == TokenType::op_minus)) || (_term->type == PrimitiveType::Boolean && token_->getType() == TokenType::op_or))
					{
						operand = token_->getType();
						token_ = scanner_->nextToken();
						_term = term();
						if (_term == nullptr)
						{
							shouldRepeat = false;
						}
					}
					else {
						// Error: SEMANTIC operand and term type can not matched
						shouldRepeat = false;
					}
				}
				else {
					return simpleExpression;
				}
			}
			else {
				// Error: SEMANTIC; factor type must be uniform
				shouldRepeat = false;
			}
		}
	}
	else {
		// No term
	}
	return nullptr;
}

std::shared_ptr<const Term> Parser::term() {
	// factor {("*" | "DIV" | "MOD" | "&") factor} -> repetition
	auto _factor = factor();
	if (_factor != nullptr)
	{
		auto term = std::make_shared<Term>();
		term->type = _factor->type;		
		auto operand = TokenType::null;
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			if (_factor->type == term->type)
			{
				auto element = std::make_shared<const TermElement>(operand, _factor);
				term->elements.emplace_back(element);
				if (token_->getType() == TokenType::op_times || token_->getType() == TokenType::op_div || token_->getType() == TokenType::op_mod || token_->getType() == TokenType::op_and)
				{
					if ((_factor->type == PrimitiveType::Number && (token_->getType() == TokenType::op_times || token_->getType() == TokenType::op_div || token_->getType() == TokenType::op_mod)) || (_factor->type == PrimitiveType::Boolean && token_->getType() == TokenType::op_and))
					{
						operand = token_->getType();
						token_ = scanner_->nextToken();
						_factor = factor();
						if (_factor == nullptr)
						{
							shouldRepeat = false;
						}
					}
					else {
						// Error: SEMANTIC operand and term type can not matched
						shouldRepeat = false;
					}
				}
				else {
					return term;
				}
			}
			else {
				// Error: SEMANTIC; factor type must be uniform
				shouldRepeat = false;
			}
		}
	}
	else {
		// Factor is not valid
	}
	return nullptr;
}

std::shared_ptr<const Factor> Parser::factor() {
	// ident selector | integer | "(" expression ")" | "~" factor	
	std::string identifier = ident();
	if (!identifier.empty())
	{
		// control identifier is a variable that has been already specied.
		// control identifier or its selector whether is in symbol table or not.
		token_ = scanner_->nextToken();
		auto _variable = std::make_shared<const Variable>(identifier);
		selector();
		return std::make_shared<const VariableFactor>(_variable);
	}
	else {
		if (token_->getType() == TokenType::const_string)
		{
			const std::string str = static_cast<const StringToken*>(token_.get())->getValue();
			token_ = scanner_->nextToken();
			return std::make_shared<const StringFactor>(str);
		}
		else if (token_->getType() == TokenType::const_number) {
			const int number = static_cast<const NumberToken*>(token_.get())->getValue();
			token_ = scanner_->nextToken();
			return std::make_shared<const NumberFactor>(number);
		}
		else if (token_->getType() == TokenType::const_true || token_->getType() == TokenType::const_false) {
			const bool boolean = (token_->getType() == TokenType::const_true) ? true : false;
			token_ = scanner_->nextToken();
			return std::make_shared<const BooleanFactor>(boolean);
		}
		else if (token_->getType() == TokenType::lparen) {
			token_ = scanner_->nextToken();
			auto _expression = expression();
			if (_expression != nullptr)
			{
				if (_expression->type == PrimitiveType::Number)
				{
					if (token_->getType() == TokenType::rparen)
					{
						token_ = scanner_->nextToken();
						return std::make_shared<const ExpressionFactor>(_expression);
					}
					else {
						logger_->error(token_->getPosition(), "- SYNTAX ERROR; \")\" is missing.");
					}
				}
				else {
					// The expression should be number
				}
			}
			else {
				// No expression
			}
		}
		else if (token_->getType() == TokenType::op_not) {
			token_ = scanner_->nextToken();
			auto _factor = factor();
			if (_factor != nullptr)
			{
				if (_factor->type == PrimitiveType::Boolean)
				{
					return std::make_shared<const NotFactor>(_factor);
				}
				else {
					// Inner factor is not boolean
				}
			}
			else {
				// No factor
			}
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; Factor is not valid.");
		}
	}
	return nullptr;
}

std::shared_ptr<const Type> Parser::type() {
	// ident | ArrayType | RecordType
	std::string name = ident();
	if (!name.empty())
	{
		// INTEGER - CHAR - BOOLEAN - LONGINT
		token_ = scanner_->nextToken();
	}
	else if (token_->getType() == TokenType::kw_array) {
		array_type();
	}
	else if (token_->getType() == TokenType::kw_record) {
		record_type();
	}
	logger_->error(token_->getPosition(), "- SYNTAX ERROR; type is not valid.");
	return nullptr;
}

const Node* Parser::array_type() {
	// "ARRAY" expression "OF" type.
	token_ = scanner_->nextToken();
	expression();
	if (token_->getType() == TokenType::kw_of)
	{
		token_ = scanner_->nextToken();
		type();
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"OF\" keyword is missing.");
	}
	return nullptr;
}

const Node* Parser::record_type() {
	// "RECORD" FieldList {";" FieldList} "END"
	token_ = scanner_->nextToken();
	field_list();
	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		if (token_->getType() != TokenType::semicolon)
		{
			token_ = scanner_->nextToken();
			field_list();
		}
		else {
			shouldRepeat = false;
		}
	}
	if (token_->getType() == TokenType::kw_end)
	{
		token_ = scanner_->nextToken();
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"END\" keyword is missing.");
	}
	return nullptr;
}

const Node* Parser::field_list() {
	// [IdentList ":" type] -> optional
	ident_list();
	if (token_->getType() == TokenType::colon)
	{
		token_ = scanner_->nextToken();
		type();
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \":\" is missing");
	}
	return nullptr;
}

const std::vector<std::string> Parser::ident_list() {
	// ident {"," ident} -> repetition
	std::vector<std::string> identList;
	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		std::string name = ident();
		if (!name.empty())
		{
			identList.push_back(name);
			token_ = scanner_->nextToken();
			if (token_->getType() != TokenType::comma)
			{
				shouldRepeat = false;
			}
			else {
				token_ = scanner_->nextToken();
			}
		}
		else {
			shouldRepeat = false;
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; Identifier is not valid.");
		}
	}
	//if (identList.size() <= 0)
	//{
	//	logger_->error(token_->getPosition(), "- SYNTAX ERROR; Identifier list is null.");
	//}
	return identList;
}

const Node* Parser::procedure_heading() {
	// "PROCEDURE" ident [FormalParameters]
	token_ = scanner_->nextToken();
	std::string name = ident();
	if (!name.empty())
	{
		token_ = scanner_->nextToken();
		formal_parameters();
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; Procedure name is not valid.");
	}
	return nullptr;
}

const Node* Parser::procedure_body() {
	// declarations ["BEGIN" StatementSequence] "END" ident
	declarations();
	if (token_->getType() == TokenType::kw_begin)
	{
		token_ = scanner_->nextToken();
		statement_sequence();
	}
	if (token_->getType() == TokenType::kw_end)
	{
		token_ = scanner_->nextToken();
		std::string name = ident();
		if (!name.empty())
		{
			token_ = scanner_->nextToken();
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; Procedure name is not same or identified.");
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"END\" keyword is missing.");
	}
	return nullptr;
}

const Node* Parser::formal_parameters() {
	// "(" [FPSection {";" FPSection} ] ")"
	if (token_->getType() == TokenType::lparen)
	{
		token_ = scanner_->nextToken();
		fp_section();
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			if (token_->getType() == TokenType::semicolon)
			{
				token_ = scanner_->nextToken();
				fp_section();
			}
			else {
				shouldRepeat = false;
			}
		}
		if (token_->getType() == TokenType::rparen)
		{
			token_ = scanner_->nextToken();
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \")\" is missing.");
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"(\" is missing.");
	}
	return nullptr;
}

const Node* Parser::fp_section() {
	// ["VAR"]	IdentList ":" type
	if (token_->getType() == TokenType::kw_var)
	{
		token_ = scanner_->nextToken();
	}
	ident_list();
	if (token_->getType() == TokenType::colon)
	{
		token_ = scanner_->nextToken();
		type();
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \":\" is missing.");
	}
	return nullptr;
}

const std::vector<std::shared_ptr<const Statement>> Parser::statement_sequence() {
	// statement {";" statement}
	std::vector<std::shared_ptr<const Statement>> statementList;
	auto s = statement();
	if (s != nullptr)
	{
		bool shouldProceed = true;
		while (shouldProceed)
		{
			if (token_->getType() == TokenType::semicolon)
			{
				token_ = scanner_->nextToken();
				auto s = statement();
				if (s == nullptr)
				{
					shouldProceed = false;
				}
			}
			else {
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; \";\" is missing.");
				shouldProceed = false;
			}
		}
	}
	else {

	}
	return statementList;
}

const Node* Parser::statement() {
	// [assignment | ProcedureCall | IfStatement | WhileStatement]
	if (token_->getType() == TokenType::kw_if)
	{
		if_statement();
	}
	else if (token_->getType() == TokenType::kw_while) {
		while_statement();
	}
	else {
		//std::string name = ident();
		//if (!name.empty())
		//{
		//	token_ = scanner_->nextToken();
		//	selector();

			// if the identifier has selector 
			// YES - Go to the corresponding method if the selector is variable (assignment) or procedure (procedure_call)
			// NO - Go to the corresponding method if the identifier is variable (assignment) or procedure (procedure_call)

			//assignment();
		procedure_call();
		//}
	}
	return nullptr;
}

const Node* Parser::assignment() {
	// ident selector ":=" expression
	//std::string name = ident();
	//if (!name.empty())
	//{
	//	token_ = scanner_->nextToken();
	//	selector();
	if (token_->getType() != TokenType::op_becomes)
	{
		token_ = scanner_->nextToken();
		expression();
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \":=\" is missing.");
	}
	//}
	//else {
	//	logger_->error(token_->getPosition(), "- SYNTAX ERROR; No valid identifier.");
	//}
	return nullptr;
}

const Node* Parser::procedure_call() {
	// ident selector [ActualParameters]
	std::string name = ident();
	if (!name.empty())
	{
		token_ = scanner_->nextToken();
		selector();
		actual_parameters();
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; No valid identifier.");
	}
	return nullptr;
}

const Node* Parser::actual_parameters() {
	// "(" [expression {"," expression}] ")"
	if (token_->getType() == TokenType::lparen)
	{
		token_ = scanner_->nextToken();
		expression();
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			if (token_->getType() == TokenType::comma)
			{
				token_ = scanner_->nextToken();
				expression();
			}
			else {
				shouldRepeat = false;
			}
		}
		if (token_->getType() == TokenType::rparen)
		{
			token_ = scanner_->nextToken();
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \")\" is missing");
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"(\" is missing");
	}
	return nullptr;
}

const Node* Parser::if_statement() {
	// "IF" expression "THEN" StatementSequence {"ELSIF" expression "THEN" StatementSequence} ["ELSE" StatementSequence] "END"
	token_ = scanner_->nextToken();
	expression();
	if (token_->getType() == TokenType::kw_then)
	{
		token_ = scanner_->nextToken();
		statement_sequence();
		if (token_->getType() == TokenType::kw_elsif)
		{
			bool shouldProceed = true;
			while (shouldProceed)
			{
				token_ = scanner_->nextToken();
				expression();
				if (token_->getType() == TokenType::kw_then) {
					token_ = scanner_->nextToken();
					statement_sequence();
					if (token_->getType() != TokenType::kw_elsif)
					{
						shouldProceed = false;
					}
				}
				else {
					logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"THEN\" keyword is missing.");
					shouldProceed = false;
					return nullptr;
				}
			}
		}
		if (token_->getType() == TokenType::kw_else)
		{
			token_ = scanner_->nextToken();
			statement_sequence();
		}
		if (token_->getType() == TokenType::kw_end)
		{
			token_ = scanner_->nextToken();
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"END\" keyword is missing.");
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"THEN\" keyword is missing.");
	}
	return nullptr;
}

const Node* Parser::while_statement() {
	// "WHILE" expression "DO" StatementSequence "END"
	token_ = scanner_->nextToken();
	expression();
	if (token_->getType() == TokenType::kw_do)
	{
		token_ = scanner_->nextToken();
		statement_sequence();
		if (token_->getType() == TokenType::kw_end)
		{
			token_ = scanner_->nextToken();
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"END\" is missing");
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"DO\" keyword is missing");
	}
	return nullptr;
}

const Node* Parser::selector() {
	// {"." ident | "[" expression "]"} -> repetition
	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		if (token_->getType() == TokenType::period)	// RECORD
		{
			token_ = scanner_->nextToken();
			std::string identifier = ident();
			if (!identifier.empty())
			{
				token_ = scanner_->nextToken();
				if (token_->getType() != TokenType::period || token_->getType() != TokenType::lbrack)
				{
					shouldRepeat = false;
				}
			}
			else {
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; Selector is not valid.");
				shouldRepeat = false;
			}
		}
		else if (token_->getType() == TokenType::lbrack) {	// ARRAY
			token_ = scanner_->nextToken();
			expression();
			if (token_->getType() == TokenType::rbrack)
			{
				token_ = scanner_->nextToken();
				if (token_->getType() != TokenType::period || token_->getType() != TokenType::lbrack)
				{
					shouldRepeat = false;
				}
			}
			else {
				shouldRepeat = false;
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"]\" is missing");
			}
		}
		else {
			shouldRepeat = false;
		}
	}
	return nullptr;
}