/*
 * DeAndIncrementTest.cpp
 *
 *  Created on: 22.01.2014
 *      Author: Moritz Lüdecke
 */

#include <src/Analyse.h>

#include "DeAndIncrementTest.h"

static std::string compoundAssignmentTestKernel =
		"__kernel void test() {   \n"
		"	int k = 0;            \n"
		"	int i = 0;            \n"
		"	k += 2;               \n"
		"	k *= 2;               \n"
		"	k /= 2;               \n"
		"	k %= 2;               \n"
		"	k &= 3;               \n"
		"	k |= 4;               \n"
		"	k ^= 2;               \n"
		"	k <<= 3;              \n"
		"	k >>= 2;              \n"
		"	k += ++i;             \n"
		"	k += i++;             \n"
		"	k = ++i * (++i + i);  \n"
		"	k += k++ + ++k;       \n"
		"}";

static std::string decAndIncTestKernel =
		"__kernel void test(int a) {  \n"
		"	int k = 0;                \n"
		"	int i = 0;                \n"
		"	k++ + i--;                \n"
		"	--k - ++i;                \n"
		"	k-- - i++;                \n"
		"	++k + --i;                \n"
		"	int j = k++ + ++i;        \n"
		"	j = --k + i--;            \n"
		"	j = --k + ++i;            \n"
		"	k = a;                    \n"
		"	i = k++ + 10;             \n"
		"	i = ++k + 10;             \n"
		"}";

static std::string functionParmTestKernel =
		"__kernel void test(int a) {  \n"
		"	a++;                      \n"
		"	++a;                      \n"
		"	a--;                      \n"
		"	--a;                      \n"
		"	int k = a++;              \n"
		"	k += a;                   \n"
		"	k += ++a;                 \n"
		"	k += ++a + 1;             \n"
		"	int i = ++k + a--;        \n"
		"}";

static std::string postDecTestKernel =
		"__kernel void test() {  \n"
		"	int k = 0;           \n"
		"	int i = 0;           \n"
		"	k--;                 \n"
		"	k-- + 1;             \n"
		"	1 + k--;             \n"
		"	k-- + i--;           \n"
		"	i-- + k--;           \n"
		"	i = k--;             \n"
		"	i = k-- + 1;         \n"
		"	i = 1 + k--;         \n"
		"	i = k-- + i--;       \n"
		"	i = i-- + k--;       \n"
		"	int j = i-- + 10;    \n"
		"}";

static std::string postIncTestKernel =
		"__kernel void test() {  \n"
		"	int k = 0;           \n"
		"	int i = 0;           \n"
		"	k++;                 \n"
		"	k++ + 1;             \n"
		"	1 + k++;             \n"
		"	k++ + i++;           \n"
		"	i++ + k++;           \n"
		"	i = k++;             \n"
		"	i = k++ + 1;         \n"
		"	i = 1 + k++;         \n"
		"	i = k++ + i++;       \n"
		"	i = i++ + k++;       \n"
		"	int j = i++ + 10;    \n"
		"}";

static std::string preDecTestKernel =
		"__kernel void test() {  \n"
		"	int k = 0;           \n"
		"	int i = 0;           \n"
		"	--k;                 \n"
		"	--k + 1;             \n"
		"	1 + --k;             \n"
		"	--k + --i;           \n"
		"	--i + --k;           \n"
		"	i = --k;             \n"
		"	i = --k + 1;         \n"
		"	i = 1 + --k;         \n"
		"	i = --k + --i;       \n"
		"	i = --i + --k;       \n"
		"	int j = --i + 10;    \n"
		"}";

static std::string preIncTestKernel =
		"__kernel void test() {  \n"
		"	int k = 0;           \n"
		"	int i = 0;           \n"
		"	++k;                 \n"
		"	++k + 1;             \n"
		"	1 + ++k;             \n"
		"	++k + ++i;           \n"
		"	++i + ++k;           \n"
		"	i = ++k;             \n"
		"	i = ++k + 1;         \n"
		"	i = 1 + ++k;         \n"
		"	i = ++k + ++i;       \n"
		"	i = ++i + ++k;       \n"
		"	int j = ++i + 10;    \n"
		"}";

bool DeAndIncrementTest::run() {
	bool result = true;

	result &= test("  testCompoundAssignment:  ",
	               (testFunction) &DeAndIncrementTest::testCompoundAssignment);
	result &= test("  testPostDec:             ",
	               (testFunction) &DeAndIncrementTest::testPostDec);
	result &= test("  testPostInc:             ",
	               (testFunction) &DeAndIncrementTest::testPostInc);
	result &= test("  testPreDec:              ",
	               (testFunction) &DeAndIncrementTest::testPreDec);
	result &= test("  testPreInc:              ",
	               (testFunction) &DeAndIncrementTest::testPreInc);
	result &= test("  testDecAndInc:           ",
	               (testFunction) &DeAndIncrementTest::testDecAndInc);
	result &= test("  testFunctionParm:        ",
	               (testFunction) &DeAndIncrementTest::testFunctionParm);

	return result;
}

bool DeAndIncrementTest::testCompoundAssignment() {
	const std::string code = compoundAssignmentTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();

	CHECK(isOccupied(varDecls),  0, ON_PRECONDITION);
	CHECK(varDecls.size() == 21, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();

	int k = 0;
	CHECK((*it)->Parameter->getValue() == k,    ++line, code);

	int i = 0;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);

	k += 2;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k *= 2;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k /= 2;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k %= 2;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k &= 3;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k |= 4;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k ^= 2;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k <<= 3;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k >>= 2;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k += ++i;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	k += i++;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	/*k = ++i * (++i + i);*/
	int i2 = ++i;
	++i;
	k = i2 * (i + i);
	CHECK((*++it)->Parameter->getValue() == i2, ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	/*k += k++ + ++k;*/
	int k1 = k;
	int k2 = k + 1;
	int k3 = k + 2;
	k += (k1 + k3);
	CHECK((*++it)->Parameter->getValue() == k2, ++line, code);
	CHECK((*++it)->Parameter->getValue() == k3,   line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DeAndIncrementTest::testDecAndInc() {
	const std::string code = decAndIncTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();

	/* Set kernel function parameter */
	int a = 42;
	std::list<ParmDeclInfo*> parmVarDecls = function.getParmVarDecls();
	CHECK(!parmVarDecls.empty(), 0, ON_PRECONDITION);
	(*((ParmDeclInfoIterator) parmVarDecls.begin()))->getReference() = a;

	CHECK(isOccupied(varDecls),  0, ON_PRECONDITION);
	CHECK(varDecls.size() == 24, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();

	int k = 0;
	CHECK((*it)->Parameter->getValue() == k,   ++line, code);

	int i = 0;
	CHECK((*++it)->Parameter->getValue() == i, ++line, code);

	k++/* + */;
	i--;
	CHECK((*++it)->Parameter->getValue() == k, ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,   line, code);

	--k/* - */;
	++i;
	CHECK((*++it)->Parameter->getValue() == k, ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,   line, code);

	k--/* - */;
	i++;
	CHECK((*++it)->Parameter->getValue() == k, ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,   line, code);

	++k/* + */;
	--i;
	CHECK((*++it)->Parameter->getValue() == k, ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,   line, code);

	int j = k++ + ++i;
	CHECK((*++it)->Parameter->getValue() == k, ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,   line, code);
	CHECK((*++it)->Parameter->getValue() == j,   line, code);

	j = --k + i--;
	CHECK((*++it)->Parameter->getValue() == k, ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,   line, code);
	CHECK((*++it)->Parameter->getValue() == j,   line, code);

	j = --k + ++i;
	CHECK((*++it)->Parameter->getValue() == k, ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,   line, code);
	CHECK((*++it)->Parameter->getValue() == j,   line, code);

	k = a;
	CHECK((*++it)->Parameter->getValue() == k, ++line, code);

	i = k++ + 10;
	CHECK((*++it)->Parameter->getValue() == i, ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,   line, code);

	i = ++k + 10;
	CHECK((*++it)->Parameter->getValue() == i, ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,   line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DeAndIncrementTest::testFunctionParm() {
	const std::string code = functionParmTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();
	std::list<ParmDeclInfo*> parmVarDecls = function.getParmVarDecls();

	CHECK(isOccupied(varDecls),                     0, ON_PRECONDITION);
	CHECK(varDecls.size() == 6,                     0, ON_PRECONDITION);
	CHECK(parmVarDecls.size() == 9,                 0, ON_PRECONDITION);
	CHECK(function.getParmArrayDecls().size() == 0, 0, ON_PRECONDITION);
	CHECK(function.getParmDecls().size() == 1,      0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();
	ParmDeclInfoIterator parmDeclInfoIt = parmVarDecls.begin();

	int a = 42;
	parmVarDecls.front()->getReference() = a;
	CHECK((*parmDeclInfoIt)->getReference() == a,        0, ON_PRECONDITION);
	CHECK((*parmDeclInfoIt)->Parameter->getValue() == a, 0, ON_PRECONDITION);

	a++;
	CHECK((*++parmDeclInfoIt)->Parameter->getValue() == a, ++line, code);

	++a;
	CHECK((*++parmDeclInfoIt)->Parameter->getValue() == a, ++line, code);

	a--;
	CHECK((*++parmDeclInfoIt)->Parameter->getValue() == a, ++line, code);

	--a;
	CHECK((*++parmDeclInfoIt)->Parameter->getValue() == a, ++line, code);

	int k = a++;
	CHECK((*++parmDeclInfoIt)->Parameter->getValue() == a, ++line, code);
	CHECK((*it)->Parameter->getValue() == k,                 line, code);

	k += a;
	CHECK((*++it)->Parameter->getValue() == k,             ++line, code);

	k += ++a;
	CHECK((*++parmDeclInfoIt)->Parameter->getValue() == a, ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,               line, code);

	k += ++a + 1;
	CHECK((*++parmDeclInfoIt)->Parameter->getValue() == a, ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,               line, code);

	int i = ++k + a--;
	CHECK((*++parmDeclInfoIt)->Parameter->getValue() == a, ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,               line, code);
	CHECK((*++it)->Parameter->getValue() == k,               line, code);

	CHECK(++it == varDecls.end(),                 0, ON_POSTCONDITION);
	CHECK(++parmDeclInfoIt == parmVarDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DeAndIncrementTest::testPostDec() {
	const std::string code = postDecTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();

	CHECK(isOccupied(varDecls),  0, ON_PRECONDITION);
	CHECK(varDecls.size() == 23, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();

	int k = 0;
	CHECK((*it)->Parameter->getValue() == k,    ++line, code);

	int i = 0;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);

	k--;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k--/* + 1*/;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	/*1 + */k--;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k--/* + */;
	i--;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	i--/* + */;
	k--;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	i = k--;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	i = k-- + 1;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	i = 1 + k--;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	int i2 = k-- + i--;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);
	CHECK((*++it)->Parameter->getValue() == i2,   line, code);
	i = i2;

	i = i2-- + k--;
	CHECK((*++it)->Parameter->getValue() == i2, ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	int j = i-- + 10;
	CHECK((*++it)->Parameter->getValue() == j,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DeAndIncrementTest::testPostInc() {
	const std::string code = postIncTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();

	CHECK(isOccupied(varDecls),  0, ON_PRECONDITION);
	CHECK(varDecls.size() == 23, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();

	int k = 0;
	CHECK((*it)->Parameter->getValue() == k,    ++line, code);

	int i = 0;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);

	k++;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k++/* + 1*/;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	/*1 + */k++;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	k++/* + */;
	i++;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	i++/* + */;
	k++;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	i = k++;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	i = k++ + 1;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	i = 1 + k++;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	int i2 = k++ + i++;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);
	CHECK((*++it)->Parameter->getValue() == i2,   line, code);
	i = i2;

	i = i2++ + k++;
	CHECK((*++it)->Parameter->getValue() == i2, ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	int j = i++ + 10;
	CHECK((*++it)->Parameter->getValue() == j,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DeAndIncrementTest::testPreDec() {
	const std::string code = preDecTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();

	CHECK(isOccupied(varDecls),  0, ON_PRECONDITION);
	CHECK(varDecls.size() == 23, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();

	int k = 0;
	CHECK((*it)->Parameter->getValue() == k,    ++line, code);

	int i = 0;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);

	--k;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	--k/* + 1*/;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	/*1 + */--k;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	--k/* + */;
	--i;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	--i/* + */;
	--k;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	i = --k;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	i = --k + 1;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	i = 1 + --k;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	int i2 = --k + --i;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);
	CHECK((*++it)->Parameter->getValue() == i2,   line, code);
	i = i2;

	i = --i2 + --k;
	CHECK((*++it)->Parameter->getValue() == i2, ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	int j = --i + 10;
	CHECK((*++it)->Parameter->getValue() == j,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DeAndIncrementTest::testPreInc() {
	const std::string code = preIncTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();

	CHECK(isOccupied(varDecls),  0, ON_PRECONDITION);
	CHECK(varDecls.size() == 23, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();

	int k = 0;
	CHECK((*it)->Parameter->getValue() == k,    ++line, code);

	int i = 0;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);

	++k;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	++k/* + 1*/;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	/*1 + */++k;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);

	++k/* + */;
	++i;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	++i/* + */;
	++k;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	i = ++k;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	i = ++k + 1;
	CHECK((*++it)->Parameter->getValue() == i,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);

	i = 1 + ++k;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	int i2 = ++k + ++i;
	CHECK((*++it)->Parameter->getValue() == k,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);
	CHECK((*++it)->Parameter->getValue() == i2,   line, code);
	i = i2;

	i = ++i2 + ++k;
	CHECK((*++it)->Parameter->getValue() == i2, ++line, code);
	CHECK((*++it)->Parameter->getValue() == k,    line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	int j = ++i + 10;
	CHECK((*++it)->Parameter->getValue() == j,  ++line, code);
	CHECK((*++it)->Parameter->getValue() == i,    line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}
