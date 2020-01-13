#pragma once

#include "ast/Type/Type.h"
#include "ast/Type/FieldList.h"

class RecordType : public Type {
public:
	std::vector<const FieldList*> fieldListNodes;
};