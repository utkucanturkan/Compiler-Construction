#include "Module.h"

Module::Module(std::string _identifier) : identifier(_identifier) { }

bool Module::addDeclaration(std::shared_ptr<const Variable> declaration) {
	for (auto& d : declarations)
	{
		if (d->identifier == declaration->identifier)
		{
			return false;
		}
		else {
			declarations.emplace_back(declaration);
		}
	}
	return true;
}

void Module::addStatement(std::shared_ptr<const Statement> statement)
{
	statements.emplace_back(statement);
}

bool Module::checkName(std::string _identifier)
{
	return (this->identifier == _identifier) ? true : false;
}
