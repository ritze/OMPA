/*
 * DependenceTest.cpp
 *
 *  Created on: 24.01.2014
 *      Author: Moritz Lüdecke
 */

#include <vector>

#include <llvm/ADT/StringRef.h>

#include <src/Analyse.h>
#include <src/Array.h>
#include <src/Context.h>

#include "DependenceTest.h"

static std::string arrayTestKernel =
		"__kernel void test() {        \n"
		"	int arrayA[] = {1, 1, 1};  \n"
		"	int arrayB[3];             \n"
		"	int k = arrayA[2];         \n"
		"	arrayB[1] = k;             \n"
		"	arrayA[0] = arrayB[1];     \n"
		"	k = arrayB[1];             \n"
		"}";

static std::string decAndIncTestKernel =
		"__kernel void test(int a) {  \n"
		"	a++;                      \n"
		"	++a;                      \n"
		"	a--;                      \n"
		"	--a;                      \n"
		"	int b = a++;              \n"
		"	b = --a + 1;              \n"
		"}";

static std::string functionCallTestKernel =
		"__kernel void test() {              \n"
		"	int a = 1;                       \n"
		"	int b = get_global_id(a);        \n"
		"	int c = get_global_id(++b);      \n"
		"	c = get_global_id(a + b);        \n"
		"	c = get_global_id(a + --b);      \n"
		"	c = get_global_id(a + b) + a;    \n"
		"	c = get_global_id(a + 1);        \n"
		"	b = get_global_id(1 + ++a);      \n"
		"	c = get_global_id(1 + ++b) + 1;  \n"
		"}";

static std::string functionCallParmTestKernel =
        "__kernel void test(int a) {         \n"
		"	int b = get_global_id(a);        \n"
		"	int c = get_global_id(++b);      \n"
		"	c = get_global_id(a + b);        \n"
		"	c = get_global_id(a + --b);      \n"
		"	c = get_global_id(a + b) + a;    \n"
		"	c = get_global_id(a + 1);        \n"
		"	b = get_global_id(1 + ++a);      \n"
		"	c = get_global_id(1 + ++b) + 1;  \n"
		"}";

static std::string functionParmTestKernel =
        "__kernel void test(int a, int b) {  \n"
		"	int c = a;                       \n"
		"	a = b;                           \n"
		"	b = c;                           \n"
		"	c = a + b;                       \n"
		"	a = c + b * 3;                   \n"
		"}";

static std::string multipleDeclTestKernel =
		"__kernel void test() {   \n"
		"	int a, b = 1;         \n"
		"	a++;                  \n"
		"	int c, d, e = 1 + 3;  \n"
		"	c = a;                \n"
		"	b = d++;              \n"
		"}";

static std::string simpleTestKernel =
		"__kernel void test() {   \n"
		"	int a;                \n"
		"	a = 0;                \n"
		"	int b = a;            \n"
		"	a = b;                \n"
		"	b = 0;                \n"
		"	int c = a + b;        \n"
		"	int d = (a + b) * c;  \n"
		"}";

bool DependenceTest::run() {
	bool result = true;

	// TODO: Implement depedeces in KernelFunction and complete testArray
//	result &= test("  testArray:               ",
//			(testFunction) &DependenceTest::testArray);
	result &= test("  testDecAndInc:           ",
	               (testFunction) &DependenceTest::testDecAndInc);
	result &= test("  testFunctionCall:        ",
	               (testFunction) &DependenceTest::testFunctionCall);
	result &= test("  testFunctionParmCall:    ",
	               (testFunction) &DependenceTest::testFunctionCallParm);
	result &= test("  testFunctionParm:        ",
	               (testFunction) &DependenceTest::testFunctionParm);
	result &= test("  testMultipleDecl:        ",
	               (testFunction) &DependenceTest::testMultipleDecl);
	result &= test("  testSimple:              ",
	               (testFunction) &DependenceTest::testSimple);

	return result;
}

// TODO: Implement depedeces in KernelFunction and complete testArray
bool DependenceTest::testArray() {
	const std::string code = arrayTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();
	std::list<ArrayInfo*> arrayDecls = function.getArrayDecls();
	std::list<ParmDeclInfo*> parmVarDecls = function.getParmVarDecls();
	std::list<const clang::IdentifierInfo*> parmDecls = function.getParmDecls();
	std::list<ParmArrayInfo*> parmArrayDecls = function.getParmArrayDecls();

	CHECK(isOccupied(varDecls),       0, ON_PRECONDITION);
	CHECK(varDecls.size() == 2,       0, ON_PRECONDITION);
	CHECK(isOccupied(arrayDecls),     0, ON_PRECONDITION);
	CHECK(arrayDecls.size() == 2,     0, ON_PRECONDITION);
	CHECK(parmVarDecls.size() == 0,   0, ON_PRECONDITION);
	CHECK(parmDecls.size() == 0,      0, ON_PRECONDITION);
	CHECK(parmArrayDecls.size() == 0, 0, ON_PRECONDITION);

	uint line = 1;
	uint idxA = 0;
	uint idxB = 0;
	DeclInfoIterator it = varDecls.begin();
	ArrayInfoIterator jt = arrayDecls.begin();

	int arrayA[] = { 1, 1, 1 };
	ArrayInfo* checkArrayA = (*jt);
	clang::StringRef arrayAName = checkArrayA->ID->getName();
	CHECK(checkArrayA->Dependences.empty(),                       ++line, code);

	int arrayB[3];
	ArrayInfo* checkArrayB = (*++jt);
	clang::StringRef arrayBName = checkArrayB->ID->getName();
	CHECK(checkArrayB->Dependences.size() == 1,                   ++line, code);

	int k = arrayA[2];
	CHECK((*it)->Dependences.size() == 1,                         ++line, code);
	CHECK((*it)->Dependences.front()->getName().equals(arrayAName), line, code);

	arrayB[1] = k;
	CHECK((*it)->Dependences.size() == 1, ++line, code);
	CHECK((*it)->Dependences.front()->getName().equals(arrayAName), line, code);

	arrayA[0] = arrayB[1];

	k = arrayB[1];

	CHECK(++it == varDecls.end(),   0, ON_POSTCONDITION);
	CHECK(++jt == arrayDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DependenceTest::testDecAndInc() {
	const std::string code = decAndIncTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();
	std::list<ParmDeclInfo*> parmVarDecls = function.getParmVarDecls();
	std::list<const clang::IdentifierInfo*> parmDecls = function.getParmDecls();
	std::list<ParmArrayInfo*> parmArrayDecls = function.getParmArrayDecls();

	CHECK(isOccupied(varDecls),       0, ON_PRECONDITION);
	CHECK(varDecls.size() == 2,       0, ON_PRECONDITION);
	CHECK(parmVarDecls.size() == 7,   0, ON_PRECONDITION);
	CHECK(parmDecls.size() == 1,      0, ON_PRECONDITION);
	CHECK(parmArrayDecls.size() == 0, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();

	int a = 42;
	ParmDeclInfoIterator parmVarDeclsIt = parmVarDecls.begin();
	(*parmVarDeclsIt)->getReference() = a;
	clang::StringRef aName = (*parmVarDeclsIt)->ID->getName();
	CHECK(parmDecls.front()->getName().equals(aName),          line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),          line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,             line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_Write),         line, code);

	a++;
	CHECK((*++parmVarDeclsIt)->Parameter->getValue() == a,   ++line, code);
	CHECK((*parmVarDeclsIt)->Dependences.empty(),              line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),          line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,             line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Write,            line, code);

	++a;
	CHECK((*++parmVarDeclsIt)->Parameter->getValue() == a,   ++line, code);
	CHECK((*parmVarDeclsIt)->Dependences.empty(),              line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),          line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,             line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Write,            line, code);

	a--;
	CHECK((*++parmVarDeclsIt)->Parameter->getValue() == a,   ++line, code);
	CHECK((*parmVarDeclsIt)->Dependences.empty(),              line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),          line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,             line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Write,            line, code);

	--a;
	CHECK((*++parmVarDeclsIt)->Parameter->getValue() == a,   ++line, code);
	CHECK((*parmVarDeclsIt)->Dependences.empty(),              line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),          line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,             line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Write,            line, code);

	int b = a++;
	CHECK((*++parmVarDeclsIt)->Parameter->getValue() == a,   ++line, code);
	CHECK((*parmVarDeclsIt)->Dependences.empty(),              line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(aName), line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),          line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,             line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Write,            line, code);

	b = --a + 1;
	CHECK((*++parmVarDeclsIt)->Parameter->getValue() == a,   ++line, code);
	CHECK((*parmVarDeclsIt)->Dependences.empty(),              line, code);
	CHECK((*++it)->Dependences.size() == 1,                    line, code);
	CHECK((*it)->Dependences.front()->getName().equals(aName), line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),          line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_Read),          line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Write,            line, code);

	CHECK(++it == varDecls.end(),                 0, ON_POSTCONDITION);
	CHECK(++parmVarDeclsIt == parmVarDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DependenceTest::testFunctionCall() {
	const std::string code = functionCallTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();
	std::list<ParmDeclInfo*> parmVarDecls = function.getParmVarDecls();
	std::list<const clang::IdentifierInfo*> parmDecls = function.getParmDecls();
	std::list<ParmArrayInfo*> parmArrayDecls = function.getParmArrayDecls();

	CHECK(isOccupied(varDecls),       0, ON_PRECONDITION);
	CHECK(varDecls.size() == 13,      0, ON_PRECONDITION);
	CHECK(parmVarDecls.size() == 0,   0, ON_PRECONDITION);
	CHECK(parmDecls.size() == 0,      0, ON_PRECONDITION);
	CHECK(parmArrayDecls.size() == 0, 0, ON_PRECONDITION);

	uint i = 0;
	uint line = 1;
	DeclInfoIterator it = varDecls.begin();
	std::list<const clang::IdentifierInfo*>::const_iterator dependencesIt;

	int a = 1;
	clang::StringRef aName = (*it)->ID->getName();
	CHECK((*it)->Parameter->getValue() == a,                 ++line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);

	/* int b = get_global_id(a); */
	int b = 0;
	clang::StringRef bName = (*++it)->ID->getName();
	CHECK((*it)->Parameter->getValue() == b,                 ++line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(aName), line, code);

	/* int c = get_global_id(++b); */
	int c = 0;
	++b;
	clang::StringRef cName = (*++it)->ID->getName();
	CHECK((*it)->Parameter->getValue() == c,                 ++line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(bName), line, code);
	CHECK((*++it)->ID->getName().equals(bName),                line, code);
	CHECK((*it)->Parameter->getValue() == b,                   line, code);
	CHECK((*it)->Dependences.size() == 0,                      line, code);

	/* c = get_global_id(a + b); */
	c = 0;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 2,                      line, code);
	dependencesIt = (*it)->Dependences.begin();
	CHECK((*dependencesIt)->getName().equals(aName),           line, code);
	CHECK((*++dependencesIt)->getName().equals(bName),         line, code);

	/* c = get_global_id(a + --b);*/
	c = 0;
	--b;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 2,                      line, code);
	dependencesIt = (*it)->Dependences.begin();
	CHECK((*dependencesIt)->getName().equals(aName),           line, code);
	CHECK((*++dependencesIt)->getName().equals(bName),         line, code);
	CHECK((*++it)->ID->getName().equals(bName),                line, code);
	CHECK((*it)->Parameter->getValue() == b,                   line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);

	/* c = get_global_id(a + b) + a;*/
	c = 0 + a;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 3,                      line, code);
	dependencesIt = (*it)->Dependences.begin();
	CHECK((*dependencesIt)->getName().equals(aName),           line, code);
	CHECK((*++dependencesIt)->getName().equals(bName),         line, code);
	CHECK((*++dependencesIt)->getName().equals(aName),         line, code);

	/* c = get_global_id(a + 1); */
	c = 0;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(aName), line, code);

	/* b = get_global_id(1 + ++a); */
	b = 0;
	++a;
	CHECK((*++it)->ID->getName().equals(bName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == b,                   line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(aName), line, code);
	CHECK((*++it)->ID->getName().equals(aName),                line, code);
	CHECK((*it)->Parameter->getValue() == a,                   line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);

	/* c = get_global_id(1 + ++b) + 1; */
	c = 0 + 1;
	++b;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(bName), line, code);
	CHECK((*++it)->ID->getName().equals(bName),                line, code);
	CHECK((*it)->Parameter->getValue() == b,                   line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DependenceTest::testFunctionCallParm() {
	const std::string code = functionCallParmTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();
	std::list<ParmDeclInfo*> parmVarDecls = function.getParmVarDecls();
	std::list<const clang::IdentifierInfo*> parmDecls = function.getParmDecls();
	std::list<ParmArrayInfo*> parmArrayDecls = function.getParmArrayDecls();

	CHECK(isOccupied(varDecls),       0, ON_PRECONDITION);
	CHECK(varDecls.size() == 11,      0, ON_PRECONDITION);
	CHECK(parmVarDecls.size() == 2,   0, ON_PRECONDITION);
	CHECK(parmDecls.size() == 1,      0, ON_PRECONDITION);
	CHECK(parmArrayDecls.size() == 0, 0, ON_PRECONDITION);

	int i = 0;
	uint line = 1;
	DeclInfoIterator it = varDecls.begin();
	std::list<const clang::IdentifierInfo*>::const_iterator dependencesIt;

	int a = 21;
	ParmDeclInfoIterator parmVarDeclsIt = parmVarDecls.begin();
	(*parmVarDeclsIt)->getReference() = a;
	clang::StringRef aName = (*parmVarDeclsIt)->ID->getName();
	CHECK(parmDecls.front()->getName().equals(aName),          line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),          line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,             line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_Write),         line, code);

	/* int b = get_global_id(a); */
	int b = 0;
	clang::StringRef bName = (*it)->ID->getName();
	CHECK((*it)->Parameter->getValue() == b,                 ++line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(aName), line, code);

	/* int c = get_global_id(++b); */
	int c = 0;
	++b;
	clang::StringRef cName = (*++it)->ID->getName();
	CHECK((*it)->Parameter->getValue() == c,                 ++line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(bName), line, code);
	CHECK((*++it)->ID->getName().equals(bName),                line, code);
	CHECK((*it)->Parameter->getValue() == b,                   line, code);
	CHECK((*it)->Dependences.size() == 0,                      line, code);

	/* c = get_global_id(a + b); */
	c = 0;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 2,                      line, code);
	dependencesIt = (*it)->Dependences.begin();
	CHECK((*dependencesIt)->getName().equals(aName),           line, code);
	CHECK((*++dependencesIt)->getName().equals(bName),         line, code);

	/* c = get_global_id(a + --b);*/
	c = 0;
	--b;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 2,                      line, code);
	dependencesIt = (*it)->Dependences.begin();
	CHECK((*dependencesIt)->getName().equals(aName),           line, code);
	CHECK((*++dependencesIt)->getName().equals(bName),         line, code);
	CHECK((*++it)->ID->getName().equals(bName),                line, code);
	CHECK((*it)->Parameter->getValue() == b,                   line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);

	/* c = get_global_id(a + b) + a;*/
	c = 0 + a;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 3,                      line, code);
	dependencesIt = (*it)->Dependences.begin();
	CHECK((*dependencesIt)->getName().equals(aName),           line, code);
	CHECK((*++dependencesIt)->getName().equals(bName),         line, code);
	CHECK((*++dependencesIt)->getName().equals(aName),         line, code);

	/* c = get_global_id(a + 1); */
	c = 0;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(aName), line, code);

	/* b = get_global_id(1 + ++a); */
	b = 0;
	++a;
	CHECK((*++it)->ID->getName().equals(bName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == b,                   line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(aName), line, code);
	CHECK((*++parmVarDeclsIt)->ID->getName().equals(aName),    line, code);
	CHECK((*parmVarDeclsIt)->Parameter->getValue() == a,       line, code);
	CHECK((*parmVarDeclsIt)->Dependences.empty(),              line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),          line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_Read),          line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Write,            line, code);

	/* c = get_global_id(1 + ++b) + 1; */
	c = 0 + 1;
	++b;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(bName), line, code);
	CHECK((*++it)->ID->getName().equals(bName),                line, code);
	CHECK((*it)->Parameter->getValue() == b,                   line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DependenceTest::testFunctionParm() {
	const std::string code = functionParmTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();
	std::list<ParmDeclInfo*> parmVarDecls = function.getParmVarDecls();
	std::list<const clang::IdentifierInfo*> parmDecls = function.getParmDecls();
	std::list<ParmArrayInfo*> parmArrayDecls = function.getParmArrayDecls();

	CHECK(isOccupied(varDecls),       0, ON_PRECONDITION);
	CHECK(varDecls.size() == 2,       0, ON_PRECONDITION);
	CHECK(parmVarDecls.size() == 5,   0, ON_PRECONDITION);
	CHECK(parmDecls.size() == 2,      0, ON_PRECONDITION);
	CHECK(parmArrayDecls.size() == 0, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();
	std::list<const clang::IdentifierInfo*> dependences;
	std::list<const clang::IdentifierInfo*>::const_iterator dependencesIt;

	int a = 2;
	int b = 5;
	ParmDeclInfoIterator parmVarDeclsIt = parmVarDecls.begin();
	(*parmVarDeclsIt)->getReference() = a;
	clang::StringRef aName = (*parmVarDeclsIt)->ID->getName();
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),      line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,         line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_Write),     line, code);
	++parmVarDeclsIt;
	(*++parmVarDeclsIt)->getReference() = b;
	clang::StringRef bName = (*parmVarDeclsIt)->ID->getName();
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),      line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,         line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_Write),     line, code);
	--parmVarDeclsIt;

	int c = a;
	clang::StringRef cName = (*it)->ID->getName();
	dependences = (*it)->Dependences;
	CHECK((*it)->Parameter->getValue() == c,             ++line, code);
	CHECK(dependences.size() == 1,                         line, code);
	CHECK(dependences.front()->getName().equals(aName),    line, code);

	a = b;
	dependences = (*parmVarDeclsIt)->Dependences;
	CHECK((*parmVarDeclsIt)->Parameter->getValue() == a, ++line, code);
	CHECK(dependences.size() == 1,                         line, code);
	CHECK((*parmVarDeclsIt)->ID->getName().equals(aName),  line, code);
	CHECK(dependences.front()->getName().equals(bName),    line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),      line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,         line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Write,        line, code);
	++parmVarDeclsIt;

	b = c;
	dependences = (*++parmVarDeclsIt)->Dependences;
	CHECK((*parmVarDeclsIt)->Parameter->getValue() == b, ++line, code);
	CHECK(dependences.size() == 1,                         line, code);
	CHECK((*parmVarDeclsIt)->ID->getName().equals(bName),  line, code);
	CHECK(dependences.front()->getName().equals(cName),    line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),      line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Read,         line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Write,        line, code);

	c = a + b;
	dependences = (*++it)->Dependences;
	dependencesIt = dependences.begin();
	CHECK((*it)->Parameter->getValue() == c,             ++line, code);
	CHECK((*it)->Dependences.size() == 2,                  line, code);
	CHECK((*dependencesIt)->getName().equals(aName),       line, code);
	CHECK((*++dependencesIt)->getName().equals(bName),     line, code);

	a = c + b * 3;
	dependences = (*++parmVarDeclsIt)->Dependences;
	CHECK((*parmVarDeclsIt)->Parameter->getValue() == a, ++line, code);
	CHECK(dependences.size() == 2, line, code);
	CHECK((*parmVarDeclsIt)->ID->getName().equals(aName),  line, code);
	CHECK(dependences.front()->getName().equals(cName),    line, code);
	CHECK(dependences.back()->getName().equals(bName),     line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_None),      line, code);
	CHECK(!((*parmVarDeclsIt)->accessMode & AF_Read),      line, code);
	CHECK((*parmVarDeclsIt)->accessMode & AF_Write,        line, code);

	CHECK(++it == varDecls.end(),                 0, ON_POSTCONDITION);
	CHECK(++parmVarDeclsIt == parmVarDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DependenceTest::testMultipleDecl() {
	const std::string code = multipleDeclTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();

	CHECK(isOccupied(varDecls), 0, ON_PRECONDITION);
	CHECK(varDecls.size() == 9, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();

	int a, b = 1;
	clang::StringRef aName = (*it)->ID->getName();
	CHECK((*it)->Parameter->getValue() == a,                 ++line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);
	clang::StringRef bName = (*++it)->ID->getName();
	CHECK((*it)->Parameter->getValue() == b,                   line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);

	a++;
	CHECK((*++it)->ID->getName().equals(aName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == a,                   line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);

	int c, d, e = 1 + 3;
	clang::StringRef cName = (*++it)->ID->getName();
	CHECK((*it)->Parameter->getValue() == c,                 ++line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);
	clang::StringRef dName = (*++it)->ID->getName();
	CHECK((*it)->Parameter->getValue() == d,                   line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);
	clang::StringRef eName = (*++it)->ID->getName();
	CHECK((*it)->Parameter->getValue() == e,                   line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);

	c = a;
	CHECK((*++it)->ID->getName().equals(cName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == c,                   line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(aName), line, code);

	b = d++;
	CHECK((*++it)->ID->getName().equals(dName),              ++line, code);
	CHECK((*it)->Parameter->getValue() == d,                   line, code);
	CHECK((*it)->Dependences.empty(),                          line, code);
	CHECK((*++it)->ID->getName().equals(bName),                line, code);
	CHECK((*it)->Parameter->getValue() == b,                   line, code);
	CHECK((*it)->Dependences.size() == 1,                      line, code);
	CHECK((*it)->Dependences.front()->getName().equals(dName), line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}

bool DependenceTest::testSimple() {
	const std::string code = simpleTestKernel;

	Analyse analyse(code);

	KernelFunction function = analyse.getKernelInfo()->getFunctions().front();
	std::list<DeclInfo*> varDecls = function.getVarDecls();

	CHECK(isOccupied(varDecls), 0, ON_PRECONDITION);
	CHECK(varDecls.size() == 7, 0, ON_PRECONDITION);

	uint line = 1;
	DeclInfoIterator it = varDecls.begin();
	std::list<const clang::IdentifierInfo*>::const_iterator dependencesIt;

	/* int a; */
	clang::StringRef a = (*it)->ID->getName();
	CHECK((*it)->Dependences.empty(),                      ++line, code);

	/* a = 0; */
	CHECK((*++it)->Dependences.empty(),                    ++line, code);

	/* int b = a; */
	clang::StringRef b = (*++it)->ID->getName();
	CHECK((*it)->Dependences.size() == 1,                  ++line, code);
	CHECK((*it)->Dependences.front()->getName().equals(a),   line, code);

	/* a = b; */
	CHECK((*++it)->Dependences.size() == 1,                ++line, code);
	CHECK((*it)->Dependences.front()->getName().equals(b),   line, code);

	/* b = 0; */
	CHECK((*++it)->Dependences.empty(),                    ++line, code);

	/* int c = a + b; */
	clang::StringRef c = (*++it)->ID->getName();
	dependencesIt = (*it)->Dependences.begin();
	CHECK((*it)->Dependences.size() == 2,                  ++line, code);
	CHECK((*dependencesIt)->getName().equals(a),             line, code);
	CHECK((*++dependencesIt)->getName().equals(b),           line, code);

	/* int d = (a + b) * c; */
	dependencesIt = (*++it)->Dependences.begin();
	CHECK((*it)->Dependences.size() == 3,                  ++line, code);
	CHECK((*dependencesIt)->getName().equals(a),             line, code);
	CHECK((*++dependencesIt)->getName().equals(b),           line, code);
	CHECK((*++dependencesIt)->getName().equals(c),           line, code);

	CHECK(++it == varDecls.end(), 0, ON_POSTCONDITION);

	return true;
}
