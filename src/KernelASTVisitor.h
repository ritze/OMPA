/*
 * KernelASTVisitor.h
 *
 *  Created on: 07.12.2013
 *      Author: Moritz Lüdecke
 */

#ifndef KERNELASTVISITOR_H_
#define KERNELASTVISITOR_H_

#include <stack>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Decl.h>
#include <clang/Basic/SourceManager.h>

#include "Context.h"
#include "KernelInfo.h"
#include "Warnings.h"

class KernelASTVisitor: public clang::RecursiveASTVisitor<KernelASTVisitor> {
public:
	KernelASTVisitor(KernelInfo* kernelInfo, Warnings* warnings) :
			kernelInfo(kernelInfo), warnings(warnings), thisContext(0) {
	};

//	bool TraverseBinaryOperator(clang::BinaryOperator *S);
//	bool TraverseBinAddAssign(clang::CompoundAssignOperator *S);
//	bool TraverseCompoundAssignOperator(clang::CompoundAssignOperator *S);
	bool TraverseArraySubscriptExpr(clang::ArraySubscriptExpr *E);
	bool TraverseBinAssign(clang::BinaryOperator *S);
	bool TraverseCallExpr(clang::CallExpr *S);
	// TODO: Implement Conditional Operator
	bool TraverseConditionalOperator(clang::ConditionalOperator *S);
	// TODO: Implement Loops
	bool TraverseDoStmt(clang::DoStmt *S);
	// TODO: Implement Loops
	bool TraverseForStmt(clang::ForStmt *S);
	// TODO: Implement if statements
	bool TraverseIfStmt(clang::IfStmt *S);
//	bool TraverseUnaryOperator(clang::UnaryOperator *S);
	bool TraverseVarDecl(clang::VarDecl *D);
	// TODO: Implement Loops
	bool TraverseWhileStmt(clang::WhileStmt *S);
	bool VisitBinaryOperator(clang::BinaryOperator *S);
	// TODO: Implement functions
	bool VisitCallExpr(clang::CallExpr *S);
	bool VisitDeclRefExpr(clang::DeclRefExpr *E);
	bool VisitFloatingLiteral(clang::FloatingLiteral *S);
	bool VisitFunctionDecl(clang::FunctionDecl *D);
	bool VisitIntegerLiteral(clang::IntegerLiteral *S);
	// TODO: Implement structs
	bool VisitMemberExpr(clang::MemberExpr *E);
	bool VisitUnaryOperator(clang::UnaryOperator *S);
	// TODO: Implement structs
	bool VisitRecordDecl(clang::RecordDecl *D);
	bool VisitVarDecl(clang::VarDecl *D);

private:
	typedef clang::RecursiveASTVisitor<KernelASTVisitor> base;

	std::stack<Context> parentContext;
	Context thisContext;
	KernelInfo* kernelInfo;
	Warnings* warnings;
};

#endif /* KERNELASTVISITOR_H_ */
