/*
 * DependenceTest.h
 *
 *  Created on: 24.01.2014
 *      Author: Moritz Lüdecke
 */

#ifndef DEPENDENCETEST_H_
#define DEPENDENCETEST_H_

#include "Test.h"

class DependenceTest: Test {
	friend class Test;

public:
	bool run();

private:
	bool testArray();
	bool testDecAndInc();
	bool testFunctionCall();
	bool testFunctionCallParm();
	bool testFunctionParm();
	bool testMultipleDecl();
	bool testSimple();
};

#endif /* DEPENDENCETEST_H_ */
