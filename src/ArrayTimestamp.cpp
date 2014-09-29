/*
 * ArrayTimestamp.cpp
 *
 *  Created on: 26.04.2014
 *      Author: Moritz Lüdecke
 */

#include "ArrayTimestamp.h"

ArrayTimestamp::ArrayTimestamp(Array* array, Parameter* index) :
		array(array), index(index) {

	timestamp = array->getTimestamp();
}

ArrayTimestamp::~ArrayTimestamp() {
	if (array != NULL) {
		delete array;
	}

	if (index != NULL) {
		delete index;
	}
}

#ifdef DEBUG
std::string ArrayTimestamp::getDescription() {
	std::ostringstream oss;

	int size = (uint) array->getSize()->getValue();

	oss << "Size: " << size << "\n";

	for (uint i = 0; i < size; ++i) {
		Parameter* index = new Parameter;
		index->pushVariable(i);

		Parameter* parameter= array->getElement(index, timestamp);
		oss << " [" << i << "] ";

		if (parameter != NULL && parameter->isOccupied()) {
			oss << parameter->getValue() << "\n";

		} else {
			oss << "NULL" << "\n";
		}

		delete index;
	}

	return oss.str();
}
#endif /* DEBUG */

int ArrayTimestamp::getValue() {
	Parameter* variable = array->getElement(index, timestamp);

	if (variable != NULL) {
		return variable->getValue();

	} else {
		return 0;
	}
}
