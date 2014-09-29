/*
 * FunctionPattern.cpp
 *
 *  Created on: 12.05.2014
 *      Author: Moritz Lüdecke
 */

#include "FunctionPattern.h"

FunctionPattern::FunctionPattern(const char* name,
                                 ParameterPatternList* parameters) :
		name(name), parameters(parameters) {
}

const char* FunctionPattern::getName() {
	return name;
}

uint FunctionPattern::getParameterSize() {
	return parameters->size();
}

ParameterPattern* FunctionPattern::getParameter(uint index) {
	if (index < parameters->size()) {
		return parameters->at(index);

	} else {
		return NULL;
	}
}
