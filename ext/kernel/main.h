
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#ifndef PHALCON_KERNEL_MAIN_H
#define PHALCON_KERNEL_MAIN_H

#include "php_phalcon.h"

#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include <ext/spl/spl_exceptions.h>
#include <ext/spl/spl_iterators.h>

#include "kernel/memory.h"
#include "kernel/backtrace.h"

/** Main macros */
#define PH_DEBUG 0

#define PH_NOISY 256
#define PH_SILENT 1024
#define PH_READONLY 4096

#define PH_NOISY_CC PH_NOISY TSRMLS_CC
#define PH_SILENT_CC PH_SILENT TSRMLS_CC

#define PH_SEPARATE 256
#define PH_COPY 1024
#define PH_CTOR 4096

#define SL(str)   ZEND_STRL(str)
#define SS(str)   ZEND_STRS(str)
#define ISL(str)  (phalcon_interned_##str), (sizeof(#str)-1)
#define ISS(str)  (phalcon_interned_##str), (sizeof(#str))

/* str_erealloc is PHP 5.6 only */
#ifndef str_erealloc
#define str_erealloc erealloc
#endif

/* Startup functions */
void php_phalcon_init_globals(zend_phalcon_globals *phalcon_globals TSRMLS_DC);
zend_class_entry *phalcon_register_internal_interface_ex(zend_class_entry *orig_ce, zend_class_entry *parent_ce);

/* Globals functions */
zval* phalcon_get_global(const char *global, unsigned int global_length TSRMLS_DC);

int phalcon_is_callable(zval *var TSRMLS_DC);
int phalcon_function_exists_ex(const char *func_name, unsigned int func_len TSRMLS_DC);

PHALCON_ATTR_NONNULL static inline zend_function *phalcon_fetch_function_str(const char *function_name, unsigned int function_len TSRMLS_DC)
{
	return zend_hash_str_find_ptr(EG(function_table), function_name, function_len+1);
}

PHALCON_ATTR_NONNULL static inline zend_function *phalcon_fetch_function(zend_string *function_name TSRMLS_DC)
{
	return zend_hash_find_ptr(EG(function_table), function_name);
}

/* Count */
long int phalcon_fast_count_int(zval *value TSRMLS_DC);
void phalcon_fast_count(zval *result, zval *array TSRMLS_DC);
int phalcon_fast_count_ev(zval *array TSRMLS_DC);

/* Utils functions */
int phalcon_is_iterable_ex(zval *arr, HashTable **arr_hash, HashPosition *hash_position, int duplicate, int reverse);

static inline int is_phalcon_class(const zend_class_entry *ce)
{
#if PHP_VERSION_ID >= 50400
	return
			ce->type == ZEND_INTERNAL_CLASS
		 && ce->info.internal.module->module_number == phalcon_module_entry.module_number
	;
#else
	return
			ce->type == ZEND_INTERNAL_CLASS
		 && ce->module->module_number == phalcon_module_entry.module_number
	;
#endif
}

/* Fetch Parameters */
int phalcon_fetch_parameters(int num_args TSRMLS_DC, int required_args, int optional_args, ...);
int phalcon_fetch_parameters_ex(int dummy TSRMLS_DC, int n_req, int n_opt, ...);

/* Compatibility macros for PHP 5.3 */
#ifndef INIT_PZVAL_COPY
 #define INIT_PZVAL_COPY(z, v) ZVAL_COPY_VALUE(z, v);\
  Z_SET_REFCOUNT_P(z, 1);\
  Z_UNSET_ISREF_P(z);
#endif

#ifndef ZVAL_COPY_VALUE
 #define ZVAL_COPY_VALUE(z, v)\
  (z)->value = (v)->value;\
  Z_TYPE_P(z) = Z_TYPE_P(v);
#endif

/** Return zval checking if it's needed to ctor */
#define RETURN_CCTOR(var) { \
		RETVAL_ZVAL_FAST(var); \
	} \
	PHALCON_MM_RESTORE(); \
	return;

/** Return zval checking if it's needed to ctor, without restoring the memory stack  */
#define RETURN_CCTORW(var) { \
		RETVAL_ZVAL_FAST(var); \
	} \
	return;

/** Return zval with always ctor */
#define RETURN_CTOR(var) { \
		RETVAL_ZVAL_FAST(var); \
	} \
	PHALCON_MM_RESTORE(); \
	return;

/** Return zval with always ctor, without restoring the memory stack */
#define RETURN_CTORW(var) { \
		RETVAL_ZVAL_FAST(var); \
	} \
	return;

/** Return this pointer */
#define RETURN_THIS() { \
		RETVAL_ZVAL_FAST(this_ptr); \
	} \
	PHALCON_MM_RESTORE(); \
	return;

/** Return zval with always ctor, without restoring the memory stack */
#define RETURN_THISW() \
	RETURN_ZVAL_FAST(this_ptr);

/**
 * Returns variables without ctor
 */
#define RETURN_NCTOR(var) { \
		*(return_value) = *(var); \
		INIT_PZVAL(return_value) \
	} \
	PHALCON_MM_RESTORE(); \
	return;

/**
 * Returns a zval in an object member
 */
#define RETURN_MEMBER(object, member_name) \
	phalcon_return_property_quick(return_value, NULL, object, SL(member_name), zend_inline_hash_func(SS(member_name)) TSRMLS_CC); \
	return;

/**
 * Returns a zval in an object member
 */
#define RETURN_MM_MEMBER(object, member_name) \
	phalcon_return_property_quick(return_value, NULL, object, SL(member_name), zend_inline_hash_func(SS(member_name)) TSRMLS_CC); \
	RETURN_MM();

/**
 * Returns a zval in an object member (quick)
 */
#define RETURN_MEMBER_QUICK(object, member_name, key) \
	phalcon_return_property_quick(return_value, NULL, object, SL(member_name), key TSRMLS_CC); \
	return;

/**
 * Returns a zval in an object member (quick)
 */
#define RETURN_MM_MEMBER_QUICK(object, member_name, key) \
	phalcon_return_property_quick(return_value, NULL, object, SL(member_name), key TSRMLS_CC); \
	RETURN_MM();

#define RETURN_ON_FAILURE(what) \
	if (FAILURE == what) { \
		return;            \
	}

#define RETURN_MM_ON_FAILURE(what) \
	if (FAILURE == what) {    \
		PHALCON_MM_RESTORE(); \
		return;               \
	}

/** Return without change return_value */
#define RETURN_MM() PHALCON_MM_RESTORE(); return;

/** Return bool restoring memory frame */
#define RETURN_MM_BOOL(value) RETVAL_BOOL(value); RETURN_MM();

/** Return null restoring memory frame */
#define RETURN_MM_NULL() PHALCON_MM_RESTORE(); RETURN_NULL();

/** Return bool restoring memory frame */
#define RETURN_MM_FALSE PHALCON_MM_RESTORE(); RETURN_FALSE;
#define RETURN_MM_TRUE PHALCON_MM_RESTORE(); RETURN_TRUE;

/** Return string restoring memory frame */
#define RETURN_MM_STRING(str, copy) PHALCON_MM_RESTORE(); RETURN_STRING(str, copy);
#define RETURN_MM_EMPTY_STRING() PHALCON_MM_RESTORE(); RETURN_EMPTY_STRING();

/* Return long */
#define RETURN_MM_LONG(value) RETVAL_LONG(value); RETURN_MM();

/* Return double */
#define RETURN_MM_DOUBLE(value) RETVAL_DOUBLE(value); RETURN_MM();

/** Return empty array */
#define RETURN_EMPTY_ARRAY() array_init(return_value); return;
#define RETURN_MM_EMPTY_ARRAY() PHALCON_MM_RESTORE(); RETURN_EMPTY_ARRAY();

#ifndef IS_INTERNED
#define IS_INTERNED(key) 0
#define INTERNED_HASH(key) 0
#endif

/** Get the current hash key without copying the hash key */
#define PHALCON_GET_HKEY(var, hash, hash_position) \
	do { \
		PHALCON_INIT_NVAR_PNULL(var); \
		phalcon_get_current_key(&var, hash, &hash_position TSRMLS_CC); \
	} while (0)

/** Get current hash key copying the iterator if needed */

#if PHP_VERSION_ID < 50500

#define PHALCON_GET_IKEY(var, it) \
	do { \
		int key_type; uint str_key_len; \
		ulong int_key; \
		char *str_key; \
		\
		PHALCON_INIT_NVAR(var); \
		key_type = it->funcs->get_current_key(it, &str_key, &str_key_len, &int_key TSRMLS_CC); \
		if (key_type == HASH_KEY_IS_STRING) { \
			ZVAL_STRINGL(var, str_key, str_key_len - 1, 1); \
			efree(str_key); \
		} else { \
			if (key_type == HASH_KEY_IS_LONG) { \
				ZVAL_LONG(var, int_key); \
			} else { \
				ZVAL_NULL(var); \
			} \
		} \
	} while (0)

#else

#define PHALCON_GET_IKEY(var, it) \
	do { \
		PHALCON_INIT_NVAR(var); \
		it->funcs->get_current_key(it, var TSRMLS_CC); \
	} while (0)

#endif

/** Check if an array is iterable or not */
#define phalcon_is_iterable(var, array_hash, hash_pointer, duplicate, reverse) \
	if (!phalcon_is_iterable_ex(var, array_hash, hash_pointer, duplicate, reverse)) { \
		zend_error(E_ERROR, "The argument is not iterable()"); \
		PHALCON_MM_RESTORE(); \
		return; \
	}

#define PHALCON_GET_HVALUE(var) \
	PHALCON_OBS_NVAR(var); \
	var = *hd; \
	Z_ADDREF_P(var);

/** class/interface registering */
#define PHALCON_REGISTER_CLASS(ns, class_name, name, methods, flags) \
	{ \
		zend_class_entry ce; \
		memset(&ce, 0, sizeof(zend_class_entry)); \
		INIT_NS_CLASS_ENTRY(ce, #ns, #class_name, methods); \
		phalcon_ ##name## _ce = zend_register_internal_class(&ce TSRMLS_CC); \
		phalcon_ ##name## _ce->ce_flags |= flags;  \
	}

#define PHALCON_REGISTER_CLASS_EX(ns, class_name, lcname, parent_ce, methods, flags) \
	{ \
		zend_class_entry ce; \
		memset(&ce, 0, sizeof(zend_class_entry)); \
		INIT_NS_CLASS_ENTRY(ce, #ns, #class_name, methods); \
		phalcon_ ##lcname## _ce = zend_register_internal_class_ex(&ce, parent_ce, NULL TSRMLS_CC); \
		if (!phalcon_ ##lcname## _ce) { \
			fprintf(stderr, "Phalcon Error: Class to extend '%s' was not found when registering class '%s'\n", (parent_ce ? parent_ce->name : "(null)"), ZEND_NS_NAME(#ns, #class_name)); \
			return FAILURE; \
		} \
		phalcon_ ##lcname## _ce->ce_flags |= flags;  \
	}

#define PHALCON_REGISTER_INTERFACE(ns, classname, name, methods) \
	{ \
		zend_class_entry ce; \
		memset(&ce, 0, sizeof(zend_class_entry)); \
		INIT_NS_CLASS_ENTRY(ce, #ns, #classname, methods); \
		phalcon_ ##name## _ce = zend_register_internal_interface(&ce); \
	}

#define PHALCON_REGISTER_INTERFACE_EX(ns, classname, lcname, parent_ce, methods) \
	{ \
		zend_class_entry ce; \
		memset(&ce, 0, sizeof(zend_class_entry)); \
		INIT_NS_CLASS_ENTRY(ce, #ns, #classname, methods); \
		phalcon_ ##lcname## _ce = phalcon_register_internal_interface_ex(&ce, parent_ce); \
		if (!phalcon_ ##lcname## _ce) { \
			fprintf(stderr, "Can't register interface %s with parent %s\n", ZEND_NS_NAME(#ns, #classname), (parent_ce ? parent_ce->name : "(null)")); \
			return FAILURE; \
		} \
	}

/** Method declaration for API generation */
#define PHALCON_DOC_METHOD(class_name, method)

/** Low overhead parse/fetch parameters */
#ifndef PHALCON_RELEASE

#define phalcon_fetch_params(memory_grow, required_params, optional_params, ...) \
	if (memory_grow) { \
		zend_phalcon_globals *phalcon_globals_ptr = PHALCON_VGLOBAL; \
		ASSUME(phalcon_globals_ptr != NULL); \
		if (unlikely(phalcon_globals_ptr->active_memory == NULL)) { \
			fprintf(stderr, "phalcon_fetch_params is called with memory_grow=1 but there is no active memory frame!\n"); \
			phalcon_print_backtrace(); \
		} \
		else if (unlikely(phalcon_globals_ptr->active_memory->func != __func__)) { \
			fprintf(stderr, "phalcon_fetch_params is called with memory_grow=1 but the memory frame was not created!\n"); \
			fprintf(stderr, "The frame was created by %s\n", phalcon_globals_ptr->active_memory->func); \
			fprintf(stderr, "Calling function: %s\n", __func__); \
			phalcon_print_backtrace(); \
		} \
	} \
	if (phalcon_fetch_parameters(ZEND_NUM_ARGS() TSRMLS_CC, required_params, optional_params, __VA_ARGS__) == FAILURE) { \
		if (memory_grow) { \
			RETURN_MM_NULL(); \
		} \
		RETURN_NULL(); \
	} \

#else

#define phalcon_fetch_params(memory_grow, required_params, optional_params, ...) \
	if (phalcon_fetch_parameters(ZEND_NUM_ARGS() TSRMLS_CC, required_params, optional_params, __VA_ARGS__) == FAILURE) { \
		if (memory_grow) { \
			RETURN_MM_NULL(); \
		} \
		RETURN_NULL(); \
	}
#endif

#define phalcon_fetch_params_ex(required_params, optional_params, ...) \
	if (phalcon_fetch_parameters_ex(0 TSRMLS_CC, required_params, optional_params, __VA_ARGS__) == FAILURE) { \
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Wrong number of parameters"); \
		return; \
	}

#define PHALCON_VERIFY_INTERFACE_EX(instance, interface_ce, exception_ce, restore_stack) \
	if (Z_TYPE_P(instance) != IS_OBJECT) { \
		zend_throw_exception_ex(exception_ce, 0 TSRMLS_CC, "Unexpected value type: expected object implementing %s, %s given", interface_ce->name, zend_zval_type_name(instance)); \
		if (restore_stack) { \
			PHALCON_MM_RESTORE(); \
		} \
		return; \
	} else { \
		if (!instanceof_function_ex(Z_OBJCE_P(instance), interface_ce, 1)) { \
			if (Z_TYPE_P(instance) != IS_OBJECT) { \
				zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object implementing %s, %s given", interface_ce->name, zend_zval_type_name(instance)); \
			} \
			else { \
				zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object implementing %s, object of type %s given", interface_ce->name, Z_OBJCE_P(instance)->name); \
			} \
			if (restore_stack) { \
				PHALCON_MM_RESTORE(); \
			} \
			return; \
		} \
	}

#define PHALCON_VERIFY_INTERFACE_OR_NULL_EX(pzv, interface_ce, exception_ce, restore_stack) \
	if (Z_TYPE_P(pzv) != IS_NULL && (Z_TYPE_P(pzv) != IS_OBJECT || !instanceof_function_ex(Z_OBJCE_P(pzv), interface_ce, 1))) { \
		if (Z_TYPE_P(pzv) != IS_OBJECT) { \
			zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object implementing %s or NULL, %s given", interface_ce->name, zend_zval_type_name(pzv)); \
		} \
		else { \
			zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object implementing %s or NULL, object of type %s given", interface_ce->name, Z_OBJCE_P(pzv)->name); \
		} \
		if (restore_stack) { \
			PHALCON_MM_RESTORE(); \
		} \
		return; \
	}

#define PHALCON_VERIFY_CLASS_EX(instance, class_ce, exception_ce, restore_stack) \
	if (Z_TYPE_P(instance) != IS_OBJECT || !instanceof_function_ex(Z_OBJCE_P(instance), class_ce, 0)) { \
		if (Z_TYPE_P(instance) != IS_OBJECT) { \
			zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object of type %s, %s given", class_ce->name, zend_zval_type_name(instance)); \
		} \
		else { \
			zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object of type %s, object of type %s given", class_ce->name, Z_OBJCE_P(instance)->name); \
		} \
		if (restore_stack) { \
			PHALCON_MM_RESTORE(); \
		} \
		return; \
	}

#define PHALCON_VERIFY_CLASS_OR_NULL_EX(pzv, class_ce, exception_ce, restore_stack) \
	if (Z_TYPE_P(pzv) != IS_NULL && (Z_TYPE_P(pzv) != IS_OBJECT || !instanceof_function_ex(Z_OBJCE_P(pzv), class_ce, 0))) { \
		if (Z_TYPE_P(pzv) != IS_OBJECT) { \
			zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object of type %s, %s given", class_ce->name, zend_zval_type_name(pzv)); \
		} \
		else { \
			zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object of type %s, object of type %s given", class_ce->name, Z_OBJCE_P(pzv)->name); \
		} \
		if (restore_stack) { \
			PHALCON_MM_RESTORE(); \
		} \
		return; \
	}

#define PHALCON_VERIFY_INTERFACE(instance, interface_ce)      PHALCON_VERIFY_INTERFACE_EX(instance, interface_ce, spl_ce_LogicException, 1)
#define PHALCON_VERIFY_INTERFACEW(instance, interface_ce)      PHALCON_VERIFY_INTERFACE_EX(instance, interface_ce, spl_ce_LogicException, 0)
#define PHALCON_VERIFY_INTERFACE_OR_NULL(pzv, interface_ce)   PHALCON_VERIFY_INTERFACE_OR_NULL_EX(pzv, interface_ce, spl_ce_LogicException, 1)
#define PHALCON_VERIFY_CLASS(instance, class_ce)              PHALCON_VERIFY_CLASS_EX(instance, class_ce, spl_ce_LogicException, 1)
#define PHALCON_VERIFY_CLASS_OR_NULL(pzv, class_ce)           PHALCON_VERIFY_CLASS_OR_NULL_EX(pzv, class_ce, spl_ce_LogicException, 1)

#define phalcon_convert_to_explicit_type_mm_ex(ppzv, str_type) \
	if (Z_TYPE_PP(ppzv) != str_type) { \
		if (!Z_ISREF_PP(ppzv)) { \
			PHALCON_SEPARATE(*ppzv); \
		} \
		convert_to_explicit_type(*ppzv, str_type); \
	}

#define PHALCON_ENSURE_IS_STRING(ppzv)    convert_to_explicit_type_ex(ppzv, IS_STRING)
#define PHALCON_ENSURE_IS_LONG(ppzv)      convert_to_explicit_type_ex(ppzv, IS_LONG)
#define PHALCON_ENSURE_IS_DOUBLE(ppzv)    convert_to_explicit_type_ex(ppzv, IS_DOUBLE)
#define PHALCON_ENSURE_IS_BOOL(ppzv)      convert_to_explicit_type_ex(ppzv, IS_BOOL)
#define PHALCON_ENSURE_IS_ARRAY(ppzv)     convert_to_explicit_type_ex(ppzv, IS_ARRAY)

#if PHP_VERSION_ID >= 50600

#if ZEND_MODULE_API_NO >= 20141001
void phalcon_clean_and_cache_symbol_table(zend_array *symbol_table);
#else
void phalcon_clean_and_cache_symbol_table(HashTable *symbol_table TSRMLS_DC);
#endif

#endif

#define PHALCON_CHECK_POINTER(v) if (!v) fprintf(stderr, "%s:%d\n", __PRETTY_FUNCTION__, __LINE__)
#define PHALCON_DEBUG_POINTER() fprintf(stderr, "%s:%d\n", __PRETTY_FUNCTION__, __LINE__)

#endif /* PHALCON_KERNEL_MAIN_H */
