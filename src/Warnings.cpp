/*
 * Warnings.cpp
 *
 *  Created on: 12.06.2013
 *      Author: Moritz Lüdecke
 */

#include <iostream>

#include "Warnings.h"

Warnings::Warnings() :
		showedWarnings(WF_none) {
};

bool Warnings::get_enqueued_local_size() {
	if (showedWarnings & WF_enqueued_local_size) {
		return false;
	}

	showedWarnings |= WF_enqueued_local_size;
	std::cout << "WARNING: size_t get_enqueued_local_size(uint dim)"
	          << " could return the wrong value!\n";

	return true;
}

bool Warnings::get_group_id() {
	if (showedWarnings & WF_group_id) {
		return false;
	}

	showedWarnings |= WF_group_id;
	std::cout << "NOTE: size_t get_group_id(uint dim)"
	          << " could return the wrong value!\n";

	return true;
}

bool Warnings::get_local_id() {
	if (showedWarnings & WF_local_id) {
		return false;
	}

	showedWarnings |= WF_local_id;
	std::cout << "NOTE: size_t get_local_id(uint dim)"
	          << " could return the wrong value!\n";

	return true;
}

bool Warnings::get_local_size() {
	if (showedWarnings & WF_local_size) {
		return false;
	}

	showedWarnings |= WF_local_size;
	std::cout << "WARNING: local_work_size is NULL; "
	          << "size_t get_local_size(uint dim) returns the wrong value!\n";

	return true;
}

bool Warnings::get_num_groups() {
	if (showedWarnings & WF_num_groups) {
		return false;
	}

	showedWarnings |= WF_num_groups;
	std::cout << "WARNING: dim is 0 at size_t get_num_groups(uint dim) is 0"
	          << "; return 1!\n";

	return true;
}

bool Warnings::ConditionalOperator() {
	if (showedWarnings & WF_conditional_operator) {
		return false;
	}

	showedWarnings |= WF_conditional_operator;
	std::cout << "WARNING: Conditional operator are not implemented yet!\n";

	return true;
}

bool Warnings::IfStmt() {
	if (showedWarnings & WF_if_statement) {
		return false;
	}

	showedWarnings |= WF_if_statement;
	std::cout << "WARNING: If statements are not implemented yet!\n";

	return true;
}

bool Warnings::RecordDecl() {
	if (showedWarnings & WF_record_decl) {
		return false;
	}

	showedWarnings |= WF_record_decl;
	std::cout << "WARNING: Structs are not implemented yet!\n";

	return true;
}

bool Warnings::DoStmt() {
	if (showedWarnings & WF_do_statement) {
		return false;
	}

	showedWarnings |= WF_do_statement;
	std::cout << "WARNING: Do loops are not implemented yet!\n";

	return true;
}

bool Warnings::ForStmt() {
	if (showedWarnings & WF_for_statement) {
		return false;
	}

	showedWarnings |= WF_for_statement;
	std::cout << "WARNING: For loops are not implemented yet!\n";

	return true;
}

bool Warnings::WhileStmt() {
	if (showedWarnings & WF_while_statement) {
		return false;
	}

	showedWarnings |= WF_while_statement;
	std::cout << "WARNING: While loops are not implemented yet!\n";

	return true;
}

