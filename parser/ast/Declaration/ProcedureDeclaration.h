#pragma once

#include "ast/Procedure/ProcedureHeading.h"
#include "ast/Procedure/ProcedureBody.h"
#include "ast/Declaration/Declaration.h"

class ProcedureDeclaration : public Declaration {
public:
	explicit ProcedureDeclaration(const ProcedureHeading* _heading, const ProcedureBody* _body);
	const ProcedureHeading* heading;
	const ProcedureBody* body;
};