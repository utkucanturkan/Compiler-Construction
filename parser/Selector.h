#pragma once

#include <memory>
#include <map>

#include "Expression.h"

enum class SelectorType
{
	Array,
	Record
};

class Selector {
public:
	SelectorType type;
	std::map<int, std::shared_ptr<const Selector>> innerSelectors;
};

class ArraySelector: public Selector {
public:
	explicit ArraySelector(std::shared_ptr<const Expression> _expression);
	std::shared_ptr<const Expression> expression;
};