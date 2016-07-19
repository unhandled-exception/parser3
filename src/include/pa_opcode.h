/** @file
	Parser: compiled code related decls.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)

	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef OPCODE_H
#define OPCODE_H

namespace OP {

#define IDENT_PA_OPCODE_H "$Id: pa_opcode.h,v 1.53 2016/07/19 16:35:35 moko Exp $"

#define OPTIMIZE_BYTECODE_GET_ELEMENT                // $a ^a
#define OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT         // $a.b ^a.b
#define OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT     // $a.$b ^a.$b
#define OPTIMIZE_BYTECODE_STRING_POOL
#define OPTIMIZE_BYTECODE_OBJECT_POOL                // $var[$a.b], $var[$a.$b]
#define OPTIMIZE_BYTECODE_CUT_REM_OPERATOR           // cut rem with all params

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
#define OPTIMIZE_BYTECODE_CONSTRUCT                  // $a(expr),  $a[value]
#endif                                               // $.a(expr), $.a[value]
                                                     // $self.a(expr), $self.a[value]

#define OPTIMIZE_BYTECODE_GET_SELF_ELEMENT           // $self.a ^self.a
#define OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL       // .CLASS, .CLASS_NAME

#define FEATURE_GET_ELEMENT4CALL                     // ^o.m[] calls get_element4call("m"), not get_element("m")

///	Compiled operation code
enum OPCODE {
	//@{ 
	/// @name literals
	OP_VALUE,  OP_CURLY_CODE__STORE_PARAM,  OP_EXPR_CODE__STORE_PARAM,
	OP_NESTED_CODE,
	//@}

	//@{
	/// @name actions
	OP_WITH_ROOT,	OP_WITH_SELF,	OP_WITH_READ,	OP_WITH_WRITE,
	OP_VALUE__GET_CLASS, OP_VALUE__GET_BASE_CLASS,
	OP_CONSTRUCT_VALUE, OP_CONSTRUCT_EXPR, OP_CURLY_CODE__CONSTRUCT,
	OP_WRITE_VALUE,	OP_WRITE_EXPR_RESULT,	OP_STRING__WRITE,
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	OP_VALUE__GET_ELEMENT_OR_OPERATOR,
#else
	OP_GET_ELEMENT_OR_OPERATOR,
#endif
	OP_GET_ELEMENT,
	OP_GET_ELEMENT__WRITE,
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	OP_VALUE__GET_ELEMENT,
	OP_VALUE__GET_ELEMENT__WRITE,
	OP_WITH_ROOT__VALUE__GET_ELEMENT,
#endif
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
	OP_GET_OBJECT_ELEMENT,			// $a.b & ^a.b
	OP_GET_OBJECT_ELEMENT__WRITE,		// $a.b & ^a.b
#endif
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
	OP_GET_OBJECT_VAR_ELEMENT,		// $a.$b & ^a.$b
	OP_GET_OBJECT_VAR_ELEMENT__WRITE,	// $a.$b & ^a.$b
#endif
#ifdef OPTIMIZE_BYTECODE_GET_SELF_ELEMENT
	OP_WITH_SELF__VALUE__GET_ELEMENT,
	OP_WITH_SELF__VALUE__GET_ELEMENT__WRITE,
#endif

#ifdef FEATURE_GET_ELEMENT4CALL
	OP_GET_ELEMENT4CALL,
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
	OP_GET_OBJECT_ELEMENT4CALL,
#endif
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
	OP_GET_OBJECT_VAR_ELEMENT4CALL,
#endif
#endif // FEATURE_GET_ELEMENT4CALL

	OP_OBJECT_POOL,	OP_STRING_POOL,
	OP_PREPARE_TO_CONSTRUCT_OBJECT,
	OP_CONSTRUCT_OBJECT,
	OP_CONSTRUCT_OBJECT__WRITE,
	OP_PREPARE_TO_EXPRESSION, 
	OP_CALL, OP_CALL__WRITE,

#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
	OP_WITH_ROOT__VALUE__CONSTRUCT_EXPR,
	OP_WITH_ROOT__VALUE__CONSTRUCT_VALUE,
	OP_WITH_WRITE__VALUE__CONSTRUCT_EXPR,
	OP_WITH_WRITE__VALUE__CONSTRUCT_VALUE,
	OP_WITH_SELF__VALUE__CONSTRUCT_EXPR,
	OP_WITH_SELF__VALUE__CONSTRUCT_VALUE,
#endif
	//@}
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	OP_GET_ELEMENT__SPECIAL,
	OP_GET_ELEMENT__SPECIAL__WRITE,
#endif
	//@{
	/// @name expression ops: unary
	OP_NEG, OP_INV, OP_NOT, OP_DEF, OP_IN, OP_FEXISTS, OP_DEXISTS,
	//@}

	//@{
	/// @name expression ops: binary
	OP_SUB, OP_ADD, OP_MUL, OP_DIV, OP_MOD, OP_INTDIV,
	OP_BIN_SL, OP_BIN_SR,
	OP_BIN_AND, OP_BIN_OR, OP_BIN_XOR,
	OP_LOG_AND, OP_LOG_OR, OP_LOG_XOR,
	OP_NUM_LT, OP_NUM_GT, OP_NUM_LE, OP_NUM_GE, OP_NUM_EQ, OP_NUM_NE,
	OP_STR_LT, OP_STR_GT, OP_STR_LE, OP_STR_GE, OP_STR_EQ, OP_STR_NE,
	OP_IS
	//@}
};
}

#endif
