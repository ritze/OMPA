/*
 * KernelFunction.h
 *
 *  Created on: 25.12.2013
 *      Author: Moritz Lüdecke
 */

#ifndef KERNELFUNCTION_H_
#define KERNELFUNCTION_H_

#include <stack>
#include <stddef.h>

#include <clang/AST/Decl.h>
#include <clang/AST/OperationKinds.h>

#include "Context.h"
#include "DeclInfo.h"
#include "KernelBuiltinFunctions.h"

// Holds the parameters of a kernel function
class KernelFunction {
public:
	KernelFunction(const clang::FunctionDecl D, KernelBuiltinFunctions* BF);

	const clang::FunctionDecl getDecl();
	void addArrayDecl(const clang::IdentifierInfo* ID, const uint size,
	                  bool initialize = false);
	void addParmDecls(const clang::IdentifierInfo* ID);
	void addParmVarDecl(const clang::IdentifierInfo* ID);
	// The boolean hasInitialization is for the special case
	//   int a;
	// a has no initialization and
	//   int a, b = 0;
	// a has no initialization, but b.
	void addVarDecl(const clang::IdentifierInfo* ID, bool hasInitialization =
	                        true);
	std::list<ParmArrayInfo*> getParmArrayDecls();
#ifdef DEBUG
	std::list<ArrayInfo*> getArrayDecls();
#endif /* DEBUG */
	std::list<const clang::IdentifierInfo*> getParmDecls();
	std::list<ParmDeclInfo*> getParmVarDecls();
#ifdef DEBUG
	std::list<DeclInfo*> getVarDecls();
#endif /* DEBUG */
	// Remember the accessed array for read or write operations on the next step
	void openArray(const clang::StringRef name, bool write, bool isParmVar =
	                       false);
	// Sets the index of the last defined array
	// Returns true, if there was a index registered,
	// or indexOperation was not occupied, otherwise false.
	bool setIndexVar(uint value);
	bool setIndexVar(const clang::IdentifierInfo* ID, bool isParmVar = false);
	// Sets the index of the last called OpenCL Work-Item Built-In Function
	// Returns true if there was a parameter registered
	// or calleeOperation was not occupied, otherwise false.
	bool setParmVar(uint i);
	bool setParmVar(const clang::IdentifierInfo* ID, bool isParmVar = false);
	void updateArrayDecl(const clang::IdentifierInfo* ID,
	                     bool isParmVar = false);
	void updateArrayDecl(int value);
	void updateParmVarDecl(const clang::IdentifierInfo* ID);
	// Push the next Unary or Binary Opcode
	void updateVarDecl(const clang::IdentifierInfo* ID, bool isCallee = false);
	typedef unsigned Opcode;
	void updateVarDeclOp(Opcode opc, bool isUnary = false, Context context = 0);
	//TODO: We only support Integers! Expand this to float
	void updateVarDecl(int value);

private:
	// We have to save the access type of an opened array
	enum ArrayAccess {
		AA_None = 0, AA_Read = 1, AA_Write = 2
	};
	KernelBuiltinFunctions* BuiltinFunctions;
	ArrayInfo* currentArray;
	typedef unsigned int ArrayAccessContext;
	ArrayAccessContext currentArrayAccess;
	ArrayInfo* currentArrayDecl;
	ParmArrayInfo* currentParmArray;
	DeclInfo* currentVarDecl;
#ifdef DEBUG
	std::list<DeclInfo*>::iterator currentVarDeclIterator;
#endif /* DEBUG */
	clang::FunctionDecl Decl;

	std::list<const clang::IdentifierInfo*> ParmDecls;
	std::list<ParmDeclInfo*> ParmVarDecls;
	std::list<ParmArrayInfo*> ParmArrayDecls;
	std::list<DeclInfo*> VarDecls;
	std::list<ArrayInfo*> ArrayDecls;

	Parameter* calleeOperation;
	Parameter* indexOperation;
	Parameter* tempOperation;
	Parameter* tempArrayOperation;
	// Returns false, if calleeOperation is already an occupied operation.
	bool addCalleeOperation(Opcode opc, bool isUnary = false);
	// Returns false, if calleeOperation is already an occupied operation.
	bool addIndexOperation(Opcode opc, bool isUnary = false);
	// Returns true, if opc is a Decrement or Increment, otherwise false.
	bool addTempOperation(Opcode opc);
	// Check if ID is already in ParmVarDecls,
	// if not add ID to ParmVarDecls and return false, otherwise return true
	bool checkParmDecl(const clang::IdentifierInfo* ID);
	// Returns the last definition of given ID.
	// Returns null, if no definition found with the given ID
	template<class T>
	T findDecl(std::list<T> list, const clang::IdentifierInfo* ID);
	// Returns the last definition of given ID.
	// Returns null, if no definition found with the given ID
	template<class T>
	Parameter* findVariable(std::list<T> list, const clang::IdentifierInfo* ID);
	// Returns the last definition of given Array.
	// Returns null, if no ArrayDecl found with the given ID
	template<class T>
	T findArray(std::list<T> list, const clang::StringRef name);
	// Returns the indexOperation and set indexOperation to NULL,
	// if indexOperation is occupied,
	// otherwise it returns NULL
	Parameter* getIndexOperationValue();
	// Finish Decrement or Increment operations
	bool updateTempOperation(const clang::IdentifierInfo* ID, Parameter* parm,
	                         bool push2VarDecl = true, bool push2ParmVarDecl =
	                                 false);
	bool updateTempArrayOperation(Parameter* index);
	bool setIndexVar(Parameter* index);
	void updateArrayDecl(Parameter* value);
};

#endif /* KERNELFUNCTION_H_ */
