/*
 * KernelBuiltinFunctions.h
 *
 *  Created on: 16.12.2013
 *      Author: Moritz Lüdecke
 */

#ifndef KERNELBUILTINFUNCTIONS_H_
#define KERNELBUILTINFUNCTIONS_H_

#include <vector>

#include <llvm/ADT/StringRef.h>
#include <clang/Basic/IdentifierTable.h>

#include <src/Operation.h>
#include <src/KernelSetting.h>
#include <src/Warnings.h>

class KernelBuiltinFunctions {
public:
	KernelBuiltinFunctions(KernelSetting* Setting, Warnings* warnings);

	// Add a new Work-Item Built-in Function.
	// Returns a pointer to the added value.
	// If there was no function called <name> then it returns NULL.
	size_t& addFunction(const llvm::StringRef name);

	// Add the parameter value for the pending Work-Item Built-in Function
	// Returns false, if there was no function which have to be updated,
	// otherwise true.
	bool pushIndex(Parameter* parameter);

	// Updates the parameter values on the new kernel setting
	// Returns false, if the value of valid is false and the progress canceled,
	// otherwise true
	bool updateValues();

private:
	Warnings* warnings;

	KernelSetting* Setting;
	// Calculation is a function pointer
	typedef size_t (KernelBuiltinFunctions::*calculationFunction)(uint);
	void updateFunction(std::vector<size_t*>* list,
	                    std::vector<Parameter*>* index,
	                    calculationFunction calculation);

	size_t get_global_size(uint dim);
	size_t get_global_id(uint dim);
	size_t get_local_size(uint dim);
	size_t get_enqueued_local_size(uint dim);
	size_t get_local_id(uint dim);
	size_t get_num_groups(uint dim);
	size_t get_group_id(uint dim);
	size_t get_global_offset(uint dim);
	size_t get_global_linear_id();
	size_t get_local_linear_id();

	// Use for better semantics.
	// True if vector size of each built-in function is the same as the
	// correspond index vector size of each, otherwise false.
	bool valid;

	// Work-Item Built-in Functions of the OpenCL Kernel
	uint* work_dim;                            // Number of dimensions in use
	std::vector<size_t*> global_size;          // Number of global work-items
	std::vector<size_t*> global_id;            // Global work-item ID value
	std::vector<size_t*> local_size;           // Number of local work-items if
	                                           // kernel executed with uniform
	                                           // work-group size
	std::vector<size_t*> enqueued_local_size;  // Number of local work-items
	std::vector<size_t*> local_id;             // Local work-item ID
	std::vector<size_t*> num_groups;           // Number of work-groups
	std::vector<size_t*> group_id;             // Work-group ID
	std::vector<size_t*> global_offset;        // Global offset
	size_t* global_linear_id;                  // Work-items 1-dimensional
	                                           // global ID
	size_t* local_linear_id;                   // Work-items 1-dimensional
	                                           // local ID

	// Sub-groups (require the cl_khr_subgroups extension.)
	// TODO: This functions are currently unused
	uint* sub_group_size;      // Number of work-items in the subgroup
	uint* max_sub_group_size;  // Maximum size of a subgroup
	uint* num_sub_groups;      // Number of subgroups
	uint* sub_group_id;        // Sub-group ID
	uint* sub_group_local_id;  // Unique work-item ID

	// Holds the parameter of get_global_id. We need this to determine the value
	// of get_global_id.
	// For example the integer value X in get_global_id(X)
	std::vector<Parameter*> global_id_index;
	// Same as get_global_id_index
	std::vector<Parameter*> global_size_index;
	std::vector<Parameter*> local_size_index;
	std::vector<Parameter*> enqueued_local_size_index;
	std::vector<Parameter*> local_id_index;
	std::vector<Parameter*> num_groups_index;
	std::vector<Parameter*> group_id_index;
	std::vector<Parameter*> global_offset_index;
};

#endif /* KERNELBUILTINFUNCTIONS_H_ */
