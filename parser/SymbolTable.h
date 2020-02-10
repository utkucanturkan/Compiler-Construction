#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "ast/Node.h"

class SymbolTable
{
private:
	std::unordered_map<std::string, const Node*> table_;

public:
	explicit SymbolTable();
	~SymbolTable();

	void beginBlock();
	void insert(const std::string& name, const Node* node);
	const Node* lookup(const std::string& name) const;
	void endBlock();
};