#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Variable.h"
#include "Statement.h"
#include "ast/Node.h"


class Module: public Node {
public:
	explicit Module(std::string _identifier);
	std::string identifier;
	std::vector<std::shared_ptr<const Variable>> declarations;
	std::vector<std::shared_ptr<const Statement>> statements;
};