#pragma once

#include <memory>
#include "Type.h"

class Factor {
public:
	Factor(PrimitiveType _type);
	PrimitiveType type;
};