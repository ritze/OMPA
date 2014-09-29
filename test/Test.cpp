/*
 * Test.cpp
 *
 *  Created on: 23.01.2014
 *      Author: Moritz Lüdecke
 */

#include <iostream>

#include "Test.h"

std::string Test::deleteLeadingSpace(const std::string text) {
	size_t begin = text.find_first_not_of(" \t");

	if (begin == std::string::npos) {
		return "";
	}

	return text.substr(begin);
}

// Print code if line = 0
bool Test::diagnose(bool expression, uint line, const std::string code) {
	if (!expression) {
		std::cout << "FAIL";

		if (line == 0) {
			std::cout << " " << code << '\n';

		} else {
			// getLine: lineNumber begins at 0
			std::string codeLine = getLine(code, line - 1);
			codeLine = deleteLeadingSpace(codeLine);
			std::cout << ", " << line << ": " << codeLine << "\n";
		}

		return false;
	}

	return true;
}

// lineNumber begins at 0
// Returns an empty string, if line is not found
std::string Test::getLine(const std::string text, const uint lineNumber) {
	size_t pos = 0;
	std::string found = "";

	for (uint i = 0; i < lineNumber && pos != std::string::npos; ++i) {
		pos = text.find('\n', pos + 1);
	}

	if (pos != std::string::npos) {
		if (pos > 0) {
			++pos;
		}

		found = text.substr(pos);
		pos = found.find('\n');

		if (pos != std::string::npos) {
			found = found.substr(0, pos);
		}
	}

	return found;
}

bool Test::isOccupied(std::list<DeclInfo*> varDecls) {
	for (DeclInfoIterator it = varDecls.begin(); it != varDecls.end(); ++it) {
		if (!(*it)->Parameter->isOccupied()) {
			return false;
		}
	}

	return true;
}

bool Test::isOccupied(std::list<ArrayInfo*> arrayDecls) {
	for (ArrayInfoIterator it = arrayDecls.begin(); it != arrayDecls.end();
	        ++it) {
		Array* array = (*it)->array;

		for (uint i = 0; i < array->getTimestamp(); ++i) {
			if (!array->getArrayElement(i)->getParameter()->isOccupied()) {
				return false;
			}
		}
	}

	return true;
}

bool Test::test(const std::string pretext, testFunction function) {
	std::cout << pretext;

	if (function != NULL && (this->*function)()) {
		std::cout << "OK\n";
		return true;
	}

	return false;
}

void Test::printVarDecls(std::list<DeclInfo*> varDecls) {
	for (DeclInfoIterator it = varDecls.begin(); it != varDecls.end(); ++it) {
		std::cout << "  " << (*it)->ID << " " << (*it)->ID->getName().data()
		          << " = " << (*it)->Parameter->getValue();

		if (!(*it)->Parameter->isOccupied()) {
			std::cout << " NOT OCCUPIED";
		}

		std::list<const clang::IdentifierInfo*> deps = (*it)->Dependences;

		if (!deps.empty()) {
			std::cout << " (";

			for (IDIterator jt = deps.begin(); jt != deps.end(); ++jt) {

				if (jt != deps.begin()) {
					std::cout << ", ";
				}

				std::cout << *jt;
			}

			std::cout << ")";
		}

		std::cout << "\n";
	}
}
