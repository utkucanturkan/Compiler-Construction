//
// Created by Michael Grossniklaus on 2/2/18.
//

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


#include "../scanner/Scanner.h"
#include "ast/Node.h"
#include "ast/ModuleNode.h"
#include "ast/ConstDeclarationNode.h"
#include "ast/TypeDeclarationNode.h"
#include "ast/VarDeclarationNode.h"
#include "ast/ProcedureDeclarationNode.h"

class Parser
{

private:
    Scanner *scanner_;
    Logger *logger_;

    const std::string ident();

    const ModuleNode* module();
    const std::vector<const DeclarationNode*> declarations();
    const std::vector<const ConstDeclarationNode*> const_declarations();
    const std::vector<const TypeDeclarationNode*> type_declarations();
    const std::vector<const VarDeclarationNode*> var_declarations();
    const ProcedureDeclarationNode* procedure_declaration();
    const ExpressionNode* expression();
    const SimpleExpressionNode* simple_expression();
    const TermNode* term();
    const FactorNode* factor();
    const TypeNode* type();
    const ArrayTypeNode* array_type();
    const RecordTypeNode* record_type();
    const FieldListNode* field_list();
    const std::vector<const std::string> ident_list();
    const ProcedureHeadingNode* procedure_heading();
    const ProcedureBodyNode* procedure_body();
    const std::vector<const FormalParameterNode*> formal_parameters();
    const FormalParameterNode* fp_section();
    const std::vector<const StatementNode*> statement_sequence();
    const StatementNode* statement();
    const AssignmentStatementNode* assignment();
    const ProcedureCallStatementNode* procedure_call();
    const IfStatementNode* if_statement();
    const WhileStatementNode* while_statement();
    const std::vector<const ExpressionNode*> actual_parameters();
    const std::vector<const SelectorNode*> selector();

public:
    explicit Parser(Scanner *scanner, Logger *logger);
    ~Parser();
    const std::unique_ptr<const Node> parse();
};


#endif //OBERON0C_PARSER_H
