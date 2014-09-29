/*
 * Test.h
 *
 *  Created on: 23.01.2014
 *      Author: Moritz Lüdecke
 */

#ifndef TEST_H_
#define TEST_H_

#include <string>
#include <list>

#include <clang/Basic/IdentifierTable.h>

#include <src/DeclInfo.h>

class Test {
#define ON_PRECONDITION "on preconditions"
#define ON_POSTCONDITION "on postconditions"

	// Only aborts the current method if expression is false
#define CHECK(expression, line, code)         \
	if (!diagnose(expression, line, code)) {  \
		return false;                         \
	}                                         \

public:
	bool run();

protected:
	typedef std::list<ParmDeclInfo*>::iterator ParmDeclInfoIterator;
	typedef std::list<DeclInfo*>::iterator DeclInfoIterator;
	typedef std::list<ArrayInfo*>::iterator ArrayInfoIterator;
	typedef std::list<const clang::IdentifierInfo*>::iterator IDIterator;
	typedef bool (Test::*testFunction)();

	std::string deleteLeadingSpace(const std::string text);
	bool diagnose(bool expression, uint line, const std::string code);
	std::string getLine(const std::string text, const uint lineNumber);
	bool isOccupied(std::list<DeclInfo*> varDecls);
	bool isOccupied(std::list<ArrayInfo*> arrayDecls);
	bool test(const std::string pretext, testFunction function);
	void printVarDecls(std::list<DeclInfo*> varDecls);
};

#endif /* TEST_H_ */
