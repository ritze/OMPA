/*
 * Array.cpp
 *
 *  Created on: 26.04.2014
 *      Author: Moritz Lüdecke
 */

#include <stddef.h>

#include "Array.h"

Array::Array(Parameter* size) :
		size(size) {
}

Array::~Array() {
	for (ArrayElementList::iterator element = elements.begin();
	        element != elements.end(); ++element) {

		if ((*element) != NULL) {
			delete (*element);
		}
	}

	if (size != NULL) {
		delete size;
	}
}

#ifdef DEBUG
std::string Array::getDescription() {
	std::ostringstream oss;

	for (ArrayElementList::iterator element = elements.begin();
			element != elements.end(); ++element) {

		oss << "    " << (*element)->getDescription() << "\n";
	}

	return oss.str();
}

ArrayElement* Array::getArrayElement(uint index) {
	return elements[index];
}

uint Array::getElementsSize() {
	return elements.size();
}

#endif /* DEBUG */

uint Array::getTimestamp() {
	uint timestamp = elements.size();

	if (!elements.empty()
	        && (elements.back()->getParameter() == NULL
	                || !elements.back()->getParameter()->isOccupied())) {

		timestamp--;
	}

	return timestamp;
}

Parameter* Array::getElement(Parameter* index, uint timestamp) {
	if ((size == NULL || index->getValue() < size->getValue())
	        && index->getValue() >= 0 && timestamp <= elements.size()) {

		for (uint i = timestamp; i > 0; --i) {
			int realI = i - 1;

			if (elements[realI] != NULL
			        && (elements[realI]->getIndex()->getValue()
			                == index->getValue())
			        // Special case for ParmArrayDecl
			        && elements[realI]->getParameter()->isOccupied()) {

				return elements[realI]->getParameter();
			}
		}
	}

	return NULL;
}

Parameter* Array::getSize() {
	return size;
}

bool Array::pushElement(Parameter* index, Parameter* var) {
	if (index == NULL || var == NULL || index->getValue() < 0) {
		return false;
	}

	ArrayElement* element = new ArrayElement(index, var);
	elements.push_back(element);

	return true;
}

