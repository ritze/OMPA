/*
 * Analyse.cpp
 *
 *  Created on: 15.11.2013
 *      Author: Moritz Lüdecke
 */

#include <time.h>

#include <clang/AST/ASTContext.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Parse/ParseAST.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/MemoryBuffer.h>

#include "Analyse.h"
#include "KernelASTVisitor.h"

using namespace clang;

class KernelConsumer: public ASTConsumer {
public:
	KernelConsumer(KernelInfo* kernelInfo, Warnings* warnings) :
			visitor(KernelASTVisitor(kernelInfo, warnings)) {
	};

	bool HandleTopLevelDecl(DeclGroupRef DGR) {
		for (DeclGroupRef::iterator it = DGR.begin(); it != DGR.end(); ++it) {
			visitor.TraverseDecl(*it);
		}
		return true;
	}

private:
	KernelASTVisitor visitor;
};

Analyse::Analyse(const std::string kernelCode, bool suppressAllDiagnostics) {
	warnings = new Warnings();
	kernelInfo = new KernelInfo(warnings);

	compiler.createDiagnostics();
	compiler.getLangOpts().OpenCL = 1;
	compiler.getLangOpts().Bool = 1;

	llvm::IntrusiveRefCntPtr<TargetOptions> targetOptions(new TargetOptions());
	targetOptions->Triple = llvm::sys::getDefaultTargetTriple();
	TargetInfo *targetInfo = TargetInfo::CreateTargetInfo(
	        compiler.getDiagnostics(), targetOptions.getPtr());
	compiler.setTarget(targetInfo);

	compiler.createFileManager();
	compiler.createSourceManager(compiler.getFileManager());
	compiler.createPreprocessor();

	/* Load the AST Consumer to analyse the kernel code */
	// TODO: Use Informations like CL_MEM_READ_ONLY and CL_MEM_WRITE_ONLY from
	//       the host code
	compiler.setASTConsumer(new KernelConsumer(kernelInfo, warnings));
	compiler.createASTContext();

	/* Load the kernel code */
	time_t current_time = time(NULL);
	StringRef kernelCodeRef = StringRef(kernelCode);
	const llvm::MemoryBuffer *kernelCodeMemoryBuffer =
	        llvm::MemoryBuffer::getMemBuffer(kernelCodeRef);
	const FileEntry *file = compiler.getFileManager().getVirtualFile(
	        "kernel.cl", kernelCodeMemoryBuffer->getBufferSize(), current_time);
	compiler.getSourceManager().overrideFileContents(file,
	                                                 kernelCodeMemoryBuffer,
	                                                 true);

	compiler.getSourceManager().createMainFileID(file);

	compiler.getDiagnosticClient().BeginSourceFile(compiler.getLangOpts(),
	                                               &compiler.getPreprocessor());
	compiler.getDiagnostics().setSuppressAllDiagnostics(suppressAllDiagnostics);
#ifdef DEBUG
	compiler.getDiagnosticOpts().ShowColors = 1;
#endif /* DEBUG */

	run();
}

Analyse::~Analyse() {
	compiler.getDiagnosticClient().EndSourceFile();
}

KernelInfo* Analyse::getKernelInfo() {
	return kernelInfo;
}

clang::SourceManager& Analyse::getSourceManager() {
	return compiler.getSourceManager();
}

void Analyse::run() {
	ParseAST(compiler.getPreprocessor(), &compiler.getASTConsumer(),
	         compiler.getASTContext());
}
