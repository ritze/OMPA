/*
 * ArrayTest.h
 *
 *  Created on: 23.04.2014
 *      Author: Moritz Lüdecke
 */

#ifndef ARRAYTEST_H_
#define ARRAYTEST_H_

#include "Test.h"

class ArrayTest: Test {
	friend class Test;

public:
	bool run();

private:
	bool testCalc();
	bool testCalcIndex();
	bool testSimpleArray();
	bool testInitArray();

};

#endif /* ARRAYTEST_H_ */
