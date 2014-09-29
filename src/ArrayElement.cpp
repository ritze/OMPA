/*
 * ArrayElement.cpp
 *
 *  Created on: 26.04.2014
 *      Author: Moritz Lüdecke
 */

#include <stddef.h>

#include "ArrayElement.h"

ArrayElement::ArrayElement(Parameter* index, Parameter* parameter) :
		index(index), parameter(parameter) {
}

ArrayElement::~ArrayElement() {
	if (index != NULL) {
		delete index;
	}

	if (parameter != NULL) {
		delete parameter;
	}
}

#ifdef DEBUG
std::string ArrayElement::getDescription() {
	std::ostringstream oss;

	oss << "[" << index->getValue() << "] ";

	if (parameter != NULL && parameter->isOccupied()) {
		oss << parameter->getValue();

	} else {
		oss << "NULL";
	}

	return oss.str();
}
#endif /* DEBUG */

Parameter* ArrayElement::getIndex() {
	return index;
}

Parameter* ArrayElement::getParameter() {
	return parameter;
}
