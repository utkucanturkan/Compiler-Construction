//
// Created by Michael Grossniklaus on 2/2/18.
//

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


#include "Scanner.h"
#include "ast/Node.h"

#include "Variable.h"
#include "ConstVariable.h"
#include "TypeVariable.h"
#include "VarVariable.h"
#include "ProcedureVariable.h"
#include "Module.h"

#include "Expression.h"
#include "Factor.h"

#include "Type.h"
#include "ArrayType.h"
#include "RecordType.h"

#include "Statement.h"
#include "IfStatement.h"
#include "WhileStatement.h"
#include "AssignmentStatement.h"
#include "ProcedureCallStatement.h"

#include "Selector.h"
#include "SymbolTable.h"

class Parser
{

private:
    Scanner *scanner_;
    Logger *logger_;
    std::unique_ptr<const Token> token_;
    SymbolTable symbolTable_;
    const std::string ident();

    std::unique_ptr<const Module> module();
    const std::vector<std::shared_ptr<const Variable>> declarations();
    const std::vector<std::shared_ptr<const ConstVariable>> const_declarations();
    const std::vector<std::shared_ptr<const TypeVariable>> type_declarations();
    const std::vector<std::shared_ptr<const VarVariable>> var_declarations();
    std::shared_ptr<const ProcedureVariable> procedure_declaration();
    std::shared_ptr<const Expression> expression();
    std::shared_ptr<const SimpleExpression> simple_expression();
    std::shared_ptr<const Term> term();
    std::shared_ptr<const Factor> factor();
    std::shared_ptr<const Type> type();
    std::shared_ptr<const ArrayType> array_type();
    std::shared_ptr<const RecordType> record_type();
    const std::vector<std::shared_ptr<const Variable>> field_list();
    const std::vector<std::string> ident_list();
    std::shared_ptr<const ProcedureHead> procedure_heading();
    std::shared_ptr<const ProcedureBody> procedure_body(const std::string _procedureIdentifier);
    const std::vector<std::shared_ptr<const Variable>> formal_parameters();
    const std::vector<std::shared_ptr<const Variable>> fp_section();
    const std::vector<std::shared_ptr<const Statement>> statement_sequence();
    std::shared_ptr<const Statement> statement();
    std::shared_ptr<const AssignmentStatement> assignment();
    std::shared_ptr<const ProcedureCallStatement> procedure_call();
    std::shared_ptr<const IfStatement> if_statement();
    std::shared_ptr<const WhileStatement> while_statement();
    const std::vector<std::shared_ptr<const Expression>> actual_parameters();
    const std::shared_ptr<const Selector>  selector();

public:
    explicit Parser(Scanner *scanner, Logger *logger);
    ~Parser();
    const std::unique_ptr<const Module> parse();
};


#endif //OBERON0C_PARSER_H
