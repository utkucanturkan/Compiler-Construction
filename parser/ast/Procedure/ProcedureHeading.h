#pragma once

#include "ast/Procedure/FormalParameter.h"

class ProcedureHeading {
public:
	explicit ProcedureHeading(std::string _ident);
	std::string ident;
	std::vector<const FormalParameter*> formalParameters;
};