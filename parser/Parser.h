//
// Created by Michael Grossniklaus on 2/2/18.
//

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


#include "Scanner.h"
#include "ast/Node.h"

#include <ast\Module\Module.h>

#include <ast\Declaration\Declaration.h>
#include <ast\Declaration\ConstDeclaration.h>
#include <ast\Declaration\TypeDeclaration.h>
#include <ast\Declaration\VarDeclaration.h>
#include <ast\Declaration\ProcedureDeclaration.h>

#include <ast\Expression\Expression.h>
#include <ast\Expression\SimpleExpression.h>
#include <ast\Expression\Term.h>
#include <ast\Expression\Factor.h>

#include <ast\Type\Type.h>
#include <ast\Type\ArrayType.h>
#include <ast\Type\RecordType.h>
#include <ast\Type\FieldList.h>

#include <ast\Procedure\ProcedureHeading.h>
#include <ast\Procedure\ProcedureBody.h>
#include <ast\Procedure\FormalParameter.h>

#include <ast\Statement\Statement.h>
#include <ast\Statement\AssignmentStatement.h>
#include <ast\Statement\ProcedureCallStatement.h>
#include <ast\Statement\IfStatement.h>
#include <ast\Statement\WhileStatement.h>

#include <ast\Selector\Selector.h>

class Parser
{

private:
    Scanner *scanner_;
    Logger *logger_;
    const Token* token_;

    const std::string ident();

    const Module* module();
    const std::vector<const Declaration*> declarations();
    const std::vector<const ConstDeclaration*> const_declarations();
    const std::vector<const TypeDeclaration*> type_declarations();
    const std::vector<const VarDeclaration*> var_declarations();
    const ProcedureDeclaration* procedure_declaration();
    const Expression* expression();
    const SimpleExpression* simple_expression();
    const Term* term();
    const Factor* factor();
    const Type* type();
    const ArrayType* array_type();
    const RecordType* record_type();
    const FieldList* field_list();
    const std::vector<const std::string> ident_list();
    const ProcedureHeading* procedure_heading();
    const ProcedureBody* procedure_body();
    const std::vector<const FormalParameter*> formal_parameters();
    const FormalParameter* fp_section();
    const std::vector<const Statement*> statement_sequence();
    const Statement* statement();
    const AssignmentStatement* assignment();
    const ProcedureCallStatement* procedure_call();
    const IfStatement* if_statement();
    const WhileStatement* while_statement();
    const std::vector<const Expression*> actual_parameters();
    const std::vector<const Selector*> selector();

public:
    explicit Parser(Scanner *scanner, Logger *logger);
    ~Parser();
    const std::unique_ptr<const Node> parse();
};


#endif //OBERON0C_PARSER_H
