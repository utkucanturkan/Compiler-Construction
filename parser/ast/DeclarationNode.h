#pragma once

class DeclarationNode {
public:
	explicit DeclarationNode(std::string _ident, std::string _type);
	std::string identifier;
	std::string type;
};