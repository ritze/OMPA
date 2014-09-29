/*
 * Context.h
 *
 *  Created on: 26.2.2014
 *      Author: Moritz Lüdecke
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

//FIXME: De- and incrementation are not flagged as CF_Write, otherwise it
//       will be added twice.
enum ContextFlags {
	CF_None      = 0,   // the object do nothing
	CF_Read      = 1,   // the object has a read access
	CF_Write     = 2,   // the object has a write access
	CF_Array     = 4,   // the object is an array
	CF_Parm      = 8,   // the object is a parameter of a function
	CF_Index     = 16,  // the object is an index of an array
	CF_Depend    = 32,  // the object is depend on other objects
	CF_ArrayInit = 64   // the object is a value used for array initialization
};

typedef unsigned int Context;

#endif /* CONTEXT_H_ */
