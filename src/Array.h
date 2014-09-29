/*
 * Array.h
 *
 *  Created on: 26.04.2014
 *      Author: Moritz Lüdecke
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include <vector>
#include <stdint.h>
#include <stdlib.h>

#include "ArrayElement.h"
#include "Operation.h"

#ifdef DEBUG
#include <sstream>
#include <string>
#endif /* DEBUG */

class ArrayElement;
class Parameter;

typedef std::vector<ArrayElement*> ArrayElementList;

class Array {
public:
	// Use size = NULL, if size is unknown
	Array(Parameter* size);
	~Array();

#ifdef DEBUG
	std::string getDescription();
	// A Wrapper to elements list for test classes
	ArrayElement* getArrayElement(uint index);
	uint getElementsSize();
#endif /* DEBUG */
	uint getTimestamp();
	// Returns NULL, if element with given index was not found
	Parameter* getElement(Parameter* index, uint timestamp);
	// Returns NULL, if size is undefined
	Parameter* getSize();
	// Returns true, if there was a variable free to fill, otherwise false.
	bool pushElement(Parameter* index, Parameter* var);

private:
	Parameter* size;
	ArrayElementList elements;
};

#endif /* ARRAY_H_ */
