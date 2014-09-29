/*
 * KernelInfo.cpp
 *
 *  Created on: 07.12.2013
 *      Author: Moritz Lüdecke
 */

#include "KernelInfo.h"

KernelInfo::KernelInfo(Warnings* warnings) :
		Setting(new KernelSetting) {

	BuiltinFunctions = new KernelBuiltinFunctions(Setting, warnings);
}

uint KernelInfo::getFunctionSize() {
	return Functions.size();
}

FunctionPattern* KernelInfo::getFunction(uint index) {
	if (index >= Functions.size()) {
		return NULL;
	}

	const char* functionName = Functions[index].getDecl().getName().data();
	ParameterPatternList* globalParameters = buildGlobalParametersList(index);

	return new FunctionPattern(functionName, globalParameters);
}

std::vector<KernelFunction> KernelInfo::getFunctions() {
	return Functions;
}

// TODO: Split this in two helper classes
ParameterPatternList* KernelInfo::buildLocalPattern(const uint index) {
	ParameterPatternList* parameters = new ParameterPatternList;
	IDList IDs = Functions[index].getParmDecls();
	VariablesList variables = Functions[index].getParmVarDecls();
	ArraysList arrays = Functions[index].getParmArrayDecls();

	uint i = 0;
	for (IDList::iterator it = IDs.begin(); it != IDs.end(); ++it, ++i) {
		ParameterPattern* parameter = new ParameterPattern(NULL);
		parameters->push_back(parameter);

		bool found = false;

		// Build Variables
		bool read = false;
		bool write = false;

		for (VariablesList::const_iterator jt = variables.begin();
		        jt != variables.end() && !read && !write; ++jt) {

			if ((*it) == (*jt)->ID) {
				found = true;

				if (!read && !write) {
					(*parameters)[i]->type = PatternType_Variable;
					(*parameters)[i]->reads = new bool[1];
					(*parameters)[i]->reads[0] = false;
					(*parameters)[i]->readsSize = 1;
					(*parameters)[i]->writes = new bool[1];
					(*parameters)[i]->writes[0] = false;
					(*parameters)[i]->writesSize = 1;
				}

				if ((*jt)->accessMode & AF_Read) {
					read = true;
					(*parameters)[i]->reads[0] = true;
				}

				if ((*jt)->accessMode & AF_Write) {
					write = true;
					(*parameters)[i]->writes[0] = true;
				}
			}
		}

		// Build Arrays
		for (ArraysList::iterator jt = arrays.begin();
		        jt != arrays.end() && !found; ++jt) {

			if ((*it) == (*jt)->ID) {
				found = true;

				AccessModesList accessModes = (*jt)->accessModes;
				SortedAccessModesList sortedAccessModes;

				uint readsSize = 0;
				uint writesSize = 0;
				for (AccessModesList::iterator kt = accessModes.begin();
				        kt != accessModes.end(); ++kt) {

					// Negative index values are not helpful
					int intIndex = (*kt)->index->getValue();
					if (intIndex < 0) {
						intIndex = 0;
					}

					uint index = intIndex;
					std::pair<bool, bool> rights;
					bool hasReadAccess = ((*kt)->accessMode & AF_Read);
					bool hasWriteAccess = ((*kt)->accessMode & AF_Write);

					if (readsSize <= index && hasReadAccess) {
						readsSize = index + 1;
					}

					if (writesSize <= index && hasWriteAccess) {
						writesSize = index + 1;
					}

					rights = sortedAccessModes[index];
					rights.first |= hasReadAccess;
					rights.second |= hasWriteAccess;
					sortedAccessModes[index] = rights;
				}

				parameters->at(i)->type = PatternType_Array;
				parameters->at(i)->reads = new bool[readsSize];
				parameters->at(i)->readsSize = readsSize;
				parameters->at(i)->writes = new bool[writesSize];
				parameters->at(i)->writesSize = writesSize;

				// Fill array for read accesses
				SortedAccessModesList::iterator kt = sortedAccessModes.begin();
				for (uint j = 0; j < readsSize; ++j) {
					parameters->at(i)->reads[j] = 0;

					if (j == (*kt).first) {
						parameters->at(i)->reads[j] |= sortedAccessModes[j]
						        .first;
						++kt;
					}
				}

				// Fill array for write accesses
				kt = sortedAccessModes.begin();
				for (uint j = 0; j < writesSize; ++j) {
					parameters->at(i)->writes[j] = 0;

					if (j == (*kt).first) {
						parameters->at(i)->writes[j] |= sortedAccessModes[j]
						        .second;
						++kt;
					}
				}
			}
		}
	}

	return parameters;
}

LocalParametersList* KernelInfo::buildLocalParametersList(uint index) {
	LocalParametersList* localParametersList = new LocalParametersList;

	size_t& group_id = Setting->group_id;
	size_t& local_work_id = Setting->local_work_id;
	size_t& local_work_size = Setting->local_work_size;
	size_t num_groups = Setting->global_work_size / local_work_size;

	for (group_id = 0; group_id < num_groups; ++group_id) {
		for (local_work_id = 0; local_work_id < local_work_size;
		        ++local_work_id) {

			BuiltinFunctions->updateValues();

			ParameterPatternList* localParameters = buildLocalPattern(index);
			localParametersList->push_back(localParameters);
		}
	}

	return localParametersList;
}

std::pair<uint, uint>* KernelInfo::getMaxSizes(
        LocalParametersList* localParametersList, uint index) {

	MaxSizes* maxSizes = new MaxSizes;
	maxSizes->first = 0;
	maxSizes->second = 0;

	for (LocalParametersList::iterator it = localParametersList->begin();
	        it != localParametersList->end(); ++it) {

		uint readsSize = (*it)->at(index)->readsSize;
		uint writesSize = (*it)->at(index)->writesSize;

		if (maxSizes->first < readsSize) {
			maxSizes->first = readsSize;
		}

		if (maxSizes->second < writesSize) {
			maxSizes->second = writesSize;
		}
	}

	return maxSizes;
}

ParameterPatternList* KernelInfo::buildGlobalParametersList(uint index) {
	ParameterPatternList* globalParameters = new ParameterPatternList;
	IDList IDs = Functions[index].getParmDecls();
	LocalParametersList* localParametersList = buildLocalParametersList(index);

	uint i = 0;
	for (IDList::iterator it = IDs.begin(); it != IDs.end(); ++it, ++i) {
		const char* functionName = (*it)->getName().data();
		ParameterPattern* globalParameter = new ParameterPattern(functionName);
		globalParameter->type = (*localParametersList->begin())->at(i)->type;

		MaxSizes* maxSizes = getMaxSizes(localParametersList, i);
		uint readsSize = maxSizes->first;
		uint writesSize = maxSizes->second;
		globalParameter->reads = new bool[readsSize];
		globalParameter->readsSize = readsSize;
		globalParameter->writes = new bool[writesSize];
		globalParameter->writesSize = writesSize;

		for (uint j = 0; j < readsSize; ++j) {
			globalParameter->reads[j] = 0;
		}
		for (uint j = 0; j < writesSize; ++j) {
			globalParameter->writes[j] = 0;
		}

		for (LocalParametersList::iterator it = localParametersList->begin();
		        it != localParametersList->end(); ++it) {

			ParameterPattern* localParameter = (*it)->at(i);

			// Handle read access pattern
			for (uint j = 0; j < localParameter->readsSize; ++j) {
				globalParameter->reads[j] |= localParameter->reads[j];
			}

			// Handle write access pattern
			for (uint j = 0; j < localParameter->writesSize; ++j) {
				globalParameter->writes[j] |= localParameter->writes[j];

			}
		}

		globalParameters->push_back(globalParameter);
		delete maxSizes;
	}

	delete localParametersList;

	return globalParameters;
}

void KernelInfo::addFunction(const clang::FunctionDecl D) {
	KernelFunction Function(D, BuiltinFunctions);
	Functions.push_back(Function);
}

KernelFunction* KernelInfo::getCurrentFunction() {
	return &Functions.back();
}

bool KernelInfo::setSetting(size_t global_work_size, size_t local_work_size,
                            size_t global_work_offset, uint work_dim,
                            bool clEnqueueTask) {

	Setting->global_work_size = global_work_size;
	Setting->local_work_size = local_work_size;
	Setting->global_work_offset = global_work_offset;
	Setting->work_dim = work_dim;
	Setting->clEnqueueTask = clEnqueueTask;

	return BuiltinFunctions->updateValues();
}
