/*
 * Warnings.h
 *
 *  Created on: 12.06.2013
 *      Author: Moritz Lüdecke
 */

#ifndef WARNINGS_H_
#define WARNINGS_H_

// We only want show warning messages once
enum WarningFlags {
	WF_none                 =    0,
	WF_enqueued_local_size  =    1,
	WF_group_id             =    2,
	WF_local_id             =    4,
	WF_local_size           =    8,
	WF_num_groups           =   16,
	WF_conditional_operator =   32,
	WF_if_statement         =   64,
	WF_record_decl          =  128,
	WF_do_statement         =  256,
	WF_for_statement        =  512,
	WF_while_statement      = 1024,

};

typedef unsigned int ShowedWarnings;

class Warnings {
public:
	Warnings();

	bool get_enqueued_local_size();
	bool get_group_id();
	bool get_local_id();
	bool get_local_size();
	bool get_num_groups();

	bool ConditionalOperator();
	bool IfStmt();
	// Structs
	bool RecordDecl();
	bool DoStmt();
	bool ForStmt();
	bool WhileStmt();

private:
	ShowedWarnings showedWarnings;
};

#endif /* WARNINGS_H_ */
