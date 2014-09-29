/*
 * FunctionPattern.h
 *
 *  Created on: 12.05.2014
 *      Author: Moritz Lüdecke
 */

#ifndef FUNCTIONPATTERN_H_
#define FUNCTIONPATTERN_H_

#include <stdlib.h>
#include <vector>

#include "ParameterPattern.h"

typedef std::vector<ParameterPattern*> ParameterPatternList;

class FunctionPattern {
public:
	FunctionPattern(const char* name, ParameterPatternList* parameters);

	// Returns the function name
	const char* getName();

	// Returns the size of parameters, which are hold by the function
	uint getParameterSize();

	// Returns the memory pattern of the given parameter
	ParameterPattern* getParameter(uint index);

private:
	const char* name;
	ParameterPatternList* parameters;
};

#endif /* FUNCTIONPATTERN_H_ */
