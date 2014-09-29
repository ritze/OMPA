/*
 * KernelASTVisitor.cpp
 *
 *  Created on: 07.12.2013
 *      Author: Moritz Lüdecke
 */

#include <stdio.h>
#include <string>

#include <clang/AST/Attr.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/DeclGroup.h>
#include <clang/Lex/Lexer.h>

#include "Context.h"
#include "KernelASTVisitor.h"
#include "KernelInfo.h"

using namespace clang;

//TODO: Doesn't work. Why?
//bool KernelASTVisitor::TraverseBinaryOperator(BinaryOperator *E) {
////	if (E->isCompoundAssignmentOp())
////		thisContext = CF_Write | CF_Read;
////	else
//	return true;
//}

//TODO: This Works, but could not used severally.
//bool KernelASTVisitor::TraverseBinAddAssign(CompoundAssignOperator *S) {
//	{
//		thisContext = CF_Write | CF_Read;
//		TraverseStmt(S->getLHS());
//	}
//	{
//		thisContext = CF_Read;
//		TraverseStmt(S->getRHS());
//	}
//
//	return true;
//}

//TODO: Doesn't works. Why?
//bool KernelASTVisitor::TraverseCompoundAssignOperator(CompoundAssignOperator *S) {
//	{
//		llvm::outs() << "1\n";
//		thisContext = CF_Write;// | CF_Read;
//		TraverseStmt(S->getLHS());
//	}
//	{
//		llvm::outs() << "2\n";
//		thisContext = CF_Read;
//		TraverseStmt(S->getRHS());
//	}
//
//	return false;
//}

bool KernelASTVisitor::TraverseArraySubscriptExpr(ArraySubscriptExpr *E) {
	{
		parentContext.push(thisContext);

		thisContext |= CF_Array;
		TraverseStmt(E->getBase());

		thisContext = parentContext.top();
		parentContext.pop();
	}
	{
		parentContext.push(thisContext);

		/* The index of an array is read */
		thisContext &= ~CF_Write;
		thisContext |= CF_Read;
		thisContext |= CF_Index;
		TraverseStmt(E->getIdx());

		thisContext = parentContext.top();
		parentContext.pop();
	}

	return true;
}

bool KernelASTVisitor::TraverseBinAssign(BinaryOperator *S) {
	kernelInfo->getCurrentFunction()->updateVarDeclOp(clang::BO_Assign);
	{
		thisContext = CF_Write;
		TraverseStmt(S->getLHS());
	}
	{
		thisContext = CF_Read;
		TraverseStmt(S->getRHS());
	}

	return true;
}

bool KernelASTVisitor::TraverseCallExpr(CallExpr *S) {
	{
		TraverseStmt(S->getCallee());
	}
	{
		parentContext.push(thisContext);
		thisContext |= CF_Parm;

		for (uint i = 0; i < S->getNumArgs(); ++i) {
			TraverseStmt(S->getArg(i));
		}

		thisContext = parentContext.top();
		parentContext.pop();
	}

	return true;
}

// TODO: Implement ConditionalOperator
// XXX: This is a workaround!
bool KernelASTVisitor::TraverseConditionalOperator(
        clang::ConditionalOperator *S) {

	warnings->ConditionalOperator();

	TraverseStmt(S->getTrueExpr());

	parentContext.push(thisContext);
	thisContext &= ~CF_Write;
	thisContext |= CF_Read;

	TraverseStmt(S->getFalseExpr());
	TraverseStmt(S->getCond());

	thisContext = parentContext.top();
	parentContext.pop();

	return true;
}

// FIXME: This method doesn't work. So we have to do this through a complicated
//        workaround: We don't flag de- and incrementation with CF_Write.
//bool KernelASTVisitor::TraverseUnaryOperator(UnaryOperator *S) {
//	S->dumpColor();
//	thisContext |= CF_Read;
//
//	// TODO: Test this
//	if (S->isPostfix() || S->isPrefix())
//		thisContext |= CF_Write;
//
//	TraverseStmt(S->getSubExpr());
//
//	return true;
//}

// TODO: Implement Loops
bool KernelASTVisitor::TraverseDoStmt(clang::DoStmt *S) {
	warnings->DoStmt();

	return true;
}

// TODO: Implement Loops
bool KernelASTVisitor::TraverseForStmt(clang::ForStmt *S) {
	warnings->ForStmt();

	return true;
}

// TODO: Implement if statements
bool KernelASTVisitor::TraverseIfStmt(clang::IfStmt *S) {
	warnings->IfStmt();

	return true;
}

bool KernelASTVisitor::TraverseVarDecl(VarDecl *D) {
	parentContext.push(thisContext);

	ASTContext& DAC = D->getASTContext();
	const ConstantArrayType* AT = DAC.getAsConstantArrayType(D->getType());

	if (AT != NULL) {
		thisContext |= CF_Array;
	}

	base::TraverseVarDecl(D);

	thisContext = parentContext.top();
	parentContext.pop();

	return true;
}

// TODO: Implement Loops
bool KernelASTVisitor::TraverseWhileStmt(clang::WhileStmt *S) {
	warnings->WhileStmt();

	return true;
}

bool KernelASTVisitor::VisitBinaryOperator(BinaryOperator *S) {
	// FIXME: Bad style. This code block have to work with
	//        TraverseBinaryOperator or TraverseCompoundAssignOperator.
	//        But it doesn't. Why?
	thisContext |= CF_Read;

	if (S->isCompoundAssignmentOp()) {
		parentContext.push(thisContext);

		thisContext = CF_Write;
		TraverseStmt(S->getLHS());

		thisContext = parentContext.top();
		parentContext.pop();
	} /* End of FIXME */

	kernelInfo->getCurrentFunction()->updateVarDeclOp(S->getOpcode(), false,
	                                                  thisContext);

	return true;
}

// TODO: Implement functions
bool KernelASTVisitor::VisitCallExpr(CallExpr *S) {
	// Some code snippets:
//	llvm::outs() << S->getCallee()->getStmtClassName() << " getNumArgs = " << S->getNumArgs() << "\n";
//	S->getCallee()->children().first->dumpColor();
//	if (S->getCallee()) ...

	return true;
}

bool KernelASTVisitor::VisitDeclRefExpr(DeclRefExpr *E) {
	ValueDecl* D = E->getDecl();
	IdentifierInfo* ID = D->getIdentifier();

	if (D->getKind() == Decl::ParmVar) {
		if (thisContext & CF_Write) {
			if (thisContext & CF_Array) {
				kernelInfo->getCurrentFunction()->openArray(D->getName(), true,
				                                            true);

			} else if (thisContext & CF_Index) {
				kernelInfo->getCurrentFunction()->setIndexVar(ID, true);

			} else {
				kernelInfo->getCurrentFunction()->addParmVarDecl(ID);
			}
		}

		if (thisContext & CF_Read) {
			if (thisContext & CF_Parm) {
				kernelInfo->getCurrentFunction()->setParmVar(ID, true);

			} else if (thisContext & CF_Array) {
				// FIXME: Handle this situation right
				// int array[] = {1, var, array2[3], 3, 5};
				if (thisContext & CF_ArrayInit) {
					kernelInfo->getCurrentFunction()->updateArrayDecl(ID, true);
				}

				kernelInfo->getCurrentFunction()->openArray(D->getName(), false,
				                                            true);

			} else if (thisContext & CF_Index) {
				kernelInfo->getCurrentFunction()->setIndexVar(ID, true);

			} else {
				kernelInfo->getCurrentFunction()->updateParmVarDecl(ID);
			}
		}

	} else {
		if (thisContext & CF_Write) {
			if (thisContext & CF_Array) {
				kernelInfo->getCurrentFunction()->openArray(D->getName(), true);

			} else {
				kernelInfo->getCurrentFunction()->addVarDecl(ID);
			}
		}

		if (D->getKind() == Decl::Function) {
			kernelInfo->getCurrentFunction()->updateVarDecl(ID, true);

		} else if (thisContext & CF_Read) {
			if (thisContext & CF_Parm) {
				kernelInfo->getCurrentFunction()->setParmVar(ID);

			} else if (thisContext & CF_Array) {
				// FIXME: Handle this situation right
				// int array[] = {1, var, array2[3], 3, 5};
				if (thisContext & CF_ArrayInit) {
					kernelInfo->getCurrentFunction()->updateArrayDecl(ID);
				}

				kernelInfo->getCurrentFunction()->openArray(D->getName(),
				                                            false);

			} else if (thisContext & CF_Index) {
				kernelInfo->getCurrentFunction()->setIndexVar(ID);

			} else {
				kernelInfo->getCurrentFunction()->updateVarDecl(ID);
			}
		}
	}

//	if (thisContext & CF_Parm)
//		//TODO: Add setParmVar(clang::Identifier)
//		kernelInfo.getCurrentFunction()->setParmVar(E->getIdentifier());

	return true;
}

// XXX: floats are not Supported. This is a workaround
bool KernelASTVisitor::VisitFloatingLiteral(clang::FloatingLiteral *S) {
	int value = (int) S->getValueAsApproximateDouble();

	if (thisContext & CF_Parm) {
		kernelInfo->getCurrentFunction()->setParmVar(value);

	} else if (thisContext & CF_Array) {
		kernelInfo->getCurrentFunction()->updateArrayDecl(value);

	} else if (thisContext & CF_Index) {
		kernelInfo->getCurrentFunction()->setIndexVar(value);

	} else {
		kernelInfo->getCurrentFunction()->updateVarDecl(value);
	}

	return true;
}

bool KernelASTVisitor::VisitFunctionDecl(FunctionDecl *D) {
	if (!D->hasAttr<OpenCLKernelAttr>()) {
		return false;
	}

	kernelInfo->addFunction(*D);

	return true;
}

bool KernelASTVisitor::VisitIntegerLiteral(IntegerLiteral *S) {
	int value = S->getValue().getLimitedValue();

	if (thisContext & CF_Parm) {
		kernelInfo->getCurrentFunction()->setParmVar(value);

	} else if (thisContext & CF_Array) {
		kernelInfo->getCurrentFunction()->updateArrayDecl(value);

	} else if (thisContext & CF_Index) {
		kernelInfo->getCurrentFunction()->setIndexVar(value);

	} else {
		kernelInfo->getCurrentFunction()->updateVarDecl(value);
	}

	return true;
}

// TODO: Implement structs
bool KernelASTVisitor::VisitMemberExpr(clang::MemberExpr *E) {

	// At the moment a struct member returns always 0
	if (thisContext & CF_Read) {
		kernelInfo->getCurrentFunction()->updateVarDecl(0);
	}

	return true;
}

bool KernelASTVisitor::VisitUnaryOperator(UnaryOperator *S) {
//	// FIXME: Bad style. This code block have to work with
//	//        TraverseUnaryOperator. But it doesn't. Why?
//	if (S->isPrefix()) {
//		kernelInfo.getCurrentFunction()->updateVarDeclOp(S->getOpcode(), true);
//		parentContext.push(thisContext);
//		thisContext |= CF_Write;
//		TraverseStmt(S->getSubExpr());
//		thisContext = parentContext.top();
//		parentContext.pop();
//
//		return true;
//	}
//
//	if (S->isPostfix()) {
//		TraverseStmt(S->getSubExpr());
//
//		kernelInfo.getCurrentFunction()->updateVarDeclOp(S->getOpcode(), true);
//		parentContext.push(thisContext);
//		thisContext |= CF_Write;
//		TraverseStmt(S->getSubExpr());
//		thisContext = parentContext.top();
//		parentContext.pop();
//
//		return false;
//	}
//
//	return true;

	thisContext |= CF_Read;

	uint opcode = S->getOpcode();

	// We ignore AddrOf and Pointer
	if (opcode != clang::UO_AddrOf && opcode != clang::UO_Deref) {
		kernelInfo->getCurrentFunction()->updateVarDeclOp(opcode, true,
		                                                  thisContext);
	}

	// FIXME: Bad style. This code block have to work with
	//        TraverseUnaryOperator. But it doesn't. Why?
	if (S->isPrefix() || S->isPostfix()) {
		parentContext.push(thisContext);

		thisContext &= ~CF_Read;
		thisContext |= CF_Write;
		TraverseStmt(S->getSubExpr());

		thisContext = parentContext.top();
		parentContext.pop();
	}

	return true;
}

// TODO: Implement structs
bool KernelASTVisitor::VisitRecordDecl(RecordDecl *D) {
	warnings->RecordDecl();

	// At the moment structs are ignored.
	return false;
}

bool KernelASTVisitor::VisitVarDecl(VarDecl *D) {
	const IdentifierInfo* ID = D->getIdentifier();

	if (D->getKind() == Decl::ParmVar) {
		kernelInfo->getCurrentFunction()->addParmDecls(ID);

	} else if (thisContext & CF_Array) {
		ASTContext& DAC = D->getASTContext();
		const ConstantArrayType* AT = DAC.getAsConstantArrayType(D->getType());
		kernelInfo->getCurrentFunction()->addArrayDecl(
		        ID, DAC.getConstantArrayElementCount(AT), !D->hasInit());

	} else {
		kernelInfo->getCurrentFunction()->addVarDecl(ID, D->hasInit());
	}

//	thisContext &= ~CF_Write;
	thisContext |= CF_Read;
	thisContext |= CF_ArrayInit;

	return true;
}
