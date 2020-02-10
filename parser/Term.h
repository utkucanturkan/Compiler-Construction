#pragma once

#include <memory>
#include <vector>

#include "TermElement.h"
#include "Type.h"

class Term {
public:
	explicit Term();
	PrimitiveType type;
	std::vector<std::shared_ptr<const TermElement>> elements;
};