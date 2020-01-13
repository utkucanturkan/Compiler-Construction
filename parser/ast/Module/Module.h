#pragma once

#include <string>
#include <vector>

#include "ast/Node.h"
#include "ast/Declaration/Declaration.h"
#include "ast/Statement/Statement.h"

class Module : public Node {
public:
	Module(std::string _name, const FilePos _filePos);
	bool addDeclaration(const Declaration*);
	void addStatement(const Statement*);
	bool checkName(std::string _name);
	void print(std::ostream& stream) const override;
private:
	std::string name;
	std::vector<const Declaration*> declarations;
	std::vector<const Statement*> statements;
};