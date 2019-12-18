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
		for (auto& typeNode : type_declarations()) {
			declarations.push_back(typeNode);
		}
	case TokenType::kw_var:
		for (auto& varNode : var_declarations()) {
			declarations.push_back(varNode);
		}
	case TokenType::kw_procedure:
		const ProcedureDeclarationNode* pdn = procedure_declaration();
		if (pdn != nullptr)
		{
			declarations.push_back(pdn);
		}
		break;
	default:
		return declarations;
	}
}

const std::vector<const ConstDeclarationNode*> Parser::const_declarations() {
	/*
		"CONST"
		{ident "=" expression ";"} -> repetation
	*/
	std::vector<const ConstDeclarationNode*> constDeclarations;
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_const)
	{
		bool shouldProceed = true;
		token = *scanner_->nextToken();
		while (shouldProceed)
		{
			if (token.getType() == TokenType::const_ident)
			{
				ConstDeclarationNode constNode(ident());
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::op_eq)
				{
					token = *scanner_->nextToken();
					const ExpressionNode* expressionNode = expression(); // MOVES NEXT TOKEN
					if (expressionNode != nullptr)
					{
						constNode.expression = expressionNode;
						token = *scanner_->peekToken();
						if (token.getType() == TokenType::semicolon)
						{
							constDeclarations.push_back(&constNode);
							token = *scanner_->nextToken();
							if (token.getType() != TokenType::const_ident)
							{
								shouldProceed = false;
							}
						}
						else {
							shouldProceed = false;
							logger_->error(token.getPosition(), "- SYNTAX ERROR; \";\ is missing");
						}
					}
					else {
						shouldProceed = false;
						logger_->error(token.getPosition(), "- SYNTAX ERROR; No expression");
					}
				}
				else {
					shouldProceed = false;
					logger_->error(token.getPosition(), "- SYNTAX ERROR; \"=\" is missing");
				}
			}
			else {
				shouldProceed = false;
				logger_->error(token.getPosition(), "- SYNTAX ERROR; No valid identatiton");
			}
		}
	}
	return constDeclarations;
}

const std::vector<const TypeDeclarationNode*> Parser::type_declarations() {
	/*
		"TYPE"
		{ident "=" type ";"} -> repetition
	*/
	std::vector<const TypeDeclarationNode*> typeDeclarations;
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_type)
	{
		bool shouldProceed = true;
		token = *scanner_->nextToken();
		while (shouldProceed)
		{
			if (token.getType() == TokenType::const_ident)
			{
				TypeDeclarationNode typeNode(ident());
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::op_eq)
				{
					token = *scanner_->nextToken();
					const TypeNode* typeN = type(); // MOVES NEXT TOKEN
					if (typeN != nullptr)
					{
						typeNode.type = typeN;
						token = *scanner_->peekToken();
						if (token.getType() == TokenType::semicolon)
						{
							typeDeclarations.push_back(&typeNode);
							token = *scanner_->nextToken();
							if (token.getType() != TokenType::const_ident)
							{
								shouldProceed = false;
							}
						}
						else {
							shouldProceed = false;
							logger_->error(token.getPosition(), "- SYNTAX ERROR; \";\ is missing");
						}
					}
					else {
						shouldProceed = false;
						logger_->error(token.getPosition(), "- SYNTAX ERROR; No type");
					}
				}
				else {
					shouldProceed = false;
					logger_->error(token.getPosition(), "- SYNTAX ERROR; \"=\" is missing");
				}
			}
			else {
				shouldProceed = false;
				logger_->error(token.getPosition(), "- SYNTAX ERROR; No valid identatiton");
			}
		}
	}
	return typeDeclarations;
}

const std::vector<const VarDeclarationNode*> Parser::var_declarations() {
	/*
		"VAR"
		{IdentList ":" type ";"} -> repetation
	*/
	std::vector<const VarDeclarationNode*> varDeclarations;
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_var)
	{
		bool shouldProceed = true;
		token = *scanner_->nextToken();
		while (shouldProceed)
		{
			std::vector<const std::string> identList = ident_list();	// MOVES NEXT TOKEN
			if (identList.size() > 0)
			{
				token = *scanner_->peekToken();
				if (token.getType() == TokenType::colon)
				{
					token = *scanner_->nextToken();
					const TypeNode* typeNode = type(); // MOVES NEXT TOKEN
					if (typeNode != nullptr)
					{
						token = *scanner_->peekToken();
						if (token.getType() == TokenType::semicolon)
						{
							for (const std::string& ident : identList) {
								VarDeclarationNode varNode(ident);
								varNode.type = typeNode;
								varDeclarations.push_back(&varNode);
							}
							token = *scanner_->nextToken();
							if (token.getType() != TokenType::const_ident)
							{
								shouldProceed = false;
							}
						}
						else {
							shouldProceed = false;
							logger_->error(token.getPosition(), "- SYNTAX ERROR; \";\ is missing");
						}
					}
					else {
						shouldProceed = false;
						logger_->error(token.getPosition(), "- SYNTAX ERROR; No type");
					}
				}
				else {
					logger_->error(token.getPosition(), "- SYNTAX ERROR; \":\" is missing");
				}
			}
			else {
				shouldProceed = false;
				logger_->error(token.getPosition(), "- SYNTAX ERROR; variable identation is not correct");
			}
		}
	}
	return varDeclarations;
}

const ProcedureDeclarationNode* Parser::procedure_declaration() {
	/*
	ProcedureHeading ";" ProcedureBody
	*/
	const ProcedureHeadingNode* heading = procedure_heading(); // MOVES NEXT TOKEN ?
	if (heading != nullptr)
	{
		Token token = *scanner_->peekToken();
		if (token.getType() == TokenType::semicolon)
		{
			token = *scanner_->nextToken();
			const ProcedureBodyNode* body = procedure_body();
			if (body != nullptr)
			{
				ProcedureDeclarationNode pdN(heading, body);
				return &pdN;
			}
			else {
				// SYNTAX ERROR: procedure body is not valid
			}
		}
		else {
			// SYNTAX ERROR: ";" is missing
		}
	}
	else {
		// SYNTAX ERROR: procedure heading is not valid
		return nullptr;
	}
}

const ExpressionNode* Parser::expression() {
	/*
	SimpleExpression
	[("=" | "#" | "<" | "<=" | ">" | ">=") SimpleExpression] -> Optional
	*/
	Token token = *scanner_->peekToken();
	const SimpleExpressionNode* lhs = simple_expression(); // MOVES NEXT TOKEN
	if (lhs != nullptr)
	{
		token = *scanner_->peekToken();
		if (token.getType() == TokenType::op_eq || token.getType() == TokenType::op_neq || token.getType() == TokenType::op_lt || token.getType() == TokenType::op_leq || token.getType() == TokenType::op_gt || token.getType() == TokenType::op_geq)
		{
			TokenType sign = token.getType();
			token = *scanner_->nextToken();
			const SimpleExpressionNode* rhs = simple_expression();
			if (rhs != nullptr)
			{
				return &ExpressionNode(lhs, sign, rhs);
			}
			else {
				logger_->error(token.getPosition(), "- SYNTAX ERROR; No expression");
			}
		}
		else {
			return &ExpressionNode(lhs);
		}
	}
	else {
		logger_->error(token.getPosition(), "- SYNTAX ERROR; No expression");
	}
}

const SimpleExpressionNode* Parser::simple_expression() {
	/*
	 ["+" | "-"] -> Optional
	 term
	 {("+" | "-" | "OR") term} -> repetition
	*/
	Token token = *scanner_->peekToken();
	SimpleExpressionNode* simpleExpressionN = nullptr;
	TokenType sign = TokenType::op_plus;
	if (token.getType() == TokenType::op_plus || token.getType() == TokenType::op_minus) {
		sign = token.getType();
		token = *scanner_->nextToken();
	}
	const TermNode* termN1 = term();	// MOVES NEXT TOKEN
	if (termN1 != nullptr)
	{
		simpleExpressionN = &SimpleExpressionNode();
		const SimpleExpressionElement simpleExpressionElement1(sign, termN1);
		simpleExpressionN->elements.push_back(&simpleExpressionElement1);
		token = *scanner_->peekToken();
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			if (token.getType() == TokenType::op_plus || token.getType() == TokenType::op_minus || token.getType() == TokenType::op_or)
			{
				sign = token.getType();
				const TermNode* termN2 = term(); // MOVES NEXT TOKEN
				if (termN2 != nullptr)
				{
					const SimpleExpressionElement simpleExpressionElement2(sign, termN2);
					simpleExpressionN->elements.push_back(&simpleExpressionElement2);
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
	}
	else {
		// SYNTAX ERROR ?
	}
	return simpleExpressionN;
}

const TermNode* Parser::term() {
	/*
	factor
	{("*" | "DIV" | "MOD" | "&") factor} -> repetition
	*/
	Token token = *scanner_->peekToken();
	TermNode* termN = nullptr;
	const FactorNode* factorN1 = factor();	// MOVES NEXT TOKEN
	if (factorN1 != nullptr)
	{
		termN = &TermNode();
		const TermNodeElement termNodeElement1(TokenType::null, factorN1);
		termN->elements.push_back(&termNodeElement1);
		token = *scanner_->peekToken();
		bool shouldRepeat = true;
		while (shouldRepeat)
		{
			if (token.getType() == TokenType::op_times || token.getType() == TokenType::op_div || token.getType() == TokenType::op_mod || token.getType() == TokenType::op_and)
			{
				TokenType sign = token.getType();
				const FactorNode* factorN2 = factor();	// MOVES NEXT TOKEN
				if (factorN2 != nullptr)
				{
					const TermNodeElement termNodeElement2(sign, factorN2);
					termN->elements.push_back(&termNodeElement2);
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
	}
	else {
		// SYNTAX ERROR
	}
	return termN;
}

const FactorNode* Parser::factor() {
	/*
	ident selector | integer | "(" expression ")" | "~" factor
	*/
	Token token = *scanner_->peekToken();
	FactorNode* factorN = nullptr;
	if (token.getType() == TokenType::const_ident)
	{
		IdentSelectorFactorNode isfn(ident());
		token = *scanner_->nextToken();
		std::vector<const SelectorNode*> selectorNodes = selector();
		if (selectorNodes.size() > 0)
		{
			for (const SelectorNode* node : selectorNodes) {
				isfn.selectors.push_back(node);
			}
		}
		else {

		}
		factorN = &isfn;
	}
	else if (token.getType() == TokenType::const_number) {
		IntegerFactorNode ifn(ident());
		factorN = &ifn;
	}
	else if (token.getType() == TokenType::lparen) {
		token = *scanner_->nextToken();
		const ExpressionNode* en = expression();
		if (en != nullptr)
		{
			ExpressionFactorNode efn(en);
			factorN = &efn;
		}
		else {
			//SYNTAX ERROR
		}
	}
	else if (token.getType() == TokenType::op_not) {
		const FactorNode* fn = factor();
		if (fn != nullptr)
		{
			NotFactorNode nfn(fn);
			factorN = &nfn;
		}
		else {
			// SYNTAX ERROR
		}
	}
	else {
		// SYNTAX ERROR
	}
	return factorN;
}

const TypeNode* Parser::type() {
	/*
	ident | ArrayType | RecordType
	*/
	Token token = *scanner_->peekToken();
	switch (token.getType())
	{
	case TokenType::const_ident:
		return &IdentTypeNode(ident());
		break;
	case TokenType::kw_array:
		return array_type();
		break;
	case TokenType::kw_record:
		return record_type();
		break;
	default:
		return nullptr;
		break;
	}
}

const ArrayTypeNode* Parser::array_type() {
	/*
	"ARRAY" expression "OF" type.
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_array)
	{
		const ExpressionNode* eN = expression(); // MOVES NEXT TOKEN
		if (eN != nullptr)
		{
			token = *scanner_->peekToken();
			if (token.getType() == TokenType::kw_of)
			{
				token = *scanner_->nextToken();
				const TypeNode* tN = type();
				if (tN != nullptr)
				{
					return &ArrayTypeNode(eN, tN);
				}
			}
		}
	}
	// SYNTAX ERROR
	return nullptr;
}

const RecordTypeNode* Parser::record_type() {
	/*
	"RECORD" FieldList
	{";" FieldList} -> repetition
	"END"
	*/
	RecordTypeNode rtN;
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_record)
	{
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::const_ident)
		{
			bool shouldRepeat = true;
			while (shouldRepeat)
			{
				const FieldListNode* flN = field_list();
				if (flN->identList.size() > 0 || flN != nullptr)
				{
					rtN.fieldListNodes.push_back(flN);
					token = *scanner_->peekToken();
					if (token.getType() == TokenType::semicolon)
					{
						token = *scanner_->nextToken();
						if (token.getType() != TokenType::const_ident)
						{
							shouldRepeat = false;
						}
					}
					else {
						shouldRepeat = false;
					}
				}
				else {
					// SYNTAX ERROR
					// fieldList is null
				}
			}
		}
		else {
			// SYNTAX ERROR
			return nullptr;
		}

		if (token.getType() != TokenType::kw_end)
		{
			// SYNTAX ERROR
			// "END" is missing
		}
		else {
			*scanner_->nextToken();
			return &rtN;
		}
	}
	else {
		return nullptr;
	}
}

const FieldListNode* Parser::field_list() {
	/*
	[IdentList ":" type] -> optional
	*/
	FieldListNode fln;
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::const_ident)
	{
		std::vector<const std::string> identList = ident_list();	// MOVES NEXT TOKEN
		for (std::string ident : identList) {
			fln.identList.push_back(ident);
		}
		token = *scanner_->peekToken();
		if (token.getType() == TokenType::colon)
		{
			token = *scanner_->nextToken();
			const TypeNode* tN = type();
			if (tN != nullptr)
			{
				fln.typeNode = tN;
				return &fln;
			}
			else {
				// SYNTAX ERROR
				return nullptr;
			}
		}
		else {
			// SYNTAX ERROR
			return nullptr;
		}
	}
	else {
		// NO FIELD LIST
		return &fln;
	}
}

const std::vector<const std::string> Parser::ident_list() {
	/*
	ident
	{"," ident} -> repetition
	*/
	Token token = *scanner_->peekToken();
	std::vector<const std::string> identList;
	if (token.getType() == TokenType::const_ident)
	{
		bool shouldProceed = true;
		identList.push_back(ident());
		token = *scanner_->nextToken();
		while (shouldProceed)
		{
			if (token.getType() == TokenType::comma)
			{
				token = *scanner_->nextToken();
				if (token.getType() == TokenType::const_ident)
				{
					identList.push_back(ident());
					token = *scanner_->nextToken();
					if (token.getType() != TokenType::comma)
					{
						shouldProceed = false;
					}
				}
				else {
					shouldProceed = false;
					logger_->error(token.getPosition(), "- SYNTAX ERROR; Unvalid identifier");
				}
			}
			else {
				shouldProceed = false;
				logger_->error(token.getPosition(), "- SYNTAX ERROR; \",\" is missing");
			}
		}
	}
	else {
		logger_->error(token.getPosition(), "- SYNTAX ERROR; Unvalid identifier");
	}

	return identList;
}

const ProcedureHeadingNode* Parser::procedure_heading() {
	/*
	"PROCEDURE" ident
	[FormalParameters] -> Optional
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_procedure) {
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::const_ident)
		{
			ProcedureHeadingNode heading(ident());
			token = *scanner_->nextToken();
			for (const FormalParameterNode* fpN : formal_parameters()) {
				heading.formalParameters.push_back(fpN);
			}
			return &heading;
		}
		else {
			// SYNTAX ERROR: Invalid procedure name
		}
	}
	else {
		return nullptr;
	}


	bool shouldProceed = true;
	bool shouldMoveNext = false;
	TokenType nextToken = TokenType::kw_procedure;
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

const ProcedureBodyNode* Parser::procedure_body() {
	/*
	declarations
	["BEGIN" StatementSequence] -> optional
	"END" ident
	*/
	Token token = *scanner_->peekToken();
	std::vector<const DeclarationNode*> declarationList = declarations();
	if (declarationList.size() > 0)
	{
		ProcedureBodyNode pbN;
		pbN.declarations = declarationList;
		token = *scanner_->peekToken();
		if (token.getType() == TokenType::kw_begin)
		{
			token = *scanner_->nextToken();
			for (const StatementNode* statement : statement_sequence()) {
				pbN.statements.push_back(statement);
			}
		}
		if (token.getType() == TokenType::kw_end)
		{
			token = *scanner_->nextToken();
			if (token.getType() == TokenType::const_ident)
			{
				pbN.ident = ident();
				return &pbN;
			}
			else {
				// SYNTAX ERROR: No valid procedure end identifier
			}
		}
		else {
			// SYNTAX ERROR: "END" is missing
		}
	}
	else {
		// SYNTAX ERROR: Procedure has no declarations
	}
	return nullptr;
}

const std::vector<const FormalParameterNode*> Parser::formal_parameters() {
	/*
	"("
	[FPSection
		{";" FPSection} -> repetition
	] -> optional
	")"
	*/
	Token token = *scanner_->peekToken();
	std::vector<const FormalParameterNode*> formalParameters;
	if (token.getType() == TokenType::lparen)
	{
		token = *scanner_->nextToken();
		if (token.getType() == TokenType::kw_var || token.getType() == TokenType::const_ident)
		{
			token = *scanner_->nextToken();
			bool shouldProceed = true;
			while (shouldProceed)
			{
				const FormalParameterNode* fpN = fp_section();
				if (fpN != nullptr)
				{
					formalParameters.push_back(fpN);
					token = *scanner_->peekToken();
					if (token.getType() == TokenType::semicolon)
					{
						token = *scanner_->nextToken();
						if (token.getType() != TokenType::kw_var || token.getType() != TokenType::const_ident)
						{
							shouldProceed = false;
						}
					}
					else {
						shouldProceed = false;
					}
				}
				else {
					shouldProceed = false;
					// SYNTAX ERROR: formalParameters are not valid
				}
			}
		}
		if (token.getType() == TokenType::rparen)
		{
			*scanner_->nextToken();
		}
		else {
			// SYNTAX ERROR: ")" is missing
		}
	}
	else {
		// SYNTAX ERROR: "(" is missing
	}
	return formalParameters;
}

const FormalParameterNode* Parser::fp_section() {
	/*
	["VAR"]					-> Optional
	IdentList ":" type
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_var)
	{
		token = *scanner_->nextToken();
	}
	if (token.getType() == TokenType::const_ident)
	{
		FormalParameterNode fpN;
		std::vector<const std::string> idents = ident_list();
		if (idents.size() > 0)
		{
			for (std::string ident : ident_list()) {
				fpN.identList.push_back(ident);
			}
			token = *scanner_->peekToken();
			if (token.getType() == TokenType::colon)
			{
				token = *scanner_->nextToken();
				const TypeNode* tN = type();
				if (tN != nullptr)
				{
					fpN.type = tN;
					return &fpN;
				}
				else {
					// SYNTAX ERROR
				}
			}
			else {
				// SYNTAX ERROR
			}
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

const std::vector<const StatementNode*> Parser::statement_sequence() {
	/*
	statement
	{";" statement}				-> Repetation
	*/
	std::vector<const StatementNode*> statementList;
	bool shouldProceed = true;
	while (shouldProceed)
	{
		const StatementNode* statementNode = statement();
		if (statementNode != nullptr)
		{
			statementList.push_back(statementNode);
			Token token = *scanner_->peekToken();
			if (token.getType() == TokenType::semicolon)
			{
				token = *scanner_->nextToken();
			}
			else {
				shouldProceed = false;
			}
		}
	}
	return statementList;
}

const StatementNode* Parser::statement() {
	/*
	[assignment | ProcedureCall | IfStatement | WhileStatement] -> Optional
	*/
	Token token = *scanner_->peekToken();
	switch (token.getType())
	{
	case TokenType::kw_if:
		return if_statement();
		break;
	case TokenType::kw_while:
		return while_statement();
		break;
	case TokenType::const_ident:
		assignment();					// **!!**!!**!!
		procedure_call();				// **!!**!!**!!
		break;
	default:
		return nullptr;
		break;
	}
}

const AssignmentStatementNode* Parser::assignment() {
	/*
	ident selector ":=" expression
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::const_ident)
	{
		std::string _ident = ident();
		token = *scanner_->nextToken();
		std::vector<const SelectorNode*> selectorNodes = selector(); // MOVES NEXT TOKEN
		if (selectorNodes.size() > 0)
		{
			token = *scanner_->peekToken();
			if (token.getType() != TokenType::op_becomes)
			{
				const ExpressionNode* eN = expression();
				if (eN != nullptr)
				{
					AssignmentStatementNode asN;
					asN.ident = _ident;
					asN.selectors = selectorNodes;
					asN.expression = eN;
					return &asN;
				}
				else {
					// SYNTAX ERROR: No valid expression
				}
			}
			else {
				// SYNTAX ERROR
			}
		}
		else {
			// NO SELECTORS
		}
	}
	else {
		return nullptr;
	}
}

const ProcedureCallStatementNode* Parser::procedure_call() {
	/*
	ident selector
	[ActualParameters] -> Optional
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::const_ident)
	{
		ProcedureCallStatementNode pcsN(ident());
		token = *scanner_->nextToken();
		std::vector<const SelectorNode*> selectors = selector(); // MOVES NEXT TOKEN
		if (selectors.size() > 0)
		{
			pcsN.selectors = selectors;
			std::vector<const ExpressionNode*> actualParameters = actual_parameters();
			if (actualParameters.size() > 0)	// MOVES NEXT TOKEN
			{
				pcsN.actualParameters = actualParameters;
			}
			return &pcsN;
		}
		else {
			// NO SELECTOR
		}
	}
	else {
		// SYNTAX ERROR: No Procedure name
	}
	return nullptr;
}

const IfStatementNode* Parser::if_statement() {
	/*
	"IF" expression "THEN" StatementSequence
	{"ELSIF" expression "THEN" StatementSequence} -> repetition
	["ELSE" StatementSequence] -> Optional
	"END"
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_if)
	{
		IfStatementNode isN;
		const ExpressionNode* eN = expression(); // MOVES NEXT TOKEN
		if (eN != nullptr)
		{
			isN.expression = eN;
			token = *scanner_->peekToken();
			if (token.getType() == TokenType::kw_then)
			{
				token = *scanner_->nextToken();
				std::vector<const StatementNode*> statements = statement_sequence();	// MOVES NEXT TOKEN
				token = *scanner_->peekToken();
				if (statements.size() > 0)
				{
					isN.statements = statements;
					if (token.getType() == TokenType::kw_elsif)
					{
						bool shouldProceed = true;
						while (shouldProceed)
						{
							token = *scanner_->nextToken();
							ElseIfNode eiN;
							const ExpressionNode* eN = expression();
							token = *scanner_->peekToken();
							if (eN != nullptr)
							{
								eiN.expression = eN;
								if (token.getType() == TokenType::kw_then) {
									std::vector<const StatementNode*> statementsEi = statement_sequence();	// MOVES NEXT TOKEN
									token = *scanner_->peekToken();
									if (statementsEi.size() > 0)
									{
										eiN.statements = statementsEi;
										isN.elseIfNodes.push_back(&eiN);
										if (token.getType() != TokenType::kw_elsif)
										{
											shouldProceed = false;
										}
									}
									else {
										// SYNTAX ERROR: No statements in elsIf clause
										shouldProceed = false;
									}
								}
								else {
									// SYNTAX ERROR: "THEN" keyword is missing in elsIf clause
									shouldProceed = false;
								}
							}
							else {
								// SYNTAX ERROR: No expression in elsIf clause
							}
						}
					}
					if (token.getType() == TokenType::kw_else)
					{
						token = *scanner_->nextToken();
						ElseNode elseN;
						std::vector<const StatementNode*> statementsElseN = statement_sequence();	// MOVES NEXT TOKEN
						token = *scanner_->peekToken();
						if (statementsElseN.size() > 0)
						{
							elseN.statements = statementsElseN;
							isN.elseNode = &elseN;
						}
						else {
							// SYNTAX ERROR: No statement in else clause
						}
					}
					token = *scanner_->peekToken();
					if (token.getType() == TokenType::kw_end)
					{
						scanner_->nextToken();
						return &isN;
					}
					else {
						// SYNTAX ERROR: "END" keyword is missing
					}
				}
				else {
					// SYNTAX ERROR: No statements in If clause
				}
			}
			else {
				// SYNTAX ERROR: "THEN" keyword is missing
			}
		}
		else {
			// SYNTAX ERROR: No valid expression
		}
	}
	return nullptr;
}

const WhileStatementNode* Parser::while_statement() {
	/*
	"WHILE" expression "DO" StatementSequence "END"
	*/
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::kw_while)
	{
		token = *scanner_->nextToken();
		const ExpressionNode* eN = expression(); // MOVES NEXT TOKEN
		if (eN != nullptr)
		{
			token = *scanner_->peekToken();
			if (token.getType() == TokenType::kw_do)
			{
				token = *scanner_->nextToken();
				std::vector<const StatementNode*> statements = statement_sequence();
				if (statements.size() > 0)
				{
					token = *scanner_->peekToken();
					if (token.getType() == TokenType::kw_end)
					{
						WhileStatementNode wsN;
						wsN.expression = eN;
						wsN.statements = statements;
						return &wsN;
					}
					else {
						// SYNTAX ERROR: "END" keyword is missing
					}
				}
				else {
					// SYNTAX ERROR: No statements in WHILE scope
				}
			}
			else {
				// SYNTAX ERROR: "DO" keywrod is missing in WHILE scope
			}
		}
		else {
			// SYNTAX ERROR: NO expression in WHILE condition
		}
	}
	return nullptr;
}

const std::vector<const ExpressionNode*> Parser::actual_parameters() {
	/*
	"("
	[expression
		{ "," expression }				-> Repetition
	]									-> Optional
	")"
	*/
	std::vector<const ExpressionNode*> actualParameters;
	Token token = *scanner_->peekToken();
	if (token.getType() == TokenType::lparen)
	{
		token = *scanner_->nextToken();
		const ExpressionNode* eN = expression();// MOVES NEXT TOKEN
		if (eN != nullptr)						// Optinal
		{
			actualParameters.push_back(eN);
			bool shouldRepeat = true;
			while (shouldRepeat)				// Repetiton
			{
				token = *scanner_->peekToken();
				if (token.getType() == TokenType::comma)
				{
					token = *scanner_->nextToken();
					const ExpressionNode* exN = expression();
					if (exN != nullptr)			// MOVES NEXT TOKEN
					{
						actualParameters.push_back(exN);
					}
					else {
						// SYNTAX ERROR; No valid expression
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

		if (token.getType() == TokenType::rparen)
		{
			*scanner_->nextToken();
			return actualParameters;
		}
		else {
			// SYNTAX ERROR: actual_parameters have to finish ")"
		}
	}
	else {
		// SYNTAX ERROR: actual_parameters have to start "("
	}
	return actualParameters;
}

const std::vector<const SelectorNode*> Parser::selector() {
	/*
	{"." ident | "[" expression "]"} -> repetition
	*/
	Token token = *scanner_->peekToken();
	std::vector<const SelectorNode*> selectors;
	bool shouldRepeat = true;
	while (shouldRepeat)
	{
		if (token.getType() == TokenType::comma)
		{
			token = *scanner_->nextToken();
			if (token.getType() == TokenType::const_ident)
			{
				const IdentSelectorNode identSelector(ident());
				selectors.push_back(&identSelector);
				token = *scanner_->nextToken();
				if (token.getType() != TokenType::comma || token.getType() != TokenType::lbrack)
				{
					shouldRepeat = false;
				}
			}
			else {
				// SYNTAX ERROR
				shouldRepeat = false;
			}
		}
		else if (token.getType() == TokenType::lbrack) {
			token = *scanner_->nextToken();
			const ExpressionNode* expressionNode = expression(); // MOVES NEXT TOKEN
			if (expressionNode != nullptr)
			{
				const IndexSelectorNode indexSelector(expressionNode);
				selectors.push_back(&indexSelector);
				if (token.getType() == TokenType::rbrack)
				{
					token = *scanner_->nextToken();
					if (token.getType() != TokenType::comma || token.getType() != TokenType::lbrack)
					{
						shouldRepeat = false;
					}
				}
				else {
					shouldRepeat = false;
					// SYNTAX ERROR
				}
			}
			else {
				// SYNTAX ERROR
			}
		}
		else {
			// SYNTAX ERROR
			shouldRepeat = false;
		}
	}
	return selectors;
}
