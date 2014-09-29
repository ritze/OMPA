/*
 * Analyse.h
 *
 *  Created on: 15.11.2013
 *      Author: Moritz Lüdecke
 */

#ifndef ANALYSE_H_
#define ANALYSE_H_

#include <clang/Frontend/CompilerInstance.h>

#include "KernelInfo.h"
#include "Warnings.h"

class Analyse {
public:
	Analyse(const std::string kernel, bool suppressAllDiagnostics = true);
	virtual ~Analyse();

	KernelInfo* getKernelInfo();
	clang::SourceManager& getSourceManager();

private:
	void run();
	template<typename T> static std::string getText(
	        const clang::SourceManager &sourceManager, const T &Node);

	clang::CompilerInstance compiler;
	KernelInfo* kernelInfo;
	Warnings* warnings;
};

#endif /* ANALYSE_H_ */
