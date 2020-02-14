#include "Selector.h"
#include "RecordSelector.h"

ArraySelector::ArraySelector(std::shared_ptr<const Expression> _expression): expression(_expression) {
	type = SelectorType::Array;
}

RecordSelector::RecordSelector(std::shared_ptr<const Variable> _variable) : variable(_variable) {
	type = SelectorType::Record;
}