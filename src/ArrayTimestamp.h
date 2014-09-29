/*
 * ArrayTimestamp.h
 *
 *  Created on: 26.04.2014
 *      Author: Moritz Lüdecke
 */

#ifndef ARRAYTIMESTAMP_H_
#define ARRAYTIMESTAMP_H_

#include <stddef.h>
#include <stdlib.h>

#ifdef DEBUG
#include <sstream>
#include <string>
#endif /* DEBUG */

#include "Array.h"
#include "ArrayElement.h"
#include "Operation.h"

class Array;
class ArrayElement;
class Parameter;

class ArrayTimestamp {
public:
	ArrayTimestamp(Array* array, Parameter* index);
	~ArrayTimestamp();

#ifdef DEBUG
	std::string getDescription();
#endif /* DEBUG */
	int getValue();

private:
	Array* array;
	uint timestamp;
	Parameter* index;
};

#endif /* ARRAYTIMESTAMP_H_ */
