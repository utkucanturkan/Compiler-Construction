#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Variable.h"
#include "Statement.h"

class Module {
public:
	explicit Module(std::string _identifier);
	bool addDeclaration(std::shared_ptr<const Variable> declaration);
	void addStatement(std::shared_ptr<const Statement> statement);
	bool checkName(std::string _identifier);
private:
	std::string identifier;
	std::vector<std::shared_ptr<const Variable>> declarations;
	std::vector<std::shared_ptr<const Statement>> statements;
};