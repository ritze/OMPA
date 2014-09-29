/*
 * Operation.h
 *
 *  Created on: 18.12.2013
 *      Author: Moritz Lüdecke
 */

#ifndef OPERATION_H_
#define OPERATION_H_

#include <stdlib.h>

#include <clang/AST/OperationKinds.h>

#ifdef DEBUG
#include <sstream>
#include <string>
#endif /* DEBUG */

#include "Array.h"
#include "ArrayTimestamp.h"

typedef unsigned Opcode;

enum ParameterType {
	PT_None,
	PT_Var,
	PT_BO,
	PT_UO,
	PT_Par
};

class Array;
class ArrayTimestamp;
class BinaryOperation;
class Parameter;
class UnaryOperation;
class Variable;

union ParameterObject {
	Variable* variable;
	BinaryOperation* binaryOperation;
	UnaryOperation* unaryOperation;
	Parameter* parameter;
};

class Parameter {
public:
	Parameter();
	~Parameter();

#ifdef DEBUG
	std::string getDescription();
#endif /* DEBUG */
	int getValue();
	bool isOccupied();
	bool pushReference(int* ref);
	bool pushOperation(BinaryOperation* operation);
	bool pushOperation(UnaryOperation* operation);
	// You have to inherit the array,
	// because the value of index is unknown at the analysis
	bool pushArrayElement(Array* array, Parameter* index);
	// Returns true, if there was a variable free to fill, otherwise false.
	bool pushVariable(Variable* var);
	bool pushVariable(int value);
	bool pushParameter(Parameter* parameter);

	ParameterType type;
	ParameterObject object;
};

class Variable {
	friend class Variable;

public:
	Variable();
	~Variable();

#ifdef DEBUG
	std::string getDescription();
#endif /* DEBUG */
	int getValue();
	bool isOccupied();
	// You have to inherit the array,
	// because the value of index is unknown at the analysis
	bool pushArrayElement(Array* array, Parameter* index);
	// Returns true, if there was a variable free to fill, otherwise false.
	bool pushVariable(Variable* var);
	bool pushVariable(int value);
	bool pushReference(int* ref);

private:
	bool holdInArray;
	bool occupied;
	bool referenced;
	int* reference;
	int variable;
	ArrayTimestamp* array;
};

class BinaryOperation {
public:
	BinaryOperation(Opcode opc);
	~BinaryOperation();

#ifdef DEBUG
	std::string getDescription();
#endif /* DEBUG */
	// Returns the integer result of the calculation.
	// If a variable is not set, it returns 0;
	int getValue();
	bool isOccupied();
	// You have to inherit the array,
	// because the value of index is unknown at the analysis
	bool pushArrayElement(Array* array, Parameter* index);
	// Returns true, if there was a variable free to fill, otherwise false.
	bool pushVariable(Variable* var);
	bool pushVariable(int value);
	bool pushReference(int* ref);
	bool pushOperation(BinaryOperation* operation);
	bool pushOperation(UnaryOperation* operation);

private:
	Parameter a;
	Parameter b;
#ifdef DEBUG
	std::string operandDescription;
#endif /* DEBUG */

	int (*operand)(int, int);
	// XXX: Workaround: When we doesn't support floats, we are rounding float
	//      values. So we have to check, if we divide through null.
	bool isDivOperand;
};

class UnaryOperation {
public:
	UnaryOperation(Opcode opc);
	~UnaryOperation();

#ifdef DEBUG
	std::string getDescription();
#endif /* DEBUG */
	// Returns the integer result of the calculation.
	// If a variable is not set, it returns 0;
	int getValue();
	bool isOccupied();
	// You have to inherit the array,
	// because the value of index is unknown at the analysis
	bool pushArrayElement(Array* array, Parameter* index);
	// Returns true, if there was a variable free to fill, otherwise false.
	bool pushVariable(Variable* var);
	bool pushVariable(int value);
	bool pushReference(int* ref);
	bool pushOperation(BinaryOperation* operation);
	bool pushOperation(UnaryOperation* operation);

private:
	Parameter a;
#ifdef DEBUG
	bool isPrefix;
	std::string operandDescription;
#endif /* DEBUG */

	int (*operand)(int);
};

#endif /* OPERATION_H_ */
