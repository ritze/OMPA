/*
 * DeclInfo.cpp
 *
 *  Created on: 03.01.2014
 *      Author: Moritz Lüdecke
 */

#include "DeclInfo.h"
#include <llvm/Support/raw_ostream.h>

static int UNDEFINED = 0;

ParameterInfo::ParameterInfo() :
		Parameter(new class Parameter) {
}

ParameterInfo::ParameterInfo(class Parameter* parameter) :
		Parameter(new class Parameter) {

	Parameter->pushParameter(parameter);
}

ParameterInfo::ParameterInfo(int variable) :
		Parameter(new class Parameter) {

	Parameter->pushVariable(variable);
}

ParameterInfo::~ParameterInfo() {
	if (Parameter != NULL) {
		delete Parameter;
	}
}

DeclInfo::DeclInfo(const clang::IdentifierInfo* ID) :
		ParameterInfo(), ID(ID) {
}

DeclInfo::~DeclInfo() {
	if (Parameter != NULL) {
		delete Parameter;
	}
}

ParmDeclInfo::ParmDeclInfo(const clang::IdentifierInfo* ID,
                           bool reserveVariable, Access accessMode) :
		DeclInfo(ID), accessMode(accessMode), reserveVariable(reserveVariable),
		storage(new int(UNDEFINED)) {

	if (reserveVariable) {
		Parameter->pushReference(storage);
	}
}

ParmDeclInfo::~ParmDeclInfo() {
	if (storage != NULL) {
		delete storage;
	}
}

int& ParmDeclInfo::getReference() {
	return *storage;
}

bool ParmDeclInfo::hasReserveVariable() {
	return reserveVariable;
}

ArrayInfo::ArrayInfo(const clang::IdentifierInfo* ID, Parameter* size,
                     bool initialize) :
		ID(ID), array(new Array(size)) {

	if (initialize && size != NULL) {
		for (uint i = 0; i < size->getValue(); ++i) {
			Parameter* parameter = new Parameter;
			parameter->pushVariable(0);
			Parameter* index = new Parameter;
			index->pushVariable(i);

			array->pushElement(index, parameter);
		}
	}
}

Parameter* ArrayInfo::getElement(Parameter* index) {
	Parameter* parameter = array->getElement(index, array->getTimestamp());

	if (parameter == NULL) {
		parameter = new Parameter;
	}

	return parameter;
}

uint ArrayInfo::getSize() {
	Parameter* size = array->getSize();

	if (size == NULL) {
		return 0;

	} else {
		return (uint) size->getValue();
	}
}

uint ArrayInfo::getTimestamp() {
	return array->getTimestamp();
}

AccessModeElement::AccessModeElement(Parameter* index, Access accessMode) :
		index(index), accessMode(accessMode) {
}

AccessModeElement::~AccessModeElement() {
	if (index != NULL) {
		delete index;
	}
}

void ArrayInfo::pushParameter(Parameter* index, Parameter* parameter) {
	array->pushElement(index, parameter);
}

ParmArrayInfo::ParmArrayInfo(const clang::IdentifierInfo* ID, Parameter* size,
                             bool initialize) :
		ArrayInfo(ID, size, initialize) {
}

void ParmArrayInfo::addAccess(Parameter* index, Access accessMode) {
	AccessModeElement* access = new AccessModeElement(index, accessMode);
	accessModes.push_back(access);
}
