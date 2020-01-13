#include "ast/Module/Module.h"

Module::Module(std::string _name, const FilePos _filePos) : Node(NodeType::module, _filePos), name(_name) {

}

bool Module::addDeclaration(const Declaration* declaration) {
	for (auto& d : declarations)
	{
		if (d->identifier == declaration->identifier)
		{
			return false;
		}
	}
	declarations.push_back(declaration);
	return true;
}

bool Module::checkName(std::string _name)
{
	return (this->name == _name) ? true : false;
}
