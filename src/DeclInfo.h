/*
 * DeclInfo.h
 *
 *  Created on: 25.12.2013
 *      Author: Moritz Lüdecke
 */

#ifndef DECLINFO_H_
#define DECLINFO_H_

#include <stddef.h>
#include <list>
#include <vector>

#include <clang/Basic/IdentifierTable.h>

#include "Operation.h"
#include "Array.h"

enum AccessFlags {
	AF_None  = 0,  // the object do nothing
	AF_Read  = 1,  // the object has a read access
	AF_Write = 2   // the object has a write access
};

typedef unsigned int Access;

class ParameterInfo {
public:
	ParameterInfo();
	ParameterInfo(Parameter* parameter);
	ParameterInfo(int value);
	~ParameterInfo();

	std::list<const clang::IdentifierInfo*> Dependences;
	Parameter* Parameter;
};

class DeclInfo: public ParameterInfo {
public:
	DeclInfo(const clang::IdentifierInfo* ID);
	~DeclInfo();

	const clang::IdentifierInfo* ID;
};

class ParmDeclInfo: public DeclInfo {
public:
	ParmDeclInfo(const clang::IdentifierInfo* ID, bool reserveVariable = true,
	             Access accessModue = AF_None);
	~ParmDeclInfo();

	int& getReference();
	bool hasReserveVariable();
	Access accessMode;

private:
	bool reserveVariable;
	int* storage;
};

class ArrayInfo {
public:
	ArrayInfo(const clang::IdentifierInfo* ID, Parameter* size,
	          bool initialize);

	Parameter* getElement(Parameter* index);
	uint getSize();
	uint getTimestamp();
	void pushParameter(Parameter* index, Parameter* parameter);

	const clang::IdentifierInfo* ID;
	// TODO: Implement this in KernelFunction
	std::list<const clang::IdentifierInfo*> Dependences;
	Array* array;
};

struct AccessModeElement {
	AccessModeElement(Parameter* index, Access accessMode = AF_None);
	~AccessModeElement();

	Parameter* index;
	Access accessMode;
};

class ParmArrayInfo: public ArrayInfo {
public:
	ParmArrayInfo(const clang::IdentifierInfo* ID, Parameter* size,
	              bool initialize);

	void addAccess(Parameter* index, Access accessMode);
	std::list<AccessModeElement*> accessModes;
};

#endif /* DECLINFO_H_ */
