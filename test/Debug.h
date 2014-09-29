/*
 * Debug.h
 *
 *  Created on: 22.02.2014
 *      Author: Moritz Lüdecke
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <string>

#include <clang/Basic/SourceManager.h>

#include <src/KernelInfo.h>

class Debug {
public:
	Debug(clang::SourceManager &sourceManager);

	void print(KernelInfo* kernelInfo);

private:
	clang::SourceManager* sourceManager;
};

#endif /* DEBUG_H_ */
