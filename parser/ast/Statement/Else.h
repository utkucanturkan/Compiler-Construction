#pragma once


#include "ast/Statement/Statement.h"

class Else {
public:
	std::vector<const Statement*> statements;
};