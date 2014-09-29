/*
 * KernelFunction.cpp
 *
 *  Created on: 25.12.2013
 *      Author: Moritz Lüdecke
 */

#include <clang/AST/ASTConsumer.h>
#include <clang/Basic/SourceManager.h>

#include "KernelFunction.h"
#include "KernelBuiltinFunctions.h"

KernelFunction::KernelFunction(const clang::FunctionDecl D,
                               KernelBuiltinFunctions* BF) :
		calleeOperation(NULL), indexOperation(NULL), tempOperation(NULL),
		Decl(D), BuiltinFunctions(BF), currentArray(NULL),
		currentParmArray(NULL), currentArrayAccess(AA_None),
		currentArrayDecl(NULL), tempArrayOperation(NULL), currentVarDecl(NULL) {
};

void KernelFunction::addArrayDecl(const clang::IdentifierInfo* ID,
                                  const uint size, bool initialize) {

	if (tempOperation == NULL && size > 0) {
		Parameter* sizeParameter = new Parameter;
		sizeParameter->pushVariable(size);
		ArrayInfo* arrayInfo = new ArrayInfo(ID, sizeParameter, initialize);
		ArrayDecls.push_back(arrayInfo);

		if (initialize) {
			currentArrayDecl = NULL;

		} else {
			currentArrayDecl = ArrayDecls.back();
		}
	}
}

void KernelFunction::addParmDecls(const clang::IdentifierInfo* ID) {
	ParmDecls.push_back(ID);
}

void KernelFunction::addParmVarDecl(const clang::IdentifierInfo* ID) {
	if (tempOperation == NULL) {
		ParmDeclInfo* parmDeclInfo = new ParmDeclInfo(ID, false, AF_Write);
		ParmVarDecls.push_back(parmDeclInfo);

		currentVarDecl = ParmVarDecls.back();
	}
}

void KernelFunction::addVarDecl(const clang::IdentifierInfo* ID,
                                bool hasInitialization) {

	// FIXME: tempOperation workaround
	if (tempOperation == NULL) {
		VarDecls.push_back(new DeclInfo(ID));
		currentVarDecl = VarDecls.back();

//		if (currentVarDecl != NULL
//				&& !currentVarDecl->Parameter->isOccupied()) {
//
//			delete VarDecls.back()->Parameter;
//			VarDecls.back()->Parameter = currentVarDecl->Parameter;
//		}

// For the special case: int a; and int a, b = 0;
		if (!hasInitialization) {
			currentVarDecl->Parameter->pushVariable(0);
			currentVarDecl = NULL;
		}

#ifdef DEBUG
		currentVarDeclIterator = VarDecls.end();
		--currentVarDeclIterator;
#endif /* DEBUG */
	}
}

void KernelFunction::updateArrayDecl(const clang::IdentifierInfo* ID,
                                     bool isParmVar) {

	// TODO: Use stack memory for all temporary operations like index operation
//	if (indexOperation != NULL) {
//		indexOperation->pushVariable(value);
//		index = getIndexOperationValue();
//	}

	Parameter* parameter;

	if (isParmVar) {
		parameter = findVariable<ParmDeclInfo*>(ParmVarDecls, ID);

	} else {
		parameter = findVariable<DeclInfo*>(VarDecls, ID);
	}

	updateArrayDecl(parameter);
}

void KernelFunction::updateArrayDecl(int value) {
	Parameter* parameter = new Parameter;
	parameter->pushVariable(value);
	updateArrayDecl(parameter);
}

void KernelFunction::updateParmVarDecl(const clang::IdentifierInfo* ID) {
	checkParmDecl(ID);
	ParmDeclInfo* parmDeclInfo = findDecl<ParmDeclInfo*>(ParmVarDecls, ID);
	if (parmDeclInfo != NULL) {
		parmDeclInfo->accessMode |= AF_Read;
	}

	if (currentVarDecl != NULL && !currentVarDecl->Parameter->isOccupied()) {
		currentVarDecl->Dependences.push_back(ID);
		currentVarDecl->Parameter->pushParameter(parmDeclInfo->Parameter);
	}

	if (tempOperation != NULL) {
		Parameter* parameter = NULL;
		if (parmDeclInfo != NULL) {
			parameter = parmDeclInfo->Parameter;
		}

		updateTempOperation(ID, parameter, false, true);
	}

	// TODO: Use the same infrastructure as VarDecls for ParmVarDecls
	//currentParmVarDecl->Dependences.push_back(ID);
}

const clang::FunctionDecl KernelFunction::getDecl() {
	return Decl;
}

std::list<ParmDeclInfo*> KernelFunction::getParmVarDecls() {
	return ParmVarDecls;
}

#ifdef DEBUG
std::list<ArrayInfo*> KernelFunction::getArrayDecls() {
	return ArrayDecls;
}
#endif /* DEBUG */

std::list<const clang::IdentifierInfo*> KernelFunction::getParmDecls() {
	return ParmDecls;
}

std::list<ParmArrayInfo*> KernelFunction::getParmArrayDecls() {
	return ParmArrayDecls;
}

#ifdef DEBUG
std::list<DeclInfo*> KernelFunction::getVarDecls() {
	return VarDecls;
}
#endif /* DEBUG */

void KernelFunction::openArray(const clang::StringRef name, bool write,
                               bool isParmVar) {

	currentArrayAccess = AA_None;

	if (isParmVar) {
		currentParmArray = findArray<ParmArrayInfo*>(ParmArrayDecls, name);

		if (currentParmArray == NULL) {
			const clang::IdentifierInfo* ID = NULL;

			// Get ID of the array from ParmDecls
			for (std::list<const clang::IdentifierInfo*>::iterator it =
			        ParmDecls.begin(); it != ParmDecls.end() && ID == NULL;
			        ++it) {

				if ((*it)->getName() == name) {
					ID = (*it);
				}
			}

			ParmArrayInfo* parmArrayInfo = new ParmArrayInfo(ID, NULL, false);
			ParmArrayDecls.push_back(parmArrayInfo);
			currentParmArray = ParmArrayDecls.back();
		}

		currentArray = currentParmArray;

	} else {
		currentArray = findArray<ArrayInfo*>(ArrayDecls, name);
	}

	if (write) {
		currentArrayAccess |= AA_Write;

	} else {
		currentArrayAccess |= AA_Read;
	}

	// FIXME: ++array[++i]: i increments two times!
	if (tempOperation != NULL) {
		tempArrayOperation = tempOperation;
		tempOperation = NULL;
	}
}

bool KernelFunction::setIndexVar(uint value) {
	Parameter* index = new Parameter;
	index->pushVariable(value);
	return setIndexVar(index);
}

bool KernelFunction::setIndexVar(const clang::IdentifierInfo* ID,
                                 bool isParmVar) {

	if (currentVarDecl != NULL && !currentVarDecl->Parameter->isOccupied()
	        && (currentArrayAccess & AA_Write)) {
		currentVarDecl->Dependences.push_back(ID);
	}

	Parameter* parameter;

	if (isParmVar) {
		checkParmDecl(ID);
		ParmDeclInfo* parmDeclInfo = findDecl<ParmDeclInfo*>(ParmVarDecls, ID);
		parmDeclInfo->accessMode |= AF_Read;
		parameter = parmDeclInfo->Parameter;

	} else {
		parameter = findVariable<DeclInfo*>(VarDecls, ID);
	}

	updateTempOperation(ID, parameter, !isParmVar, isParmVar);

	return setIndexVar(parameter);
}

bool KernelFunction::setParmVar(uint value) {
	Parameter* parameter;

	if (calleeOperation != NULL) {
		bool result = calleeOperation->pushVariable(value);

		if (!calleeOperation->isOccupied()) {
			return result;
		}

		parameter = calleeOperation;
		calleeOperation = NULL;

	} else {
		parameter = new Parameter();
		parameter->pushVariable(value);
	}

	return BuiltinFunctions->pushIndex(parameter);
}

// FIXME: Handle tempOperation
bool KernelFunction::setParmVar(const clang::IdentifierInfo* ID,
                                bool isParmVar) {

	if (currentVarDecl != NULL) {
		currentVarDecl->Dependences.push_back(ID);
	}

	Parameter* parameter;

	if (isParmVar) {
		checkParmDecl(ID);
		parameter = findVariable<ParmDeclInfo*>(ParmVarDecls, ID);

	} else {
		parameter = findVariable<DeclInfo*>(VarDecls, ID);
	}

	updateTempOperation(ID, parameter, !isParmVar, isParmVar);

	if (calleeOperation != NULL) {
		bool result = calleeOperation->pushParameter(parameter);

		if (!calleeOperation->isOccupied()) {
			return result;
		}

		parameter = calleeOperation;
		calleeOperation = NULL;
	}

	return BuiltinFunctions->pushIndex(parameter);
}

void KernelFunction::updateVarDecl(const clang::IdentifierInfo* ID,
                                   bool isCallee) {

	if (currentVarDecl == NULL || currentVarDecl->Parameter->isOccupied()) {
		if (tempOperation != NULL) {
			Parameter* parameter = findVariable<DeclInfo*>(VarDecls, ID);

			if (parameter == NULL) {
				checkParmDecl(ID);
				parameter = findVariable<ParmDeclInfo*>(ParmVarDecls, ID);
			}

			updateTempOperation(ID, parameter);
		}

		// TODO: Add function support
		//       Don't track built-in functions
		//currentVarDecl->Dependences.push_back(ID);

	} else if (isCallee) {
		// TODO: Handle size_t pointer, if NULL then it isn't a Builtin Function
		int& reference = (int&) BuiltinFunctions->addFunction(ID->getName());
		currentVarDecl->Parameter->pushReference(&reference);

	} else {
		Parameter* parameter = findVariable<DeclInfo*>(VarDecls, ID);
		currentVarDecl->Parameter->pushParameter(parameter);

		updateTempOperation(ID, parameter);
		currentVarDecl->Dependences.push_back(ID);

		// Push the new definition at the end for the special case k = k++;
		if (currentVarDecl->Parameter->isOccupied()
				&& currentVarDecl != VarDecls.back()
				&& currentVarDecl != ParmVarDecls.back()
				&& currentVarDecl->ID != NULL) {

			VarDecls.push_back(currentVarDecl);
#ifdef DEBUG
			VarDecls.erase(currentVarDeclIterator);
#endif /* DEBUG */
		}
	}
}

void KernelFunction::updateVarDecl(int value) {
	if (currentVarDecl != NULL && !currentVarDecl->Parameter->isOccupied()) {
		currentVarDecl->Parameter->pushVariable(value);
	}
}

void KernelFunction::updateVarDeclOp(Opcode opc, bool isUnary,
                                     Context context) {

	if (isUnary) {
		addTempOperation(opc);
	}

	if (context & CF_Parm) {
		addCalleeOperation(opc, isUnary);
		return;
	}

	if ((context & CF_Index) || (context & CF_Array)) {
		addIndexOperation(opc, isUnary);
		return;
	}

	if (currentVarDecl != NULL && !currentVarDecl->Parameter->isOccupied()) {
		if (isUnary) {
			currentVarDecl->Parameter->pushOperation(new UnaryOperation(opc));

		} else {
			currentVarDecl->Parameter->pushOperation(new BinaryOperation(opc));
		}
	}
}

//TODO: Handle with tempOperation (UnaryOperations...)
//      Use enum
#define DEFINE_ADD_OP(name, operation)                       \
bool KernelFunction::name(Opcode opc, bool isUnary) {        \
	if (operation == NULL) {                                 \
		operation = new Parameter();                         \
	}                                                        \
	                                                         \
	if (isUnary) {                                           \
		operation->pushOperation(new UnaryOperation(opc));   \
	                                                         \
	} else {                                                 \
		operation->pushOperation(new BinaryOperation(opc));	 \
	}                                                        \
	                                                         \
	return true;                                             \
}                                                            \

DEFINE_ADD_OP(addCalleeOperation, calleeOperation)
DEFINE_ADD_OP(addIndexOperation, indexOperation)
#undef DEFINE_ADD_OP

bool KernelFunction::addTempOperation(Opcode opc) {
	if (opc != clang::UO_PreDec && opc != clang::UO_PreInc
	        && opc != clang::UO_PostDec && opc != clang::UO_PostInc) {

		return false;
	}

	Parameter* parameter = new Parameter();
	parameter->pushOperation(new BinaryOperation(clang::BO_Add));

	// x = x + 1;
	if (opc == clang::UO_PreInc || opc == clang::UO_PostInc) {
		parameter->pushVariable(1);

		// x = x - 1;
	} else {
		parameter->pushVariable(-1);
	}

	tempOperation = parameter;

	return true;
}

bool KernelFunction::updateTempOperation(const clang::IdentifierInfo* ID,
                                         Parameter* parm, bool push2VarDecl,
                                         bool push2ParmVarDecl) {

	if (tempOperation == NULL) {
		return false;
	}

	tempOperation->pushParameter(parm);

	if (push2VarDecl) {
		DeclInfo* varDecl = new DeclInfo(ID);
		varDecl->Parameter->pushParameter(tempOperation);
		VarDecls.push_back(varDecl);
	}

	if (push2ParmVarDecl) {
		ParmDeclInfo* parmVarDecl = new ParmDeclInfo(ID, false, AF_Write);
		parmVarDecl->Parameter->pushParameter(tempOperation);
		ParmVarDecls.push_back(parmVarDecl);
	}

	tempOperation = NULL;

	return true;
}

// TODO: Use a stack memory for temporary operations
bool KernelFunction::updateTempArrayOperation(Parameter* index) {
	if (tempArrayOperation == NULL) {
		return false;
	}

	Parameter* parm = currentArray->getElement(index);
	tempArrayOperation->pushParameter(parm);
	currentArray->pushParameter(index, tempArrayOperation);

	tempArrayOperation = NULL;

	return true;
}

bool KernelFunction::checkParmDecl(const clang::IdentifierInfo* ID) {
	ParmDeclInfo* found = findDecl<ParmDeclInfo*>(ParmVarDecls, ID);
	if (found == NULL) {
		ParmDeclInfo* parmDeclInfo = new ParmDeclInfo(ID, true, AF_None);
		ParmVarDecls.push_back(parmDeclInfo);
		return false;
	}

	return true;
}

template<class T>
T KernelFunction::findDecl(std::list<T> list, const clang::IdentifierInfo* ID) {
	typedef typename std::list<T>::reverse_iterator iterator;

	for (iterator it = list.rbegin(); it != list.rend(); ++it) {
		if ((*it)->ID == ID && (*it)->Parameter->isOccupied()) {
			return *it;
		}
	}

	return NULL;
}

template<class T>
Parameter* KernelFunction::findVariable(std::list<T> list,
                                        const clang::IdentifierInfo* ID) {

	T variable = findDecl<T>(list, ID);

	if (variable != NULL) {
		return variable->Parameter;

	} else {
		return NULL;
	}
}

template<class T>
T KernelFunction::findArray(std::list<T> list, const clang::StringRef name) {
	typedef typename std::list<T>::reverse_iterator iterator;

	for (iterator it = list.rbegin(); it != list.rend(); ++it) {
		if ((*it)->ID->getName() == name) {
			return (*it);
		}
	}

	return NULL;
}

Parameter* KernelFunction::getIndexOperationValue() {
	Parameter* parameter = indexOperation;

	if (indexOperation != NULL && indexOperation->isOccupied()) {
		indexOperation = NULL;
	}

	return parameter;
}

bool KernelFunction::setIndexVar(Parameter* value) {
	bool result = false;

	if (currentArray != NULL) {
		Parameter* index = value;

		if (indexOperation != NULL) {
			result = indexOperation->pushParameter(value);
			index = getIndexOperationValue();
		}

		if (index != NULL && index->isOccupied()) {
			if ((currentArrayAccess & AA_Write) && tempArrayOperation == NULL) {
				if (currentVarDecl == NULL
				        || currentVarDecl->Parameter->isOccupied()) {

					// TODO: Use currentVarDecl as a stack memory.
					currentVarDecl = new DeclInfo(NULL);
					currentVarDecl->Parameter = new Parameter;
				}

				currentArray->pushParameter(index, currentVarDecl->Parameter);
				if (currentParmArray != NULL) {
					currentParmArray->addAccess(index, AF_Write);
				}

				currentArray = NULL;
				currentParmArray = NULL;
				currentArrayAccess = AA_None;

			} else {
				// XXX: This if-statement is a workaround for structs,
				// because structs are not supported, but ignored!
				if (currentVarDecl != NULL) {
					// FIXME: Use updateVarDecl: var = f([array[i]);
					//updateVarDecl(index);
					currentVarDecl->Dependences.push_back(currentArray->ID);

					currentVarDecl->Parameter->pushArrayElement(
					        currentArray->array, index);
					if (currentParmArray != NULL) {
						currentParmArray->addAccess(index, AF_Read);
					}

					updateTempArrayOperation(index);

					currentArray = NULL;
					currentParmArray = NULL;
					currentArrayAccess = AA_None;
				}
			}

			result = true;
		}
	}

	return result;
}

void KernelFunction::updateArrayDecl(Parameter* value) {
	if (value == NULL) {
		return;
	}

	Parameter* parameter = value;

	if (indexOperation != NULL) {
		indexOperation->pushParameter(value);
		Parameter* tempParameter = getIndexOperationValue();
		if (tempParameter != NULL) {
			parameter = tempParameter;
		}
	}

	if (currentArrayDecl != NULL
	        && currentArrayDecl->getTimestamp() < currentArrayDecl->getSize()) {

		Parameter* parameterIndex = new Parameter;
		parameterIndex->pushVariable(currentArrayDecl->array->getTimestamp());
		currentArrayDecl->pushParameter(parameterIndex, parameter);

		if (currentArrayDecl->getTimestamp() >= currentArrayDecl->getSize()) {
			currentArrayDecl = NULL;
		}
	}
}
