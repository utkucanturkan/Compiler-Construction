#pragma once

#include <vector>
#include <memory>

#include "Type.h"
#include "Variable.h"

class RecordType : public Type {
public:
	explicit RecordType();
	std::vector<std::shared_ptr<const Variable>> fieldListNodes;
};