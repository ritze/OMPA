/*
 * KernelInfo.h
 *
 *  Created on: 07.12.2013
 *      Author: Moritz Lüdecke
 */

#ifndef KERNELINFO_H_
#define KERNELINFO_H_

#include <map>
#include <vector>

#include <clang/AST/Decl.h>

#include "FunctionPattern.h"
#include "KernelFunction.h"
#include "KernelBuiltinFunctions.h"
#include "KernelSetting.h"
#include "Warnings.h"

typedef std::list<const clang::IdentifierInfo*> IDList;
typedef std::list<ParmArrayInfo*> ArraysList;
typedef std::list<ParmDeclInfo*> VariablesList;
typedef std::list<AccessModeElement*> AccessModesList;
typedef std::vector<ParameterPattern*> ParameterPatternList;
typedef std::list<ParameterPatternList*> LocalParametersList;
typedef std::map<uint, std::pair<bool, bool> > SortedAccessModesList;
typedef std::pair<uint, uint> MaxSizes;

class KernelInfo {
	friend class KernelASTVisitor;
public:
	KernelInfo(Warnings* warnings);
	uint getFunctionSize();
	FunctionPattern* getFunction(uint index);

	std::vector<KernelFunction> getFunctions();

	bool setSetting(size_t global_work_size, size_t local_work_size,
	                size_t global_work_offset, uint work_dim = 1,
	                bool clEnqueueTask = false);

private:
	void addFunction(const clang::FunctionDecl D);
	ParameterPatternList* buildLocalPattern(const uint index);
	LocalParametersList* buildLocalParametersList(uint index);
	std::pair<uint, uint>* getMaxSizes(LocalParametersList* localParametersList,
	                                   uint index);
	ParameterPatternList* buildGlobalParametersList(uint index);
	KernelFunction* getCurrentFunction();

	std::vector<KernelFunction> Functions;
	KernelBuiltinFunctions* BuiltinFunctions;
	KernelSetting* Setting;
};

#endif /* KERNELINFO_H_ */
