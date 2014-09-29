/*
 * MathTest.h
 *
 *  Created on: 05.02.2014
 *      Author: Moritz Lüdecke
 */

#ifndef MATHTEST_H_
#define MATHTEST_H_

#include "Test.h"

class MathTest: Test {
	friend class Test;

public:
	bool run();

private:
	bool testComplicatedMath();
	bool testSimpleMath();

};

#endif /* MATHTEST_H_ */
