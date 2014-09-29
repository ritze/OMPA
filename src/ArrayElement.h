/*
 * ArrayElement.h
 *
 *  Created on: 26.04.2014
 *      Author: Moritz Lüdecke
 */

#ifndef ARRAYELEMENT_H_
#define ARRAYELEMENT_H_

#include "Operation.h"

#ifdef DEBUG
#include <sstream>
#include <string>
#endif /* DEBUG */

class Parameter;
class Variable;

class ArrayElement {
public:
	ArrayElement(Parameter* index, Parameter* parameter);
	~ArrayElement();

#ifdef DEBUG
	std::string getDescription();
#endif /* DEBUG */
	Parameter* getIndex();
	Parameter* getParameter();

private:
	Parameter* index;
	Parameter* parameter;
};

#endif /* ARRAYELEMENT_H_ */
