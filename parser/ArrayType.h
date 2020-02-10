#pragma once

#include <memory>
#include "Type.h"
#include "Expression.h"
 
class ArrayType : public Type {
public:
	explicit ArrayType(std::shared_ptr<const Expression> _expression, std::shared_ptr<const Type> _type);
	std::shared_ptr<const Expression> expression;
	std::shared_ptr<const Type> type;
};