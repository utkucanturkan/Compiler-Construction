#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "ast/Node.h"

class SymbolTable
{
private:
	std::unordered_map<std::string, std::shared_ptr<const Node>> table_;

public:
	explicit SymbolTable();
	~SymbolTable();

	void beginBlock();
	void insert(const std::string& name, std::shared_ptr<const Node> node);
	const Node* lookup(const std::string& name) const;
	void endBlock();
};