/*
 * DeAndIncrementTest.h
 *
 *  Created on: 22.01.2014
 *      Author: Moritz Lüdecke
 */

#ifndef DEANDINCREMENTTEST_H_
#define DEANDINCREMENTTEST_H_

#include "Test.h"

class DeAndIncrementTest: Test {
	friend class Test;

public:
	bool run();

private:
	bool testCompoundAssignment();
	bool testDecAndInc();
	bool testFunctionParm();
	bool testPostDec();
	bool testPostInc();
	bool testPreDec();
	bool testPreInc();
};

#endif /* DEANDINCREMENTTEST_H_ */
