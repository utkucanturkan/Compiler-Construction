#pragma once

#include <vector>
#include "Node.h"
#include "DeclarationNode.h"
#include "StatementNode.h"

class ModuleNode :public Node {
public:
	ModuleNode(std::string _name, FilePos filePos);

	bool checkName(std::string endName);
	void addDeclaration(const DeclarationNode* declaration);
	void addStatement(const StatementNode* statement);
	void print(std::ostream& stream) const override;
private:
	std::string name;
	std::vector<const DeclarationNode*> declarations;
	std::vector<const StatementNode*> statements;
};