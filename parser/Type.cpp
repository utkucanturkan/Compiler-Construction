#include "Type.h"
#include "ArrayType.h"
#include "RecordType.h"

Type::Type(PrimitiveType _primitiveType): primitiveType(_primitiveType), Node(NodeType::basic_type) { }

ArrayType::ArrayType(std::shared_ptr<const Expression> _expression, std::shared_ptr<const Type> _type): Type(PrimitiveType::Array), expression(_expression), type(_type) { 
	nodeType_ = NodeType::array_type;
}

RecordType::RecordType(): Type(PrimitiveType::Record) {
	nodeType_ = NodeType::record_type;
}