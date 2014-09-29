/*
 * MathTest.cpp
 *
 *  Created on: 05.02.2014
 *      Author: Moritz Lüdecke
 */

#include <src/Analyse.h>

#include "MathTest.h"

static std::string complicatedMathTestKernel =
		"__kernel void test() {       \n"
		"	int a = (1 + 2) - 2 * 2;  \n"
		"	a *= -((11 % 3) * 3);     \n"
		"	a /= -a / 2;              \n"
		"}";

static std::string simpleMathTestKernel =
		"__kernel void test() {  \n"
		"	int a = 1 + 2;       \n"
		"	a = 1 - 2;           \n"
		"	a = 2 * 3;           \n"
		"	a = 4 / 2;           \n"
		"	a = 4 % 3;           \n"
		"	a = 2 & 1;           \n"
		"	a = 2 | 1;           \n"
		"	a = 2 ^ 1;           \n"
		"	a = 2 << 1;          \n"
		"	a = 2 >> 1;          \n"
		"	a += 1;              \n"
		"	a -= 1;              \n"
		"	a *= 6;              \n"
		"	a /= 2;              \n"
		"	a %= 2;              \n"
		"	a &= 2;              \n"
		"	a |= 2;              \n"
		"	a ^= 2;              \n"
		"	a <<= 2;             \n"
		"	a >>= 2;             \n"
		"	a = -2;              \n"
		"	a = +2;              \n"
		"}";

bool MathTest::run() {
	bool result = true;

	result &= test("  testSimpleMath:          ",
	               (testFunction) &MathTest::testSimpleMath);
	result &= test("  testComplicatedMath:     ",
	               (testFunction) &MathTest::testComplicatedMath);

	return result;
}

bool MathTest::testComplicatedMath() {
	const std::string code = complicatedMathTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();

	CHECK(isOccupied(varDecls), 0, ON_PRECONDITION);
	CHECK(varDecls.size() == 3, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();

	int a = (1 + 2) - 2 * 2;
	CHECK((*it)->Parameter->getValue() == a,   ++line, code);

	a *= -((11 % 3) * 3);
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a /= -a / 2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool MathTest::testSimpleMath() {
	const std::string code = simpleMathTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();

	CHECK(isOccupied(varDecls),  0, ON_PRECONDITION);
	CHECK(varDecls.size() == 22, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();

	int a = 1 + 2;
	CHECK((*it)->Parameter->getValue() == a,   ++line, code);

	a = 1 - 2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a = 2 * 3;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a = 4 / 2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a = 4 % 3;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a = 2 & 1;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a = 2 | 1;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a = 2 ^ 1;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a = 2 << 1;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a = 2 >> 1;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a += 1;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a -= 1;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a *= 6;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a /= 2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a %= 2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a &= 2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a |= 2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a ^= 2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a <<= 2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a >>= 2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a = -2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	a = +2;
	CHECK((*++it)->Parameter->getValue() == a, ++line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}
