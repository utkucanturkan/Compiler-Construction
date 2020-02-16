#include "SymbolTable.h"

SymbolTable::SymbolTable() { }

SymbolTable::~SymbolTable() = default;


void SymbolTable::insert(const std::string& name, std::shared_ptr<const Node> node)
{
	table_.insert({name, node});
}

std::shared_ptr<const Node> SymbolTable::lookup(const std::string& name) const
{
	auto element = table_.find(name);
	if (element != table_.end())
	{
		return element->second;
	}
	return nullptr;
}