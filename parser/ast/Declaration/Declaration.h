#pragma once

#include<string>

class Declaration {
public:
	explicit Declaration(std::string _ident);
	std::string identifier;
};