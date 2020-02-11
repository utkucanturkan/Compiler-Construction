#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Variable.h"
#include "Statement.h"

class Module {
public:
	explicit Module(std::string _identifier);
	bool checkName(std::string _identifier);
	std::string identifier;
	std::vector<std::shared_ptr<const Variable>> declarations;
	std::vector<std::shared_ptr<const Statement>> statements;
};