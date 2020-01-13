//
// Created by Michael Grossniklaus on 11/20/18.
//

#include <iostream>
#include "Parser.h"
#include <IdentToken.h>
#include "NumberToken.h"
#include "StringToken.h"
#include <ast\Expression\SimpleExpressionElement.h>
#include <ast\Expression\TermElement.h>
#include <ast\Expression\IdentSelectorFactor.h>
#include <ast\Expression\StringFactor.h>
#include <ast\Expression\NumberFactor.h>
#include <ast\Expression\ExpressionFactor.h>
#include <ast\Expression\NotFactor.h>
#include <ast\Type\IdentType.h>
#include <ast\Statement\ElseIf.h>
#include <ast\Statement\Else.h>
#include <ast\Selector\IdentSelector.h>
#include <ast\Selector\IndexSelector.h>

Parser::Parser(Scanner* scanner, Logger* logger) :
	scanner_(scanner), logger_(logger) {
}

Parser::~Parser() = default;

const std::unique_ptr<const Node> Parser::parse() {
	return std::unique_ptr<const Node>(module());
}

const std::string Parser::ident() {
	std::string result;
	token_->print(std::cout);
	if (token_->getType() == TokenType::const_ident) {
		result = static_cast<const IdentToken*>(token_)->getValue();
	}
	return result;
}

const Module* Parser::module()
{
	// "MODULE" ident ";" declarations ["BEGIN" StatementSequence] "END" ident "."
	token_ = scanner_->nextToken().get();
	if (token_->getType() == TokenType::kw_module)
	{
		token_ = scanner_->nextToken().get();
		std::string name = ident();
		if (!name.empty())
		{
			token_ = scanner_->nextToken().get();
			if (token_->getType() == TokenType::semicolon)
			{
				token_ = scanner_->nextToken().get();
				Module moduleNode(name, token_->getPosition());

				//
				// Control the declarations name whether there is a duplicated name
				//

				for (const Declaration* declarationNode : declarations()) {
					if (moduleNode.addDeclaration(declarationNode)) {
						logger_->error(token_->getPosition(), "- SEMANTIC ERROR; " + declarationNode->identifier + " is duplicated.");
					}
				}
				if (token_->getType() == TokenType::kw_begin)
				{
					token_ = scanner_->nextToken().get();

					//
					// Adding statements to the module
					//

					for (auto& statementNode : statement_sequence()) {
						moduleNode.addStatement(statementNode);
					}
				}
				if (token_->getType() == TokenType::kw_end)
				{
					token_ = scanner_->nextToken().get();
					std::string name = ident();
					if (!name.empty())
					{
						//
						// Control the module name that is identified after END keyword.
						// The name must be same as the main name
						//

						if (moduleNode.checkName(name))
						{
							token_ = scanner_->nextToken().get();
							if (token_->getType() == TokenType::period)
							{
								token_ = scanner_->nextToken().get();
								//
								// Every control is alright then return the module
								return &moduleNode;
							}
							else
							{
								logger_->error(token_->getPosition(), "- SYNTAX ERROR; \".\" is missing.");
							}
						}
						else {
							logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Module name is not correct.");
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

const std::vector<const Declaration*> Parser::declarations() {
	std::vector<const Declaration*> declarationsNode;

	// ["CONST" {ident "=" expression ";"}]
	// ["TYPE" {ident "=" type ";"}]
	// ["VAR" {IdentList ":" type ";"}]
	// {ProcedureDeclaration ";"}.

	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		if (token_->getType() == TokenType::kw_const || token_->getType() == TokenType::kw_type || token_->getType() == TokenType::kw_var || token_->getType() == TokenType::kw_procedure)
		{
			if (token_->getType() == TokenType::kw_const)
			{
				for (const ConstDeclaration* constDeclarations : const_declarations()) {
					declarationsNode.push_back(constDeclarations);
				}
			}

			if (token_->getType() == TokenType::kw_type)
			{
				for (const TypeDeclaration* typeDeclarations : type_declarations()) {
					declarationsNode.push_back(typeDeclarations);
				}
			}

			if (token_->getType() == TokenType::kw_var) {
				for (const VarDeclaration* varDeclarations : var_declarations()) {
					declarationsNode.push_back(varDeclarations);
				}
			}

			if (token_->getType() == TokenType::kw_procedure)
			{
				const ProcedureDeclaration* pN = procedure_declaration();
				if (pN != nullptr)
				{
					declarationsNode.push_back(pN);
				}
				else {
					shouldRepeat = false;
				}
			}
		}
		else {
			shouldRepeat = false;
		}
	}
	return declarationsNode;
}

const std::vector<const ConstDeclaration*> Parser::const_declarations() {
	// "CONST" {ident "=" expression ";"} -> repetation
	std::vector<const ConstDeclaration*> constDeclarations;
	token_ = scanner_->nextToken().get();
	std::string name = ident();
	if (!name.empty())
	{
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			token_ = scanner_->nextToken().get();
			if (token_->getType() == TokenType::op_eq)
			{
				token_ = scanner_->nextToken().get();
				const Expression* expressionNode = expression();
				if (expressionNode != nullptr)
				{
					//
					// Expression must be a constant.
					//
					if (token_->getType() == TokenType::semicolon)
					{
						logger_->info("CONST Declaration", "Name: " + name);
						ConstDeclaration constNode(name);
						constDeclarations.push_back(&constNode);
						token_ = scanner_->nextToken().get();
						name = ident();
						if (name.empty())
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
	return constDeclarations;
}

const std::vector<const TypeDeclaration*> Parser::type_declarations() {
	// "TYPE" {ident "=" type ";"} -> repetition
	std::vector<const TypeDeclaration*> typeDeclarations;
	bool shouldProceed = true;
	token_ = scanner_->nextToken().get();
	std::string name = ident();
	if (!name.empty())
	{
		while (shouldProceed)
		{
			token_ = scanner_->nextToken().get();
			if (token_->getType() == TokenType::op_eq)
			{
				token_ = scanner_->nextToken().get();
				const Type* typeN = type();
				if (typeN != nullptr)
				{
					if (token_->getType() == TokenType::semicolon)
					{
						logger_->info("TYPE Declaration", "Name: " + name);
						TypeDeclaration typeNode(name);
						//typeNode.type = typeN;
						typeDeclarations.push_back(&typeNode);
						token_ = scanner_->nextToken().get();
						name = ident();
						if (name.empty())
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

const std::vector<const VarDeclaration*> Parser::var_declarations() {
	// "VAR" {IdentList ":" type ";"} -> repetation
	token_ = scanner_->nextToken().get();
	std::vector<const VarDeclaration*> varDeclarations;
	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		std::vector<const std::string> identList = ident_list();
		if (identList.size() > 0)
		{
			if (token_->getType() == TokenType::colon)
			{
				token_ = scanner_->nextToken().get();
				const Type* typeNode = type();
				if (token_->getType() == TokenType::semicolon)
				{
					for (const std::string& ident : identList) {
						VarDeclaration varNode(ident);
						//varNode.type = typeNode;
						varDeclarations.push_back(&varNode);
					}
					token_ = scanner_->nextToken().get();
				}
				else {
					shouldRepeat = false;
					logger_->error(token_->getPosition(), "- SYNTAX ERROR; \";\ is missing");
				}
			}
			else {
				shouldRepeat = false;
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; \":\" is missing");
			}
		}
	}
	return varDeclarations;
}

const ProcedureDeclaration* Parser::procedure_declaration() {
	// ProcedureHeading ";" ProcedureBody
	const ProcedureHeading* heading = procedure_heading();
	if (heading != nullptr)
	{
		if (token_->getType() == TokenType::semicolon)
		{
			token_ = scanner_->nextToken().get();
			const ProcedureBody* body = procedure_body();
			if (body != nullptr)
			{
				ProcedureDeclaration pdN(heading, body);
				return &pdN;
			}
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \";\" is missing");
		}
	}
	return nullptr;
}

const Expression* Parser::expression() {
	// SimpleExpression [("=" | "#" | "<" | "<=" | ">" | ">=") SimpleExpression] -> Optional	
	const SimpleExpression* lhs = simple_expression();
	if (lhs != nullptr)
	{
		Expression eN(lhs);
		if (token_->getType() == TokenType::op_eq || token_->getType() == TokenType::op_neq || token_->getType() == TokenType::op_lt || token_->getType() == TokenType::op_leq || token_->getType() == TokenType::op_gt || token_->getType() == TokenType::op_geq)
		{
			TokenType sign = token_->getType();
			token_ = scanner_->nextToken().get();
			const SimpleExpression* rhs = simple_expression();
			if (rhs != nullptr)
			{
				if (lhs->type == rhs->type)
				{
					eN = Expression(lhs, sign, rhs);
				}
				else {
					logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Simple Expression types are not same.");
					return nullptr;
				}
			}
			else {
				return nullptr;
			}
		}
		eN.type = lhs->type;
		return &eN;
	}
	return nullptr;
}

const SimpleExpression* Parser::simple_expression() {
	// ["+" | "-"] term {("+" | "-" | "OR") term}
	TokenType sign = TokenType::null;
	if (token_->getType() == TokenType::op_plus || token_->getType() == TokenType::op_minus) {
		sign = token_->getType();
		token_ = scanner_->nextToken().get();
	}
	const Term* termNode = term();
	if (termNode != nullptr)
	{
		if (sign != TokenType::null && termNode->type != ExpressionType::number)
		{
			// ??
			logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Term type is not number.");
			return nullptr;
		}
		SimpleExpression simpleExpressionN;
		simpleExpressionN.type = ExpressionType::string;
		const SimpleExpressionElement simpleExpressionElement1(sign, termNode);
		simpleExpressionN.elements.push_back(&simpleExpressionElement1);
		//
		// If the term is string, after that +, -, OR signs are not valid.
		//
		bool shouldRepeat = termNode->type != ExpressionType::string;
		while (shouldRepeat)
		{
			if (token_->getType() == TokenType::op_plus || token_->getType() == TokenType::op_minus)
			{
				if (termNode->type == ExpressionType::number)
				{
					sign = token_->getType();
					token_ = scanner_->nextToken().get();
					termNode = term();
					if (termNode != nullptr)
					{
						if (termNode->type == ExpressionType::number)
						{
							const SimpleExpressionElement simpleExpressionElement2(sign, termNode);
							simpleExpressionN.elements.push_back(&simpleExpressionElement2);
							simpleExpressionN.type = ExpressionType::number;
						}
						else {
							// ??
							logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Term type is not number.");
							shouldRepeat = false;
							return nullptr;
						}
					}
					else {
						shouldRepeat = false;
						return nullptr;
					}
				}
				else {
					// ??
					logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Term type is not number.");
					shouldRepeat = false;
					return nullptr;
				}
			}
			else if (token_->getType() == TokenType::op_or) {
				if (termNode->type == ExpressionType::boolean)
				{
					sign = token_->getType();
					token_ = scanner_->nextToken().get();
					termNode = term();
					if (termNode != nullptr)
					{
						if (termNode->type == ExpressionType::boolean)
						{
							const SimpleExpressionElement simpleExpressionElement2(sign, termNode);
							simpleExpressionN.elements.push_back(&simpleExpressionElement2);
							simpleExpressionN.type = ExpressionType::boolean;
						}
						else {
							// ??
							logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Term type is not boolean.");
							shouldRepeat = false;
							return nullptr;
						}
					}
					else {
						shouldRepeat = false;
						return nullptr;
					}
				}
				else {
					// ??
					logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Term type is not boolean.");
					shouldRepeat = false;
					return nullptr;
				}
			}
			else {
				shouldRepeat = false;
			}
		}
		return &simpleExpressionN;
	}
	return nullptr;
}

const Term* Parser::term() {
	// factor {("*" | "DIV" | "MOD" | "&") factor} -> repetition
	const Factor* factorNode = factor();
	if (factorNode != nullptr)
	{
		Term termNode;
		const TermElement termNodeElement1(TokenType::null, factorNode);
		termNode.elements.push_back(&termNodeElement1);
		termNode.type = ExpressionType::string;
		//
		// If the factor is string, after that *, DIV, MOD, & signs are not valid.
		//
		bool shouldRepeat = factorNode->type != ExpressionType::string;
		while (shouldRepeat)
		{
			TokenType sign;
			if (token_->getType() == TokenType::op_times || token_->getType() == TokenType::op_div || token_->getType() == TokenType::op_mod)
			{
				if (factorNode->type == ExpressionType::number)
				{
					sign = token_->getType();
					token_ = scanner_->nextToken().get();
					factorNode = factor();
					if (factorNode != nullptr)
					{
						if (factorNode->type != ExpressionType::number)
						{
							const TermElement termNodeElement2(sign, factorNode);
							termNode.elements.push_back(&termNodeElement2);
							termNode.type = ExpressionType::number;
						}
						else {
							// ??
							logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Factor type is not number.");
							shouldRepeat = false;
							return nullptr;
						}
					}
					else {
						shouldRepeat = false;
						return nullptr;
					}
				}
				else {
					// ??
					logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Factor type is not number.");
					shouldRepeat = false;
					return nullptr;
				}
			}
			else if (token_->getType() == TokenType::op_and) {
				if (factorNode->type == ExpressionType::boolean)
				{
					sign = token_->getType();
					token_ = scanner_->nextToken().get();
					factorNode = factor();
					if (factorNode != nullptr)
					{
						if (factorNode->type != ExpressionType::boolean)
						{
							const TermElement termNodeElement2(sign, factorNode);
							termNode.elements.push_back(&termNodeElement2);
							termNode.type = ExpressionType::boolean;
						}
						else {
							// ??
							logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Factor type is not boolean.");
							shouldRepeat = false;
							return nullptr;
						}
					}
					else {
						shouldRepeat = false;
						return nullptr;
					}
				}
				else {
					// ??
					logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Factor type is not boolean.");
					shouldRepeat = false;
					return nullptr;
				}
			}
			else {
				shouldRepeat = false;
			}
		}
		return &termNode;
	}
	return nullptr;
}

const Factor* Parser::factor() {
	// ident selector | integer | "(" expression ")" | "~" factor	
	std::string identifier = ident();
	if (!identifier.empty())
	{
		token_ = scanner_->nextToken().get();
		IdentSelectorFactor isfn(identifier);
		std::vector<const Selector*> selectorNodes = selector();
		if (selectorNodes.size() > 0)
		{
			for (const Selector* node : selectorNodes) {
				isfn.selectors.push_back(node);
			}
		}
		return &isfn;
	}
	else {
		if (token_->getType() == TokenType::const_string)
		{
			StringFactor sfn(static_cast<const StringToken*>(token_)->getValue());
			token_ = scanner_->nextToken().get();
			return &sfn;
		}
		else if (token_->getType() == TokenType::const_number) {
			NumberFactor ifn(static_cast<const NumberToken*>(token_)->getValue());
			token_ = scanner_->nextToken().get();
			return &ifn;
		}
		else if (token_->getType() == TokenType::lparen) {
			token_ = scanner_->nextToken().get();
			const Expression* en = expression();
			if (en != nullptr)
			{
				if (token_->getType() == TokenType::rparen)
				{
					token_ = scanner_->nextToken().get();
					ExpressionFactor efn(en);
					return &efn;
				}
				else {
					logger_->error(token_->getPosition(), "- SYNTAX ERROR; \")\" is missing.");
				}
			}
		}
		else if (token_->getType() == TokenType::op_not) {
			token_ = scanner_->nextToken().get();
			const Factor* fn = factor();
			if (fn != nullptr)
			{
				if (fn->type == ExpressionType::boolean)
				{
					NotFactor nfn(fn);
					return &nfn;
				}
				else {
					logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Factor is not boolean.");
				}
			}
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; Factor is not valid.");
		}
	}
	return nullptr;
}

const Type* Parser::type() {
	// ident | ArrayType | RecordType
	std::string name = ident();
	if (!name.empty())
	{
		IdentType itn(name);
		token_ = scanner_->nextToken().get();
		return &itn;
	}
	else if (token_->getType() == TokenType::kw_array) {
		const ArrayType* aN = array_type();
		return array_type();
	}
	else if (token_->getType() == TokenType::kw_record) {
		const RecordType* rN = record_type();
		return rN;
	}
	logger_->error(token_->getPosition(), "- SYNTAX ERROR; type is not valid.");
	return nullptr;
}

const ArrayType* Parser::array_type() {
	// "ARRAY" expression "OF" type.
	token_ = scanner_->nextToken().get();
	const Expression* eN = expression();
	if (eN != nullptr)
	{
		if (token_->getType() == TokenType::kw_of)
		{
			token_ = scanner_->nextToken().get();
			const Type* tN = type();
			if (tN != nullptr)
			{
				return &ArrayType(eN, tN);
			}
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"OF\" keyword is missing.");
		}
	}
	return nullptr;
}

const RecordType* Parser::record_type() {
	// "RECORD" FieldList {";" FieldList} "END"
	token_ = scanner_->nextToken().get();
	RecordType rtN;
	const FieldList* flN = field_list();
	if (flN != nullptr)
	{
		rtN.fieldListNodes.push_back(flN);
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			if (token_->getType() != TokenType::semicolon)
			{
				token_ = scanner_->nextToken().get();
				flN = field_list();
				if (flN != nullptr)
				{
					rtN.fieldListNodes.push_back(flN);
				}
				else {
					shouldRepeat = false;
					return nullptr;
				}
			}
			else {
				shouldRepeat = false;
			}
		}
		if (token_->getType() == TokenType::kw_end)
		{
			token_ = scanner_->nextToken().get();
			return &rtN;
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"END\" keyword is missing.");
		}
	}
	return nullptr;
}

const FieldList* Parser::field_list() {
	// [IdentList ":" type] -> optional
	std::vector<const std::string> identList = ident_list();
	if (identList.size() > 0)
	{
		FieldList fln;
		for (std::string ident : identList) {
			fln.identList.push_back(ident);
		}
		if (token_->getType() == TokenType::colon)
		{
			token_ = scanner_->nextToken().get();
			const Type* tN = type();
			if (tN != nullptr)
			{
				fln.typeNode = tN;
				return &fln;
			}
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \":\" is missing");
		}
	}
	return nullptr;
}

const std::vector<const std::string> Parser::ident_list() {
	// ident {"," ident} -> repetition
	std::vector<const std::string> identList;
	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		std::string name = ident();
		if (!name.empty())
		{
			identList.push_back(name);
			token_ = scanner_->nextToken().get();
			if (token_->getType() != TokenType::comma)
			{
				shouldRepeat = false;
			}
			else {
				token_ = scanner_->nextToken().get();
			}
		}
		else {
			shouldRepeat = false;
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; Identifier is not valid.");
		}
	}
	if (identList.size() <= 0)
	{
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; Identifier list is null.");
	}
	return identList;
}

const ProcedureHeading* Parser::procedure_heading() {
	// "PROCEDURE" ident [FormalParameters]
	token_ = scanner_->nextToken().get();
	std::string name = ident();
	if (!name.empty())
	{
		ProcedureHeading heading(name);
		token_ = scanner_->nextToken().get();
		for (const FormalParameter* fpN : formal_parameters()) {
			heading.formalParameters.push_back(fpN);
		}
		return &heading;
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; Procedure name is not valid.");
	}
	return nullptr;
}

const ProcedureBody* Parser::procedure_body() {
	// declarations ["BEGIN" StatementSequence] "END" ident
	std::vector<const Declaration*> declarationList = declarations();
	ProcedureBody pbN;
	pbN.declarations = declarationList;
	if (token_->getType() == TokenType::kw_begin)
	{
		token_ = scanner_->nextToken().get();
		for (const Statement* statement : statement_sequence()) {
			pbN.statements.push_back(statement);
		}
	}
	if (token_->getType() == TokenType::kw_end)
	{
		token_ = scanner_->nextToken().get();
		std::string name = ident();
		if (!name.empty())
		{
			pbN.ident = name;
			token_ = scanner_->nextToken().get();
			return &pbN;
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

const std::vector<const FormalParameter*> Parser::formal_parameters() {
	// "(" [FPSection {";" FPSection} ] ")"
	std::vector<const FormalParameter*> formalParameters;
	if (token_->getType() == TokenType::lparen)
	{
		token_ = scanner_->nextToken().get();
		const FormalParameter* fpn = fp_section();
		if (fpn != nullptr)
		{
			formalParameters.push_back(fpn);
			fpn = nullptr;
			bool shouldRepeat = true;
			while (shouldRepeat)
			{
				if (token_->getType() == TokenType::semicolon)
				{
					token_ = scanner_->nextToken().get();
					fpn = fp_section();
					if (fpn != nullptr)
					{
						formalParameters.push_back(fpn);
					}
					else {
						shouldRepeat = false;
					}
				}
				else {
					shouldRepeat = false;
				}
			}
		}
		if (token_->getType() == TokenType::rparen)
		{
			token_ = scanner_->nextToken().get();
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \")\" is missing.");
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"(\" is missing.");
	}
	return formalParameters;
}

const FormalParameter* Parser::fp_section() {
	// ["VAR"]	IdentList ":" type
	if (token_->getType() == TokenType::kw_var)
	{
		token_ = scanner_->nextToken().get();
	}
	std::vector<const std::string> idents = ident_list();
	if (idents.size() > 0)
	{
		FormalParameter fpN;
		for (std::string ident : ident_list()) {
			fpN.identList.push_back(ident);
		}
		if (token_->getType() == TokenType::colon)
		{
			token_ = scanner_->nextToken().get();
			const Type* tN = type();
			if (tN != nullptr)
			{
				//fpN.type = tN;
				return &fpN;
			}
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \":\" is missing.");
		}
	}
	return nullptr;
}

const std::vector<const Statement*> Parser::statement_sequence() {
	// statement {";" statement}
	std::vector<const Statement*> statementList;
	bool shouldProceed = true;
	while (shouldProceed)
	{
		const Statement* statementNode = statement();
		if (statementNode != nullptr)
		{
			statementList.push_back(statementNode);
			if (token_->getType() == TokenType::semicolon)
			{
				token_ = scanner_->nextToken().get();
			}
			else {
				shouldProceed = false;
			}
		}
		else {
			shouldProceed = false;
		}
	}
	return statementList;
}

const Statement* Parser::statement() {
	// [assignment | ProcedureCall | IfStatement | WhileStatement]
	if (token_->getType() == TokenType::kw_if)
	{
		return if_statement();
	}
	else if (token_->getType() == TokenType::kw_while) {
		return while_statement();
	}
	else {
		std::string name = ident();
		if (!name.empty())
		{
			token_ = scanner_->nextToken().get();
			std::vector<const Selector*> selectorNodes = selector();
			const AssignmentStatement* _asN = assignment();
			if (_asN != nullptr)
			{
				AssignmentStatement asN;
				asN.ident = name;
				asN.selectors = selectorNodes;
				asN.expression = _asN->expression;
				return &asN;
			}
			const ProcedureCallStatement* _pcsN = procedure_call();
			if (_pcsN != nullptr)
			{
				ProcedureCallStatement pcsN(name);
				pcsN.actualParameters = _pcsN->actualParameters;
				return &pcsN;
			}
		}
	}
	return nullptr;
}

const AssignmentStatement* Parser::assignment() {
	// ident selector ":=" expression
	if (token_->getType() != TokenType::op_becomes)
	{
		token_ = scanner_->nextToken().get();
		const Expression* eN = expression();
		if (eN != nullptr)
		{
			AssignmentStatement asN;
			asN.expression = eN;
			return &asN;
		}
	}
	/*else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \":=\" is missing.");
	}*/
	return nullptr;
}

const ProcedureCallStatement* Parser::procedure_call() {
	// ident selector [ActualParameters]
	if (token_->getType() == TokenType::lparen)
	{
		token_ = scanner_->nextToken().get();
		const std::vector<const Expression*> actualParameters = actual_parameters();
		if (token_->getType() == TokenType::rparen)
		{
			token_ = scanner_->nextToken().get();
			ProcedureCallStatement pcsn("");
			pcsn.actualParameters = actualParameters;
			return &pcsn;
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \")\" is missing");
		}
	}/*
	else {
		logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"(\" is missing");
	}*/
	return nullptr;
}

const std::vector<const Expression*> Parser::actual_parameters() {
	// "(" [expression {"," expression}] ")"
	std::vector<const Expression*> actualParameters;
	const Expression* eN = expression();
	if (eN != nullptr)
	{
		actualParameters.push_back(eN);
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			if (token_->getType() == TokenType::comma)
			{
				token_ = scanner_->nextToken().get();
				const Expression* exN = expression();
				if (exN != nullptr)
				{
					actualParameters.push_back(exN);
				}
				else {
					shouldRepeat = false;
					// No valid expression after comma 
					actualParameters.clear();
				}
			}
			else {
				shouldRepeat = false;
			}
		}
	}
	return actualParameters;
}

const IfStatement* Parser::if_statement() {
	// "IF" expression "THEN" StatementSequence {"ELSIF" expression "THEN" StatementSequence} ["ELSE" StatementSequence] "END"
	token_ = scanner_->nextToken().get();
	IfStatement isN;
	const Expression* eN = expression();
	if (eN != nullptr && eN->type == ExpressionType::boolean)
	{
		isN.expression = eN;
		if (token_->getType() == TokenType::kw_then)
		{
			token_ = scanner_->nextToken().get();
			std::vector<const Statement*> statements = statement_sequence();
			isN.statements = statements;
			if (token_->getType() == TokenType::kw_elsif)
			{
				bool shouldProceed = true;
				while (shouldProceed)
				{
					token_ = scanner_->nextToken().get();
					ElseIf eiN;
					const Expression* eN = expression();
					if (eN != nullptr && eN->type == ExpressionType::boolean)
					{
						eiN.expression = eN;
						if (token_->getType() == TokenType::kw_then) {
							token_ = scanner_->nextToken().get();
							std::vector<const Statement*> statementsEi = statement_sequence();
							eiN.statements = statementsEi;
							isN.elseIfNodes.push_back(&eiN);
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
					else {
						logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Expression type must be boolean.");
						shouldProceed = false;
						return nullptr;
					}
				}
			}
			if (token_->getType() == TokenType::kw_else)
			{
				token_ = scanner_->nextToken().get();
				Else elseN;
				std::vector<const Statement*> statementsElseN = statement_sequence();
				elseN.statements = statementsElseN;
				isN.elseNode = &elseN;
			}
			if (token_->getType() == TokenType::kw_end)
			{
				token_ = scanner_->nextToken().get();
				return &isN;
			}
			else {
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"END\" keyword is missing.");
			}
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"THEN\" keyword is missing.");
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Expression type must be boolean.");
	}
	return nullptr;
}

const WhileStatement* Parser::while_statement() {
	// "WHILE" expression "DO" StatementSequence "END"
	token_ = scanner_->nextToken().get();
	const Expression* eN = expression();
	if (eN != nullptr && eN->type == ExpressionType::boolean)
	{
		if (token_->getType() == TokenType::kw_do)
		{
			token_ = scanner_->nextToken().get();
			std::vector<const Statement*> statements = statement_sequence();
			if (token_->getType() == TokenType::kw_end)
			{
				token_ = scanner_->nextToken().get();
				WhileStatement wsN;
				wsN.expression = eN;
				wsN.statements = statements;
				return &wsN;
			}
			else {
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"END\" is missing");
			}
		}
		else {
			logger_->error(token_->getPosition(), "- SYNTAX ERROR; \"DO\" keyword is missing");
		}
	}
	else {
		logger_->error(token_->getPosition(), "- SEMANTIC ERROR; Expression type must be boolean.");
	}
	return nullptr;
}

const std::vector<const Selector*> Parser::selector() {
	// {"." ident | "[" expression "]"} -> repetition
	std::vector<const Selector*> selectors;
	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		if (token_->getType() == TokenType::period)
		{
			token_ = scanner_->nextToken().get();
			std::string identifier = ident();
			if (!identifier.empty())
			{
				const IdentSelector identSelector(identifier);
				selectors.push_back(&identSelector);
				token_ = scanner_->nextToken().get();
				if (token_->getType() != TokenType::comma || token_->getType() != TokenType::lbrack)
				{
					shouldRepeat = false;
				}
			}
			else {
				logger_->error(token_->getPosition(), "- SYNTAX ERROR; Selector is not valid.");
				shouldRepeat = false;
			}
		}
		else if (token_->getType() == TokenType::lbrack) {
			token_ = scanner_->nextToken().get();
			const Expression* expressionNode = expression();
			if (expressionNode != nullptr)
			{
				const IndexSelector indexSelector(expressionNode);
				selectors.push_back(&indexSelector);
				if (token_->getType() == TokenType::rbrack)
				{
					token_ = scanner_->nextToken().get();
					if (token_->getType() != TokenType::comma || token_->getType() != TokenType::lbrack)
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
		else {
			shouldRepeat = false;
		}
	}
	return selectors;
}