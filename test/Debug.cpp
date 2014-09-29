/*
 * Debug.cpp
 *
 *  Created on: 22.01.2014
 *      Author: Moritz Lüdecke
 */

#include <list>
#include <vector>
#include <string>
#include <time.h>

#include <clang/Basic/IdentifierTable.h>
#include <clang/Lex/Lexer.h>
#include <llvm/Support/raw_ostream.h>

#include <src/Context.h>
#include <src/DeclInfo.h>
#include <src/KernelBuiltinFunctions.h>
#include <src/FunctionPattern.h>
#include <src/ParameterPattern.h>

#include "Debug.h"

using namespace clang;

template<typename T>
static std::string getText(const SourceManager &sourceManager, const T &Node) {
	SourceLocation StartSpellingLocation = sourceManager.getSpellingLoc(
	        Node.getLocStart());
	SourceLocation EndSpellingLocation = sourceManager.getSpellingLoc(
	        Node.getLocEnd());

	if (!StartSpellingLocation.isValid() || !EndSpellingLocation.isValid()) {
		return std::string();
	}

	bool Invalid = true;
	const char *Text = sourceManager.getCharacterData(StartSpellingLocation,
	                                                  &Invalid);

	if (Invalid) {
		return std::string();
	}

	std::pair<FileID, unsigned> Start = sourceManager.getDecomposedLoc(
	        StartSpellingLocation);
	std::pair<FileID, unsigned> End = sourceManager.getDecomposedLoc(
	        Lexer::getLocForEndOfToken(EndSpellingLocation, 0, sourceManager,
	                                   LangOptions()));

	if (Start.first != End.first) {
		// Start and end are in different files.
		return std::string();
	}

	if (End.second < Start.second) {
		// Shuffling text with macros may cause this.
		return std::string();
	}

	return std::string(Text, End.second - Start.second);
}

Debug::Debug(SourceManager &sourceManager) :
		sourceManager(&sourceManager) {
}

void Debug::print(KernelInfo* kernelInfo) {
	llvm::outs()
	        << "\n########################################################"
	        << "###############################################################"
	        << "\n\n";

	std::vector<KernelFunction> functions = kernelInfo->getFunctions();

	// Set all function parameter values to 42
	for (std::vector<KernelFunction>::iterator it = functions.begin();
	        it != functions.end(); ++it) {

		std::list<ParmDeclInfo*> ParmVarDecls = it->getParmVarDecls();
		int value = 10;
		for (std::list<ParmDeclInfo*>::iterator jt = ParmVarDecls.begin();
		        jt != ParmVarDecls.end(); ++jt) {

			// Calculate live!
			if ((*jt)->hasReserveVariable()) {
				(*jt)->getReference() = value;
				value += 10;
			}
		}
	}

	// Setup kernel setting
	kernelInfo->setSetting(32, 8, 0);

	for (std::vector<KernelFunction>::iterator it = functions.begin();
	        it != functions.end(); ++it) {

		llvm::outs() << "\n";
		const std::string KFCode = getText(*sourceManager, it->getDecl());
		llvm::outs() << KFCode << "\n\n";

		std::list<const clang::IdentifierInfo*> ParmDecls = it->getParmDecls();
		if (!ParmDecls.empty()) {
			llvm::outs() << "ParmDecls (" << it->getDecl().param_size()
			             << "):\n";
		}
		typedef std::list<const clang::IdentifierInfo*>::iterator IDIterator;
		for (IDIterator jt = ParmDecls.begin(); jt != ParmDecls.end(); ++jt) {
			llvm::outs() << "  " << (*jt) << " " << (*jt)->getName() << "\n";
		}

		std::list<ParmDeclInfo*> ParmVarDecls = it->getParmVarDecls();
		if (!ParmVarDecls.empty()) {
			llvm::outs() << "ParmVarDecls:\n";
		}
		for (std::list<ParmDeclInfo*>::iterator jt = ParmVarDecls.begin();
		        jt != ParmVarDecls.end(); ++jt) {

			llvm::outs() << "  " << (*jt)->ID << " ";

			if ((*jt)->accessMode & AF_Read) {
				llvm::outs() << "R";
			} else {
				llvm::outs() << " ";
			}

			if ((*jt)->accessMode & AF_Write) {
				llvm::outs() << "W";
			} else {
				llvm::outs() << " ";
			}

			llvm::outs() << " " << (*jt)->ID->getName() << " = "
			             << (*jt)->Parameter->getValue();

			std::list<const IdentifierInfo*> Deps = (*jt)->Dependences;
			if (!Deps.empty()) {
				llvm::outs() << " (";
				for (std::list<const IdentifierInfo*>::iterator kt =
				        Deps.begin(); kt != Deps.end(); ++kt) {

					if (kt != Deps.begin()) {
						llvm::outs() << ", ";
					}

					llvm::outs() << *kt;
				}
				llvm::outs() << ")";
			}
			llvm::outs() << "\n";
		}

		std::list<ParmArrayInfo*> ParmArrayDecls = it->getParmArrayDecls();
		if (!ParmArrayDecls.empty()) {
			llvm::outs() << "ParmArrayDecls:\n";
		}
		for (std::list<ParmArrayInfo*>::iterator jt = ParmArrayDecls.begin();
		        jt != ParmArrayDecls.end(); ++jt) {

			llvm::outs() << "  " << (*jt)->ID << " " << (*jt)->ID->getName()
			             << "[" << (*jt)->getSize() << "]: ";

			if ((*jt)->array == NULL) {
				llvm::outs() << "NULL\n";

			} else {
				llvm::outs() << "\n";
			}
			llvm::outs() << (*jt)->array->getDescription();

			llvm::outs() << "    AccessModes:\n";
			typedef std::list<AccessModeElement*>::iterator AccessIterator;
			for (AccessIterator access = (*jt)->accessModes.begin();
			        access != (*jt)->accessModes.end(); ++access) {

				llvm::outs() << "      [" << (*access)->index->getValue()
				             << "] ";
				if ((*access)->accessMode & AF_Read) {
					llvm::outs() << "R";
				}

				if ((*access)->accessMode & AF_Write) {
					llvm::outs() << "W";
				}

				llvm::outs() << "\n";
			}
		}

		std::list<ArrayInfo*> ArrayDecls = it->getArrayDecls();
		if (!ArrayDecls.empty()) {
			llvm::outs() << "ArrayDecls:\n";
		}
		for (std::list<ArrayInfo*>::iterator jt = ArrayDecls.begin();
		        jt != ArrayDecls.end(); ++jt) {

			llvm::outs() << "  " << (*jt)->ID << " " << (*jt)->ID->getName()
			        << "[" << (*jt)->getSize() << "]: ";

			uint index = 0;
			if ((*jt)->array == NULL) {
				llvm::outs() << "NULL";
			}
			llvm::outs() << "\n";

			llvm::outs() << (*jt)->array->getDescription();
		}

		std::list<DeclInfo*> VarDecls = it->getVarDecls();
		if (!VarDecls.empty()) {
			llvm::outs() << "VarDecls:\n";
		}
		for (std::list<DeclInfo*>::iterator jt = VarDecls.begin();
		        jt != VarDecls.end(); ++jt) {

			if ((*jt)->ID == NULL) {
				llvm::outs() << "  NULL = ";

			} else {
				llvm::outs() << "  " << (*jt)->ID << " " << (*jt)->ID->getName()
				             << " = ";
			}

			if ((*jt)->Parameter->isOccupied()) {
				llvm::outs() << (*jt)->Parameter->getValue();

			} else {
				llvm::outs() << "NULL";
			}
			llvm::outs() << " | ";

			if (!(*jt)->Parameter->isOccupied()) {
				llvm::outs() << "Calculation: NOT OCCUPIED; ";
			}

			std::list<const IdentifierInfo*> Deps = (*jt)->Dependences;
			if (!Deps.empty()) {
				llvm::outs() << "Dependences: ";
				for (std::list<const IdentifierInfo*>::iterator kt =
				        Deps.begin(); kt != Deps.end(); ++kt) {

					if (kt != Deps.begin()) {
						llvm::outs() << ", ";
					}

					llvm::outs() << *kt;
				}
			}
			llvm::outs() << "\n";
		}
	}

	llvm::outs() << "\n";

	for (uint i = 0; i < kernelInfo->getFunctionSize(); ++i) {
		FunctionPattern* function = kernelInfo->getFunction(i);

		llvm::outs() << function->getName() << " ("
		             << function->getParameterSize() << "):\n";

		for (uint j = 0; j < function->getParameterSize(); ++j) {
			ParameterPattern* parameter = function->getParameter(j);

			llvm::outs() << "  " << parameter->name;
			if (parameter->type == PatternType_Variable) {
				llvm::outs() << " (Variable): ";

				if (parameter->reads[0]) {
					llvm::outs() << "R";
				}

				if (parameter->writes[0]) {
					llvm::outs() << "W";
				}

				llvm::outs() << "\n";

			} else if (parameter->type == PatternType_Array) {
				llvm::outs() << " (Array):\n    READS:";

				for (uint i = 0; i < parameter->readsSize; ++i) {
					if (i % 64 == 0) {
						llvm::outs() << "\n    ";

					} else if (i % 8 == 0) {
						llvm::outs() << ".";
					}

					if (parameter->reads[i]) {
						llvm::outs() << "1";

					} else {
						llvm::outs() << "0";
					}
				}

				llvm::outs() << "\n    WRITES:";
				for (uint i = 0; i < parameter->writesSize; ++i) {
					if (i % 64 == 0) {
						llvm::outs() << "\n    ";

					} else if (i % 8 == 0) {
						llvm::outs() << ".";
					}

					if (parameter->writes[i]) {
						llvm::outs() << "1";

					} else {
						llvm::outs() << "0";
					}
				}
				llvm::outs() << "\n";

			} else {
				llvm::outs() << " (None)\n";
			}
		}
	}
}
