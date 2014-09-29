/*
 * ParameterPattern.h
 *
 *  Created on: 12.05.2014
 *      Author: Moritz Lüdecke
 */

#ifndef PARAMETERPATTERN_H_
#define PARAMETERPATTERN_H_

enum PatternType {
	PatternType_None, PatternType_Variable, PatternType_Array
};

struct ParameterPattern {
	ParameterPattern(const char* name) :
			name(name), type(PatternType_None), reads(NULL), writes(NULL),
			readsSize(0), writesSize(0) {
	};

	~ParameterPattern() {
		if (reads != NULL) {
			delete reads;
		}

		if (writes != NULL) {
			delete writes;
		}
	}

	const char* name;

	PatternType type;

	bool* reads;
	uint readsSize;

	bool* writes;
	uint writesSize;
};

#endif /* PARAMETERPATTERN_H_ */
