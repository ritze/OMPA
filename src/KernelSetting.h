/*
 * KernelSetting.h
 *
 *  Created on: 06.02.2014
 *      Author: Moritz Lüdecke
 */

#ifndef KERNELSETTING_H_
#define KERNELSETTING_H_

struct KernelSetting {
	KernelSetting() :
			clEnqueueTask(false), work_dim(0), global_work_offset(0),
			global_work_size(0), local_work_size(0), local_work_id(0),
			group_id(0) {
	};

	bool clEnqueueTask;
	uint work_dim;
	size_t global_work_offset;
	size_t global_work_size;
	size_t local_work_size;

	size_t local_work_id;
	size_t group_id;
};

#endif /* KERNELSETTING_H_ */
