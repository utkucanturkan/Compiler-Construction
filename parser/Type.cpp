#include "Type.h"
#include "ArrayType.h"
#include "RecordType.h"

Type::Type(PrimitiveType _primitiveType): primitiveType(_primitiveType) { }

ArrayType::ArrayType(std::shared_ptr<const Expression> _expression, std::shared_ptr<const Type> _type): Type(PrimitiveType::Array), expression(_expression), type(_type) { }

RecordType::RecordType(): Type(PrimitiveType::Record) {}