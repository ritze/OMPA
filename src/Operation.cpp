/*
 * Operation.cpp
 *
 *  Created on: 18.12.2013
 *      Author: Moritz Lüdecke
 */

#include <stddef.h>

#include "Operation.h"

// XXX: Stolen from http://stackoverflow.com/questions/6336025/why-do-several-of-the-standard-operators-not-have-standard-functors
#define DEFINE_BINARY_OP(name, op)  \
int name (int a, int b) {           \
	return a op b;                  \
}                                   \

DEFINE_BINARY_OP(plus, +);
DEFINE_BINARY_OP(minus, -);
DEFINE_BINARY_OP(multiplies, *);
DEFINE_BINARY_OP(divides, /);
DEFINE_BINARY_OP(modulus, %);

DEFINE_BINARY_OP(logical_or, ||);
DEFINE_BINARY_OP(logical_and, &&);

DEFINE_BINARY_OP(equal_to, ==);
DEFINE_BINARY_OP(not_equal_to, !=);
DEFINE_BINARY_OP(less, <);
DEFINE_BINARY_OP(greater, >);
DEFINE_BINARY_OP(less_equal, <=);
DEFINE_BINARY_OP(greater_equal, >=);

DEFINE_BINARY_OP(bitwise_and, &);
DEFINE_BINARY_OP(bitwise_or, |);
DEFINE_BINARY_OP(bitwise_xor, ^);
DEFINE_BINARY_OP(left_shift, <<);
DEFINE_BINARY_OP(right_shift, >>);

DEFINE_BINARY_OP(assign, =);
DEFINE_BINARY_OP(plus_assign, +=);
DEFINE_BINARY_OP(minus_assign, -=);
DEFINE_BINARY_OP(multiplies_assign, *=);
DEFINE_BINARY_OP(divides_assign, /=);
DEFINE_BINARY_OP(modulus_assign, %=);
DEFINE_BINARY_OP(bitwise_and_assign, &=);
DEFINE_BINARY_OP(bitwise_or_assign, |=);
DEFINE_BINARY_OP(bitwise_xor_assign, ^=);
DEFINE_BINARY_OP(left_shift_assign, <<=);
DEFINE_BINARY_OP(right_shift_assign, >>=);

#undef DEFINE_BINARY_OP

#define DEFINE_UNARY_OP(name, pre_op, post_op)  \
int name (int a) {                              \
	return pre_op a post_op;                    \
}                                               \

DEFINE_UNARY_OP(unary_plus, +,);
DEFINE_UNARY_OP(logical_not, !,);
DEFINE_UNARY_OP(negate, -,);
DEFINE_UNARY_OP(bitwise_not, ~,);
DEFINE_UNARY_OP(prefix_increment, ++,);
DEFINE_UNARY_OP(postfix_increment,, ++);
DEFINE_UNARY_OP(prefix_decrement, --,);
DEFINE_UNARY_OP(postfix_decrement,, --);

#undef DEFINE_UNARY_OP

typedef int (*ptr2BinaryOperator)(int, int);
static ptr2BinaryOperator getBinaryOperand(Opcode opc) {
	switch (opc) {
		case clang::BO_Add:
			return &plus;

		case clang::BO_Sub:
			return &minus;

		case clang::BO_Mul:
			return &multiplies;

		case clang::BO_Div:
			return &divides;

		case clang::BO_Rem:
			return &modulus;

		case clang::BO_LOr:
			return &logical_or;

		case clang::BO_LAnd:
			return &logical_and;

		case clang::BO_EQ:
			return &equal_to;

		case clang::BO_NE:
			return &not_equal_to;

		case clang::BO_LT:
			return &less;

		case clang::BO_GT:
			return &greater;

		case clang::BO_LE:
			return &less_equal;

		case clang::BO_GE:
			return &greater_equal;

		case clang::BO_And:
			return &bitwise_and;

		case clang::BO_Or:
			return &bitwise_or;

		case clang::BO_Xor:
			return &bitwise_xor;

		case clang::BO_Shl:
			return &left_shift;

		case clang::BO_Shr:
			return &right_shift;

		case clang::BO_Assign:
			return &assign;

		case clang::BO_AddAssign:
			return &plus_assign;

		case clang::BO_SubAssign:
			return &minus_assign;

		case clang::BO_MulAssign:
			return &multiplies_assign;

		case clang::BO_DivAssign:
			return &divides_assign;

		case clang::BO_RemAssign:
			return &modulus_assign;

		case clang::BO_AndAssign:
			return &bitwise_and_assign;

		case clang::BO_OrAssign:
			return &bitwise_or_assign;

		case clang::BO_XorAssign:
			return &bitwise_xor_assign;

		case clang::BO_ShlAssign:
			return &left_shift_assign;

		case clang::BO_ShrAssign:
			return &right_shift_assign;

		default:
			return NULL;
	}
}

#ifdef DEBUG
static std::string getBinaryOperandDescription(Opcode opc) {
	switch (opc) {
		case clang::BO_Add:
		return "+";

		case clang::BO_Sub:
		return "-";

		case clang::BO_Mul:
		return "*";

		case clang::BO_Div:
		return "/";

		case clang::BO_Rem:
		return "%";

		case clang::BO_LOr:
		return "||";

		case clang::BO_LAnd:
		return "&&";

		case clang::BO_EQ:
		return "==";

		case clang::BO_NE:
		return "!=";

		case clang::BO_LT:
		return "<";

		case clang::BO_GT:
		return ">";

		case clang::BO_LE:
		return "<=";

		case clang::BO_GE:
		return ">=";

		case clang::BO_And:
		return "&";

		case clang::BO_Or:
		return "|";

		case clang::BO_Xor:
		return "^";

		case clang::BO_Shl:
		return "<<";

		case clang::BO_Shr:
		return ">>";

		case clang::BO_Assign:
		return "=";

		case clang::BO_AddAssign:
		return "+=";

		case clang::BO_SubAssign:
		return "-=";

		case clang::BO_MulAssign:
		return "*=";

		case clang::BO_DivAssign:
		return "/=";

		case clang::BO_RemAssign:
		return "%=";

		case clang::BO_AndAssign:
		return "&=";

		case clang::BO_OrAssign:
		return "|=";

		case clang::BO_XorAssign:
		return "^=";

		case clang::BO_ShlAssign:
		return "<<=";

		case clang::BO_ShrAssign:
		return ">>=";

		default:
		return "";
	}
}
#endif /* DEBUG */

typedef int (*ptr2UnaryOperator)(int);
static ptr2UnaryOperator getUnaryOperand(Opcode opc) {
	switch (opc) {
		case clang::UO_Plus:
			return &unary_plus;

		case clang::UO_LNot:
			return &logical_not;

		case clang::UO_Minus:
			return &negate;

		case clang::UO_Not:
			return &bitwise_not;

		case clang::UO_PreDec:
			return &prefix_decrement;

		case clang::UO_PreInc:
			return &prefix_increment;

		case clang::UO_PostDec:
			return &postfix_decrement;

		case clang::UO_PostInc:
			return &postfix_increment;

		default:
			return NULL;
	}
}

#ifdef DEBUG
static std::string getUnaryOperandDescription(Opcode opc) {
	switch (opc) {
		case clang::UO_Plus:
		return "+";

		case clang::UO_LNot:
		return "!";

		case clang::UO_Minus:
		return "-";

		case clang::UO_Not:
		return "|";

		case clang::UO_PreDec:
		return "--";

		case clang::UO_PreInc:
		return "++";

		case clang::UO_PostDec:
		return "--";

		case clang::UO_PostInc:
		return "++";

		default:
		return "";
	}
}

static bool unaryOperandIsPrefix(Opcode opc) {
	if (opc == clang::UO_PostDec || opc == clang::UO_PostInc) {
		return false;

	} else {
		return true;
	}
}
#endif /* DEBUG */

static int UNDEFINED = 0;

Parameter::Parameter() :
		type(PT_None) {
}
;

Parameter::~Parameter() {
	switch (type) {
		case PT_Var:
			if (object.variable != NULL) {
				delete object.variable;
			}
			break;

		case PT_BO:
			if (object.binaryOperation != NULL) {
				delete object.binaryOperation;
			}
			break;

		case PT_UO:
			if (object.unaryOperation != NULL) {
				delete object.unaryOperation;
			}
			break;

		case PT_Par:
			if (object.parameter != NULL) {
				delete object.parameter;
			}
			break;

		default:
			;
	}
}

bool Parameter::pushParameter(Parameter* parameter) {
	switch (parameter->type) {
		case PT_Var:
			return pushVariable(parameter->object.variable);

		case PT_BO:
			return pushOperation(parameter->object.binaryOperation);

		case PT_UO:
			return pushOperation(parameter->object.unaryOperation);

		default:
			type = PT_Par;
			object.parameter = parameter;
			return true;
	}
}

#define DEFINE_PARAMETER_STATUS(name, returnType, fault)  \
returnType Parameter::name() {                            \
	switch (type) {                                       \
		case PT_Var:                                      \
			return object.variable->name();               \
			                                              \
		case PT_BO:	                                      \
			return object.binaryOperation->name();        \
			                                              \
		case PT_UO:                                       \
			return object.unaryOperation->name();         \
			                                              \
		case PT_Par:                                      \
			return object.parameter->name();              \
		                                                  \
		default:                                          \
			return fault;                                 \
	}                                                     \
}                                                         \

#ifdef DEBUG
DEFINE_PARAMETER_STATUS(getDescription, std::string, "NULL" );
#endif /* DEBUG */
DEFINE_PARAMETER_STATUS(getValue, int, 0);
DEFINE_PARAMETER_STATUS(isOccupied, bool, false);
#undef DEFINE_PARAMETER_STATUS

#define DEFINE_PARAMETER_VAR(name, className)           \
bool Parameter::name(className parm) {                  \
	if (type == PT_None) {                              \
		type = PT_Var;                                  \
		object.variable = new Variable;                 \
	}                                                   \
	                                                    \
	switch (type) {                                     \
		case PT_Var:                                    \
			return object.variable->name(parm);         \
		                                                \
		case PT_BO:                                     \
			return object.binaryOperation->name(parm);  \
		                                                \
		case PT_UO:                                     \
			return object.unaryOperation->name(parm);   \
		                                                \
		case PT_Par:                                    \
			return object.parameter->name(parm);        \
		                                                \
		default:                                        \
			return false;                               \
	}                                                   \
}                                                       \

DEFINE_PARAMETER_VAR(pushVariable, Variable*);
DEFINE_PARAMETER_VAR(pushVariable, int);
DEFINE_PARAMETER_VAR(pushReference, int*);
#undef DEFINE_PARAMETER_FUNC_VAR

#define DEFINE_PARAMETER_OP(parmType, className, parmObj)             \
bool Parameter::pushOperation(className operation) {                  \
	if (type == PT_None) {                                            \
		type = parmType;                                              \
		object.parmObj = operation;                                   \
		return true;                                                  \
	}                                                                 \
	                                                                  \
	switch (type) {                                                   \
		case PT_BO:                                                   \
			return object.binaryOperation->pushOperation(operation);  \
		                                                              \
		case PT_UO:                                                   \
			return object.unaryOperation->pushOperation(operation);   \
		                                                              \
		case PT_Par:                                                  \
			return object.parameter->pushOperation(operation);        \
		                                                              \
		default:                                                      \
			return false;                                             \
	}                                                                 \
}                                                                     \

DEFINE_PARAMETER_OP(PT_BO, BinaryOperation*, binaryOperation);
DEFINE_PARAMETER_OP(PT_UO, UnaryOperation*, unaryOperation);
#undef DEFINE_PARAMETER_OP

bool Parameter::pushArrayElement(Array* array, Parameter* index) {
	if (type == PT_None) {
		type = PT_Var;
		object.variable = new Variable;
	}

	switch (type) {
		case PT_Var:
			return object.variable->pushArrayElement(array, index);

		case PT_BO:
			return object.binaryOperation->pushArrayElement(array, index);

		case PT_UO:
			return object.unaryOperation->pushArrayElement(array, index);

		case PT_Par:
			return object.parameter->pushArrayElement(array, index);

		default:
			return false;
	}
}

Variable::Variable() :
		holdInArray(false), occupied(false), referenced(false),
		variable(UNDEFINED), reference(NULL), array(NULL) {
}

Variable::~Variable() {
	if (array != NULL) {
		delete array;
	}
}

bool Variable::isOccupied() {
	return occupied;
}

#ifdef DEBUG
std::string Variable::getDescription() {
	std::ostringstream oss;

	if (referenced) {
		oss << *reference;

	} else if (holdInArray) {
		oss << array->getValue();

	} else {
		oss << variable;
	}

	return oss.str();
}
#endif /* DEBUG */

int Variable::getValue() {
	if (referenced) {
		return *reference;

	} else if (holdInArray) {
		return array->getValue();

	} else {
		return variable;
	}
}

bool Variable::pushArrayElement(Array* array, Parameter* index) {
	if (occupied) {
		return false;

	} else {
		this->array = new ArrayTimestamp(array, index);
		return (occupied = holdInArray = true);
	}
}

bool Variable::pushVariable(Variable* var) {
	if (var->referenced) {
		return pushReference(var->reference);

	} else {
		return pushVariable(var->variable);
	}
}

bool Variable::pushVariable(int value) {
	if (occupied) {
		return false;

	} else {
		variable = value;
		return (occupied = true);
	}
}

bool Variable::pushReference(int* ref) {
	if (occupied) {
		return false;

	} else {
		reference = ref;
		referenced = true;
		return (occupied = true);
	}
}

BinaryOperation::BinaryOperation(Opcode opc) :
		isDivOperand(opc == clang::BO_Div || opc == clang::BO_DivAssign
		             || opc == clang::BO_Rem || opc == clang::BO_RemAssign) {
	operand = getBinaryOperand(opc);

#ifdef DEBUG
	operandDescription = getBinaryOperandDescription(opc);
#endif /* DEBUG */

}

BinaryOperation::~BinaryOperation() {
}

#ifdef DEBUG
std::string BinaryOperation::getDescription() {
	return "(" + a.getDescription() + " " + operandDescription + " "
	+ b.getDescription() + ")";
}
#endif /* DEBUG */

int BinaryOperation::getValue() {
	if (isOccupied() && !(isDivOperand && b.getValue() == 0)) {

		return operand(a.getValue(), b.getValue());

	} else {
		return UNDEFINED;
	}
}

bool BinaryOperation::isOccupied() {
	return a.isOccupied() && b.isOccupied();
}

bool BinaryOperation::pushArrayElement(Array* array, Parameter* index) {
	return a.pushArrayElement(array, index) || b.pushArrayElement(array, index);
}

#define DEFINE_BOP_PUSH_VAR(name, className)  \
bool BinaryOperation::name(className parm) {  \
	return a.name(parm) || b.name(parm);      \
}                                             \

DEFINE_BOP_PUSH_VAR(pushVariable, Variable*);
DEFINE_BOP_PUSH_VAR(pushVariable, int);
DEFINE_BOP_PUSH_VAR(pushReference, int*);
#undef DEFINE_PARAMETER_OP

#define DEFINE_BOP_PUSH_OP(parmType, className, parmObj)     \
bool BinaryOperation::pushOperation(className* operation) {  \
	if (a.type == PT_None) {                                 \
		a.type = parmType;                                   \
		a.object.parmObj = operation;                        \
		return true;                                         \
		                                                     \
	} else if (a.pushOperation(operation)) {                 \
		return true;                                         \
		                                                     \
	} else if (b.type == PT_None) {                          \
		b.type = parmType;                                   \
		b.object.parmObj = operation;                        \
		return true;                                         \
		                                                     \
	} else if (b.pushOperation(operation)) {                 \
		return true;                                         \
		                                                     \
	} else {                                                 \
		return false;                                        \
	}                                                        \
}                                                            \

DEFINE_BOP_PUSH_OP(PT_BO, BinaryOperation, binaryOperation);
DEFINE_BOP_PUSH_OP(PT_UO, UnaryOperation, unaryOperation);
#undef DEFINE_BOP_PUSH_OP

UnaryOperation::UnaryOperation(Opcode opc) {
	operand = getUnaryOperand(opc);

#ifdef DEBUG
	operandDescription = getUnaryOperandDescription(opc);
	isPrefix = unaryOperandIsPrefix(opc);
#endif /* DEBUG */

}

UnaryOperation::~UnaryOperation() {
}

#ifdef DEBUG
std::string UnaryOperation::getDescription() {
	if (isPrefix) {
		return operandDescription + a.getDescription();
	} else {
		return a.getDescription() + operandDescription;
	}
}
#endif /* DEBUG */

int UnaryOperation::getValue() {
	if (isOccupied()) {
		return operand(a.getValue());

	} else {
		return UNDEFINED;
	}
}

bool UnaryOperation::isOccupied() {
	return a.isOccupied();
}

bool UnaryOperation::pushArrayElement(Array* array, Parameter* index) {
	return a.pushArrayElement(array, index);
}

#define DEFINE_UOP_PUSH_VAR(name, className)  \
bool UnaryOperation::name(className parm) {   \
	return a.name(parm);                      \
}                                             \

DEFINE_UOP_PUSH_VAR(pushVariable, Variable*);
DEFINE_UOP_PUSH_VAR(pushVariable, int);
DEFINE_UOP_PUSH_VAR(pushReference, int*);
#undef DEFINE_UOP_PUSH_VAR

#define DEFINE_UOP_PUSH_OP(parmType, className, parmObj)    \
bool UnaryOperation::pushOperation(className* operation) {  \
	if (a.type == PT_None) {                                \
		a.type = parmType;                                  \
		a.object.parmObj = operation;                       \
		return true;                                        \
		                                                    \
	} else {                                                \
		return a.pushOperation(operation);                  \
	}                                                       \
}                                                           \

DEFINE_UOP_PUSH_OP(PT_BO, BinaryOperation, binaryOperation);
DEFINE_UOP_PUSH_OP(PT_UO, UnaryOperation, unaryOperation);
#undef DEFINE_UOP_PUSH_OP
