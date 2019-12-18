#include "ModuleNode.h"

ModuleNode::ModuleNode(std::string _name, FilePos filePos) : Node(NodeType::module, filePos), name(_name) {

}

bool ModuleNode::checkName(std::string endName) {
	return (this->name == endName) ? true : false;
}

void ModuleNode::addDeclaration(const DeclarationNode* declaration) {
	bool isDeclarationOk = true;
	for (auto& d : declarations)
	{
		if (d->name == declaration->name /*&& d->type == declaration->type*/)
		{
			isDeclarationOk = false;
			break;
		}
	}

	if (isDeclarationOk)
	{
		declarations.push_back(declaration);
	}
	else {
		throw;
	}
}

void ModuleNode::addStatement(const StatementNode* statement)
{
	statements.push_back(statement);
}

void ModuleNode::print(std::ostream& stream) const
{

}

