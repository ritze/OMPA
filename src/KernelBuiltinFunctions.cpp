/*
 * KernelBuiltinFunctions.cpp
 *
 *  Created on: 16.12.2013
 *      Author: Moritz Lüdecke
 */

#include <iostream>

#include "KernelBuiltinFunctions.h"

static size_t UNDEFINED = 0;

KernelBuiltinFunctions::KernelBuiltinFunctions(KernelSetting* Setting,
                                               Warnings* warnings) :
		Setting(Setting), valid(true), work_dim(&Setting->work_dim),
		global_linear_id(new size_t(UNDEFINED)),
		local_linear_id(new size_t(UNDEFINED)),
		sub_group_size(new uint(UNDEFINED)),
		max_sub_group_size(new uint(UNDEFINED)),
		num_sub_groups(new uint(UNDEFINED)), sub_group_id(new uint(UNDEFINED)),
		sub_group_local_id(new uint(UNDEFINED)), warnings(warnings) {
};

size_t& KernelBuiltinFunctions::addFunction(const llvm::StringRef name) {
	std::vector<size_t*>* list;
	size_t* value = &UNDEFINED;

	if (name.equals("get_global_size")) {
		list = &global_size;

	} else if (name.equals("get_global_id")) {
		list = &global_id;

	} else if (name.equals("get_local_size")) {
		list = &local_size;

	} else if (name.equals("get_enqueued_local_size")) {
		list = &enqueued_local_size;

	} else if (name.equals("get_local_id")) {
		list = &local_id;

	} else if (name.equals("get_num_groups")) {
		list = &num_groups;

	} else if (name.equals("get_group_id")) {
		list = &group_id;

	} else if (name.equals("get_global_offset")) {
		list = &global_offset;

	} else if (name.equals("get_work_dim")) {
		value = (size_t*) work_dim;

	} else if (name.equals("get_global_linear_id")) {
		value = global_linear_id;

	} else if (name.equals("get_local_linear_id")) {
		value = local_linear_id;

	} else if (name.equals("get_sub_group_size")) {
		value = (size_t*) sub_group_size;

	} else if (name.equals("get_max_sub_group_size")) {
		value = (size_t*) max_sub_group_size;

	} else if (name.equals("get_num_sub_groups")) {
		value = (size_t*) num_sub_groups;

	} else if (name.equals("get_sub_group_id")) {
		value = (size_t*) sub_group_id;

	} else if (name.equals("get_sub_group_local_id")) {
		value = (size_t*) sub_group_local_id;
	}

	/* Check if the function is a OpenCL Kernel Work-Item Built-in Function */
	if (list != NULL) {
		valid = false;
		list->push_back(new size_t(UNDEFINED));

		return *list->back();
	}

	return *value;
}

bool KernelBuiltinFunctions::pushIndex(Parameter* parameter) {
	if (valid) {
		return false;
	}

	if (global_size.size() != global_size_index.size()) {
		global_size_index.push_back(parameter);

	} else if (global_id.size() != global_id_index.size()) {
		global_id_index.push_back(parameter);

	} else if (local_size.size() != local_size_index.size()) {
		local_size_index.push_back(parameter);

	} else if (enqueued_local_size.size() != enqueued_local_size_index.size()) {
		enqueued_local_size_index.push_back(parameter);

	} else if (local_id.size() != local_id_index.size()) {
		local_id_index.push_back(parameter);

	} else if (num_groups.size() != num_groups_index.size()) {
		num_groups_index.push_back(parameter);

	} else if (group_id.size() != group_id_index.size()) {
		group_id_index.push_back(parameter);

	} else if (global_offset.size() != global_offset_index.size()) {
		global_offset_index.push_back(parameter);
	}

	return (valid = true);
}

bool KernelBuiltinFunctions::updateValues() {
	if (!valid) {
		return false;
	}

	updateFunction(
	        &global_size, &global_size_index,
	        (calculationFunction) &KernelBuiltinFunctions::get_global_size);
	updateFunction(
	        &global_id, &global_id_index,
	        (calculationFunction) &KernelBuiltinFunctions::get_global_id);
	updateFunction(
	        &local_size, &local_size_index,
	        (calculationFunction) &KernelBuiltinFunctions::get_local_size);
	updateFunction(
	        &enqueued_local_size,
	        &enqueued_local_size_index,
	        (calculationFunction) &KernelBuiltinFunctions::get_enqueued_local_size);
	updateFunction(&local_id, &local_id_index,
	               (calculationFunction) &KernelBuiltinFunctions::get_local_id);
	updateFunction(
	        &num_groups, &num_groups_index,
	        (calculationFunction) &KernelBuiltinFunctions::get_num_groups);
	updateFunction(&group_id, &group_id_index,
	               (calculationFunction) &KernelBuiltinFunctions::get_group_id);
	updateFunction(
	        &global_offset, &global_offset_index,
	        (calculationFunction) &KernelBuiltinFunctions::get_global_offset);

	*global_linear_id = get_global_linear_id();
	*local_linear_id = get_local_linear_id();

	// TODO: This functions are currently unused
//	sub_group_size = UNDEFINED;
//	max_sub_group_size = UNDEFINED;
//	num_sub_groups = UNDEFINED;
//	sub_group_id = UNDEFINED;
//	sub_group_local_id = UNDEFINED;

	return true;
}

void KernelBuiltinFunctions::updateFunction(std::vector<size_t*>* list,
                                            std::vector<Parameter*>* index,
                                            calculationFunction calculation) {

	for (int i = 0; i < list->size(); ++i) {
		int value = (*index)[i]->getValue();
		*(*list)[i] = (this->*calculation)(value);
	}
}

size_t KernelBuiltinFunctions::get_global_size(uint dim) {
	if (Setting->clEnqueueTask || dim >= Setting->work_dim) {
		return 1;
	}

	return get_num_groups(dim) * get_local_size(dim);
}

size_t KernelBuiltinFunctions::get_global_id(uint dim) {
	if (dim >= Setting->work_dim) {
		return 0;
	}

	return get_group_id(dim) * get_local_size(dim) + get_local_id(dim);
}

size_t KernelBuiltinFunctions::get_local_size(uint dim) {
	if (dim >= Setting->work_dim) {
		return 1;

	} else if (Setting->local_work_size == 0) {
		// TODO:
		// ...otherwise the OpenCL implementation chooses an appropriate
		// local_work_size value which is returned by this function. If the
		// kernel is executed with a non-uniform work-group size 28, calls to
		// this built-in from some work-groups may return different values than
		// calls to this built-in from other work-groups.

		warnings->get_local_size();
		return 0;
	}

	return Setting->local_work_size;
}

size_t KernelBuiltinFunctions::get_enqueued_local_size(uint dim) {
	if (dim >= Setting->work_dim) {
		return 1;

	} else {
		// TODO:
		// Returns the same value as that returned by get_local_size(dim) if the
		// kernel is executed with a uniform work-group size.
		//
		// If the kernel is executed with a non-uniform work-group size, returns
		// the number of local work-items in each of the work-groups that make
		// up the uniform region of the global range in the dimension identified
		// by dim. If the local_work_size argument to clEnqueueNDRangeKernel is
		// not NULL, this value will match the value specified in
		// local_work_size[dim]. If local_work_size is NULL, this value will
		// match the local size that the implementation determined would be most
		// efficient at implementing the uniform region of the global range.

		warnings->get_enqueued_local_size();
		return get_local_size(dim);
	}
}

size_t KernelBuiltinFunctions::get_local_id(uint dim) {
	if (dim >= Setting->work_dim) {
		return 0;
	}

	// TODO:
	// Returns the unique local work-item ID i.e. a work-item within a
	// specific work-group for dimension identified by dim.

	warnings->get_local_id();
	return Setting->local_work_id;
}

size_t KernelBuiltinFunctions::get_num_groups(uint dim) {
	if (dim >= Setting->work_dim) {
		return 1;

	} else if (dim == 0) {
		warnings->get_num_groups();
		return 1;
	}

	// TODO;
	// Returns the number of work-groups that will execute a kernel for
	// dimension identified by dim.
	return Setting->global_work_size / get_local_size(dim);
}

size_t KernelBuiltinFunctions::get_group_id(uint dim) {
	if (dim >= Setting->work_dim) {
		return 0;
	}

	// TODO:
	// get_group_id returns the work-group ID which is a number
	// from 0 .. get_num_groups(dim) - 1.

	warnings->get_group_id();
	return Setting->group_id;
}

size_t KernelBuiltinFunctions::get_global_offset(uint dim) {
	if (dim >= Setting->work_dim) {
		return 0;
	}

	return Setting->global_work_offset;
}

size_t KernelBuiltinFunctions::get_global_linear_id() {
	size_t id;

	if (Setting->work_dim == 1) {
		id = get_global_id(0) - get_global_offset(0);

	} else if (Setting->work_dim == 2) {
		id = (get_global_id(1) - get_global_offset(1)) * get_global_size(0)
		        + (get_global_id(0) - get_global_offset(0));

	} else {
		id = ((get_global_id(2) - get_global_offset(2)) * get_global_size(1)
		        * get_global_size(0))
		        + ((get_global_id(1) - get_global_offset(1))
		                * get_global_size(0))
		        + (get_global_id(0) - get_global_offset(0));
	}

	return id;
}

size_t KernelBuiltinFunctions::get_local_linear_id() {
	size_t id;

	if (Setting->work_dim == 1) {
		id = get_local_id(0);

	} else if (Setting->work_dim == 2) {
		id = get_local_id(1) * get_local_size(0) + get_local_id(0);

	} else {
		id = (get_local_id(2) * get_local_size(1) * get_local_size(0))
		        + (get_local_id(1) * get_local_size(0)) + get_local_id(0);
	}

	return id;
}
