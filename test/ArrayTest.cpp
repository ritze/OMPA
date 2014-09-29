/*
 * ArrayTest.cpp
 *
 *  Created on: 23.04.2014
 *      Author: Moritz Lüdecke
 */

#include <src/Analyse.h>
#include <src/Array.h>

#include "ArrayTest.h"

static std::string calcTestKernel =
		"__kernel void test() {                 \n"
		"	int array[] = {1, 2, 3, 4};         \n"
		"	array[1] += 52;                     \n"
		"	array[2] -= 52;                     \n"
		"	array[3] %= 10 - 2;                 \n"
		"	array[3] *= array[1] + 1;           \n"
		"	int k = array[0]++;                 \n"
		"	k = ++array[1];                     \n"
		"	k = array[2]--;                     \n"
		"	k = --array[2];                     \n"
		"	array[3] = --array[2];              \n"
		"	array[1] += array[2]--;             \n"
		"	array[0] *= ++array[1];             \n"
		"	array[0] = 0;                       \n"
		"	array[1] = array[0];                \n"
		"	array[2] = array[1];                \n"
		"	array[3] = array[2];                \n"
//		"	array[array[0] + 1] = 2;            \n"
//		"	array[array[++array[0]] + 1] = 42;  \n"
		"	k = array[0];                       \n"
		"	k = array[1];                       \n"
		"	k = array[2];                       \n"
		"	k = array[3];                       \n"
		"	array[2] = 3;                       \n"
// TODO:
//		"	k = array[array[array[array[0]]]];  \n"
		"	k = 0;                              \n"
//		"	k = array[array[array[array[k]]]];  \n"
		"	k = 0;                              \n"
//		"	++array[++k];                       \n"
		"	k = array[1];                       \n"
//		"	array[2] = array[2]++;              \n"
//		"	array[2] = ++array[2];              \n"
		"}";

static std::string calcIndexTestKernel =
		"__kernel void test() {          \n"
		"	int array[] = {1, 2, 3, 4};  \n"
		"	array[1 + 1] = 52;           \n"
		"	array[0 - 1 + 2] = 34;       \n"
		"	array[4 / 2] = -323;         \n"
		"	int i = 0;                   \n"
		"	array[i] = 123;              \n"
		"	array[i++] = 23;             \n"
		"	array[++i] = 17;             \n"
		"	array[--i] = 56;             \n"
		"	array[i--] = -9;             \n"
		"	array[i + 1] = 5665;         \n"
		"	int k = array[1 + 3 -2];     \n"
		"	k = array[i];                \n"
		"	k = array[++i];              \n"
		"	k = array[i++];              \n"
		"	k = array[--i];              \n"
		"	k = array[i--];              \n"
		"	k = array[i + i + 2];        \n"
		"	k = array[++i / 2 + 1];      \n"
		"}";

static std::string initArrayTestKernel =
		"__kernel void test() {                  \n"
		"	int array1[1];                       \n"
		"	int array2[2];                       \n"
		"	int array3[3];                       \n"
		"	int array4[4];                       \n"
		"	int array5[5];                       \n"
		"	int array6[6];                       \n"
		"	int array7[7];                       \n"
		"	int array8[8];                       \n"
		"	int array16[16];                     \n"
		"	int array32[32];                     \n"
		"	int array64[64];                     \n"
		"	int array1024[1024];                 \n"
		"	int a[] = {1};                       \n"
		"	int b[] = {34, -127495};             \n"
		"	int c[] = {1, -2, 3, -4};            \n"
		"	int d[] = {1025, 5, 18, 23};         \n"
		"	int e[] = {3, 7, 23, 789, 1, -4,     \n"
		"			   34, -56765, 1, 5656,      \n"
		"			   123123, -67768, 345,      \n"
		"			   123, 980, 345, 21, -123,  \n"
		"			   -234, 234, -7678, 23,     \n"
		"			   5667, 3214, -56, 345};    \n"
		"	int k = 42;                          \n"
		"	int f[] = {k, k};                    \n"
// TODO:
//		"	int g[] = {f[0], k};                 \n"
//		"	int h[] = {k, g[0], k};              \n"
		"}";

static std::string simpleArrayTestKernel =
		"__kernel void test() {        \n"
		"	int array[] = {1, -2, 3};  \n"
		"	array[0] = -10;            \n"
		"	array[1] = 12 / 2 * -1;    \n"
		"	array[2] = 10 + 3;         \n"
		"	array[0] = 0;              \n"
		"	int value = 23;            \n"
		"	array[2] = value;          \n"
		"}";

bool ArrayTest::run() {
	bool result = true;

	result &= test("  testCalc:                ",
	               (testFunction) &ArrayTest::testCalc);
	result &= test("  testCalcIndex:           ",
	               (testFunction) &ArrayTest::testCalcIndex);
	result &= test("  testInitArray:           ",
	               (testFunction) &ArrayTest::testInitArray);
	result &= test("  testSimpleArray:         ",
	               (testFunction) &ArrayTest::testSimpleArray);

	return result;
}

bool ArrayTest::testCalc() {
	const std::string code = calcTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();
	std::list<ArrayInfo*> arrayDecls = function.getArrayDecls();

	CHECK(isOccupied(varDecls),   0, ON_PRECONDITION);
	CHECK(varDecls.size() == 11,  0, ON_PRECONDITION);
//	CHECK(varDecls.size() == 13,  0, ON_PRECONDITION);
	CHECK(isOccupied(arrayDecls), 0, ON_PRECONDITION);
	CHECK(arrayDecls.size() == 1, 0, ON_PRECONDITION);

	uint line = 2;
	ArrayElement* checkElement;
	ArrayInfoIterator it = arrayDecls.begin();
	DeclInfoIterator jt = varDecls.begin();
	Array* checkArray = (*it)->array;

	int array[] = { 1, 2, 3, 4 };
	uint idx = 0;
	while (idx < checkArray->getSize()->getValue()) {
		checkElement = checkArray->getArrayElement(idx);
		CHECK(checkElement->getIndex()->getValue() == idx,          line, code);
		CHECK(checkElement->getParameter()->getValue() == array[idx++],
		                                                            line, code);
	}

	array[1] += 52;
	checkElement = checkArray->getArrayElement(idx);
	CHECK(checkElement->getIndex()->getValue() == 1,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[1],     line, code);

	array[2] -= 52;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 2,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);

	array[3] %= 10 - 2;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 3,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[3],     line, code);

	array[3] *= array[1] + 1;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 3,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[3],     line, code);

	int k = array[0]++;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 0,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[0],     line, code);
	CHECK((*jt)->Parameter->getValue() == k,                        line, code);

	k = ++array[1];
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 1,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[1],     line, code);
	CHECK((*++jt)->Parameter->getValue() == k,                      line, code);

	k = array[2]--;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 2,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);
	CHECK((*++jt)->Parameter->getValue() == k,                      line, code);

	k = --array[2];
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 2,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);
	CHECK((*++jt)->Parameter->getValue() == k,                      line, code);

	array[3] = --array[2];
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 3,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[3],     line, code);
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 2,                line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);

	array[1] += array[2]--;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 1,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[1],     line, code);
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 2,                line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);

	array[0] *= ++array[1];
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 0,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[0],     line, code);
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 1,                line, code);
	CHECK(checkElement->getParameter()->getValue() == array[1],     line, code);

	array[0] = 0;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 0,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[0],     line, code);

	array[1] = array[0];
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 1,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[1],     line, code);

	array[2] = array[1];
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 2,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);

	array[3] = array[2];
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 3,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[3],     line, code);

//	array[array[0] + 1] = 2;
//	checkElement = checkArray->getArrayElement(++idx);
//	CHECK(checkElement->getIndex()->getValue() == array[0] + 1,   ++line, code);
//	CHECK(checkElement->getParameter()->getValue() == array[1],     line, code);
//
//	array[array[++array[0]] + 1] = 42;
//	checkElement = checkArray->getArrayElement(++idx);
//	CHECK(checkElement->getIndex()->getValue() == array[3],       ++line, code);
//	CHECK(checkElement->getParameter()->getValue() == 42,           line, code);
//	checkElement = checkArray->getArrayElement(++idx);
//	CHECK(checkElement->getIndex()->getValue() == 0,                line, code);
//	CHECK(checkElement->getParameter()->getValue() == array[0],     line, code);

	k = array[0];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

	k = array[1];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

	k = array[2];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

	k = array[3];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

	array[2] = 3;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 2,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);

//	k = array[array[array[array[0]]]];
//	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

	k = 0;
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

//	k = array[array[array[array[k]]]];
//	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

	k = 0;
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

//	++array[++k];
//	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);
//	checkElement = checkArray->getArrayElement(++idx);
//	CHECK(checkElement->getIndex()->getValue() == k,                line, code);
//	CHECK(checkElement->getParameter()->getValue() == array[k],     line, code);

	k = array[1];
	CHECK((*++jt)->Parameter->getValue() == k, ++line, code);

//	array[2] = array[2]++;
//	checkElement = checkArray->getArrayElement(++idx);
//	CHECK(checkElement->getIndex()->getValue() == 2,              ++line, code);
//	CHECK(checkElement->getParameter()->getValue() == array[2] + 1, line, code);
//	checkElement = checkArray->getArrayElement(++idx);
//	CHECK(checkElement->getIndex()->getValue() == 2,                line, code);
//	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);

//	array[2] = ++array[2];
//	checkElement = checkArray->getArrayElement(++idx);
//	CHECK(checkElement->getIndex()->getValue() == 2,              ++line, code);
//	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);
//	checkElement = checkArray->getArrayElement(++idx);
//	CHECK(checkElement->getIndex()->getValue() == 2,                line, code);
//	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);

	CHECK(++it == arrayDecls.end(), 0, ON_POSTCONDITION);
	CHECK(++jt == varDecls.end(),   0, ON_POSTCONDITION);

	return true;
}

bool ArrayTest::testCalcIndex() {
	const std::string code = calcIndexTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();
	std::list<ArrayInfo*> arrayDecls = function.getArrayDecls();

	CHECK(isOccupied(varDecls),   0, ON_PRECONDITION);
	CHECK(varDecls.size() == 18,  0, ON_PRECONDITION);
	CHECK(isOccupied(arrayDecls), 0, ON_PRECONDITION);
	CHECK(arrayDecls.size() == 1, 0, ON_PRECONDITION);

	uint line = 2;
	ArrayElement* checkElement;
	ArrayInfoIterator it = arrayDecls.begin();
	DeclInfoIterator jt = varDecls.begin();
	Array* checkArray = (*it)->array;

	int array[] = { 1, 2, 3, 4 };
	uint idx = 0;
	while (idx < checkArray->getSize()->getValue()) {
		checkElement = checkArray->getArrayElement(idx);
		CHECK(checkElement->getIndex()->getValue() == idx,          line, code);
		CHECK(checkElement->getParameter()->getValue() == array[idx++],
		                                                            line, code);
	}

	array[1 + 1] = 52;
	checkElement = checkArray->getArrayElement(idx);
	CHECK(checkElement->getIndex()->getValue() == 2,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);

	array[0 - 1 + 2] = 34;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 1,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[1],     line, code);

	array[4 / 2] = -323;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 2,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2],     line, code);

	int i = 0;
	CHECK((*jt)->Parameter->getValue() == i,                      ++line, code);

	array[i] = 123;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == i,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[i],     line, code);

	array[i++] = 23;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK((*++jt)->Parameter->getValue() == i,                    ++line, code);
	CHECK(checkElement->getIndex()->getValue() == i - 1,            line, code);
	CHECK(checkElement->getParameter()->getValue() == array[i - 1], line, code);

	array[++i] = 17;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == i,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[i],     line, code);
	CHECK((*++jt)->Parameter->getValue() == i,                      line, code);

	array[--i] = 56;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == i,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[i],     line, code);
	CHECK((*++jt)->Parameter->getValue() == i,                      line, code);

	array[i--] = -9;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == i + 1,          ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[i + 1], line, code);
	CHECK((*++jt)->Parameter->getValue() == i,                      line, code);

	array[i + 1] = 5665;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == i + 1,          ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[i + 1], line, code);

	int k = array[1 + 3 - 2];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

	k = array[i];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

	k = array[++i];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);
	CHECK((*++jt)->Parameter->getValue() == i,                      line, code);

	k = array[i++];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);
	CHECK((*++jt)->Parameter->getValue() == i,                      line, code);

	k = array[--i];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);
	CHECK((*++jt)->Parameter->getValue() == i,                      line, code);

	k = array[i--];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);
	CHECK((*++jt)->Parameter->getValue() == i,                      line, code);

	k = array[i + i + 2];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);

	k = array[++i / 2 + 1];
	CHECK((*++jt)->Parameter->getValue() == k,                    ++line, code);
	CHECK((*++jt)->Parameter->getValue() == i,                      line, code);

	CHECK(++it == arrayDecls.end(), 0, ON_POSTCONDITION);
	CHECK(++jt == varDecls.end(),   0, ON_POSTCONDITION);

	return true;
}

bool ArrayTest::testInitArray() {
	const std::string code = initArrayTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();
	std::list<ArrayInfo*> arrayDecls = function.getArrayDecls();

	CHECK(isOccupied(varDecls),    0, ON_PRECONDITION);
	CHECK(varDecls.size() == 1,    0, ON_PRECONDITION);
	CHECK(isOccupied(arrayDecls),  0, ON_PRECONDITION);
	CHECK(arrayDecls.size() == 18, 0, ON_PRECONDITION);
	//TODO: Implement int array[] = {array2[1], ...}
//	CHECK(arrayDecls.size() == 20, 0, ON_PRECONDITION);

	uint line = 1;
	Array* checkArray;
	ArrayElement* checkElement;
	ArrayInfoIterator it = arrayDecls.begin();
	DeclInfoIterator jt = varDecls.begin();

	// int array1[1]
	checkArray = (*it)->array;
	CHECK(checkArray->getSize()->getValue() == 1,                 ++line, code);
	checkElement = checkArray->getArrayElement(0);
	CHECK(checkElement->getIndex()->getValue() == 0,                line, code);
	CHECK(checkElement->getParameter()->getValue() == 0,            line, code);

	// int array2[2]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 2,                 ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int array3[3]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 3,                 ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int array4[4]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 4,                 ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int array5[5]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 5,                 ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int array6[6]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 6,                 ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int array7[7]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 7,                 ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int array8[8]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 8,                 ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int array16[16]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 16,                ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int array32[32]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 32,                ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int array64[64]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 64,                ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int array1024[1024]
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 1024,              ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == 0,        line, code);
	}

	// int a[] = {1};
	checkArray = (*++it)->array;
	checkElement = checkArray->getArrayElement(0);
	CHECK(checkElement->getIndex()->getValue() == 0,              ++line, code);
	CHECK(checkElement->getParameter()->getValue() == 1,            line, code);

	int b[] = { 34, -127495 };
	++line;
	checkArray = (*++it)->array;
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == b[i],     line, code);
	}

	int c[] = { 1, -2, 3, -4 };
	++line;
	checkArray = (*++it)->array;
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == c[i],     line, code);
	}

	int d[] = { 1025, 5, 18, 23 };
	++line;
	checkArray = (*++it)->array;
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == d[i],     line, code);
	}

	int e[] = { 3, 7, 23, 789, 1, -4, 34, -56765, 1, 5656, 123123, -67768, 345,
	            123, 980, 345, 21, -123, -234, 234, -7678, 23, 5667, 3214, -56,
	            345 };
	++line;
	checkArray = (*++it)->array;
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == e[i],     line, code);
	}
	line += 5;

	int k = 42;
	CHECK((*jt)->Parameter->getValue() == k,                      ++line, code);

	int f[] = { k, k };
	checkArray = (*++it)->array;
	CHECK(checkArray->getSize()->getValue() == 2,                 ++line, code);
	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
		checkElement = checkArray->getArrayElement(i);
		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
		CHECK(checkElement->getParameter()->getValue() == f[i],     line, code);
	}

	//TODO: Implement int array[] = {array2[1], ...}
//	int g[] = {f[0], k};
//	checkArray = (*++it)->array;
//	CHECK(checkArray->getSize()->getValue() == 2,                 ++line, code);
//	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
//		checkElement = checkArray->getArrayElement(i);
//		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
//		CHECK(checkElement->getParameter()->getValue() == g[i],     line, code);
//	}
//
//	int h[] = {k, g[0], k};
//	checkArray = (*++it)->array;
//	CHECK(checkArray->getSize()->getValue() == 3,                 ++line, code);
//	for (uint i = 0; i < checkArray->getSize()->getValue(); ++i) {
//		checkElement = checkArray->getArrayElement(i);
//		CHECK(checkElement->getIndex()->getValue() == i,            line, code);
//		CHECK(checkElement->getParameter()->getValue() == h[i],     line, code);
//	}

	CHECK(++it == arrayDecls.end(), 0, ON_POSTCONDITION);
	CHECK(++jt == varDecls.end(),   0, ON_POSTCONDITION);

	return true;
}

bool ArrayTest::testSimpleArray() {
	const std::string code = simpleArrayTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();
	std::list<ArrayInfo*> arrayDecls = function.getArrayDecls();

	CHECK(isOccupied(varDecls),   0, ON_PRECONDITION);
	CHECK(varDecls.size() == 1,   0, ON_PRECONDITION);
	CHECK(isOccupied(arrayDecls), 0, ON_PRECONDITION);
	CHECK(arrayDecls.size() == 1, 0, ON_PRECONDITION);

	uint line = 2;
	ArrayElement* checkElement;
	ArrayInfoIterator it = arrayDecls.begin();
	DeclInfoIterator jt = varDecls.begin();
	Array* checkArray = (*it)->array;

	int array[] = { 1, -2, 3 };
	uint idx = 0;
	while (idx < checkArray->getSize()->getValue()) {
		checkElement = checkArray->getArrayElement(idx);
		CHECK(checkElement->getIndex()->getValue() == idx,      line, code);
		CHECK(checkElement->getParameter()->getValue() == array[idx++],
		                                                        line, code);
	}

	array[0] = -10;
	checkElement = checkArray->getArrayElement(idx);
	CHECK(checkElement->getIndex()->getValue() == 0,          ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[0], line, code);

	array[1] = 12 / 2 * -1;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 1,          ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[1], line, code);

	array[2] = 10 + 3;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 2,          ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2], line, code);

	array[0] = 0;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 0,          ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[0], line, code);

	int value = 23;
	CHECK((*jt)->Parameter->getValue() == value,                line, code);

	array[2] = value;
	checkElement = checkArray->getArrayElement(++idx);
	CHECK(checkElement->getIndex()->getValue() == 2,          ++line, code);
	CHECK(checkElement->getParameter()->getValue() == array[2], line, code);

	CHECK(++it == arrayDecls.end(), 0, ON_POSTCONDITION);
	CHECK(++jt == varDecls.end(),   0, ON_POSTCONDITION);

	return true;
}
