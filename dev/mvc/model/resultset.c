
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2012 Phalcon Team (http://www.phalconphp.com)       |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "kernel/main.h"
#include "kernel/memory.h"

#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/exception.h"
#include "kernel/array.h"

/**
 * Phalcon\Mvc\Model\Resultset
 *
 * This component allows to Phalcon\Mvc\Model returns large resulsets with the minimum memory consumption
 * Resulsets can be traversed using a standard foreach or a while statement. If a resultset is serialized
 * it will dump all the rows into a big array. Then unserialize will retrieve the rows as they were before
 * serializing.
 *
 * 
 *
 */

/**
 * Phalcon\Mvc\Model\Resultset constructor
 *
 * @param Phalcon\Mvc\Model $model
 * @param Phalcon\Mvc\Model\Result $result
 * @param Phalcon\Cache\Backend $cache
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, __construct){

	zval *model = NULL, *result = NULL, *cache = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &model, &result, &cache) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!cache) {
		PHALCON_ALLOC_ZVAL_MM(cache);
		ZVAL_NULL(cache);
	}
	
	phalcon_update_property_zval(this_ptr, SL("_model"), model TSRMLS_CC);
	phalcon_update_property_zval(this_ptr, SL("_result"), result TSRMLS_CC);
	phalcon_update_property_zval(this_ptr, SL("_cache"), cache TSRMLS_CC);
	phalcon_update_property_long(this_ptr, SL("_type"), 1 TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

/**
 * Check whether internal resource has rows to fetch
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, valid){

	zval *result = NULL, *row = NULL, *model = NULL, *active_row = NULL, *rows = NULL;
	zval *t0 = NULL;
	zval *c0 = NULL, *c1 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_type"), PH_NOISY_CC);
	if (zend_is_true(t0)) {
		PHALCON_INIT_VAR(result);
		phalcon_read_property(&result, this_ptr, SL("_result"), PH_NOISY_CC);
		if (Z_TYPE_P(result) != IS_BOOL || (Z_TYPE_P(result) == IS_BOOL && Z_BVAL_P(result))) {
			PHALCON_INIT_VAR(c0);
			ZVAL_LONG(c0, 1);
			PHALCON_CALL_METHOD_PARAMS_1_NORETURN(result, "setfetchmode", c0, PH_NO_CHECK);
			
			PHALCON_INIT_VAR(row);
			PHALCON_CALL_METHOD_PARAMS_1(row, result, "fetcharray", result, PH_NO_CHECK);
			if (zend_is_true(row)) {
				PHALCON_INIT_VAR(model);
				phalcon_read_property(&model, this_ptr, SL("_model"), PH_NOISY_CC);
				
				PHALCON_INIT_VAR(active_row);
				PHALCON_CALL_STATIC_PARAMS_2(active_row, "phalcon\\mvc\\model", "dumpresult", model, row);
				phalcon_update_property_zval(this_ptr, SL("_activeRow"), active_row TSRMLS_CC);
				PHALCON_MM_RESTORE();
				RETURN_TRUE;
			} else {
				PHALCON_INIT_VAR(c1);
				ZVAL_LONG(c1, 2);
				PHALCON_CALL_METHOD_PARAMS_1_NORETURN(result, "setfetchmode", c1, PH_NO_CHECK);
			}
		}
	} else {
		PHALCON_INIT_VAR(rows);
		phalcon_read_property(&rows, this_ptr, SL("_rows"), PH_NOISY_CC);
		Z_SET_ISREF_P(rows);
		
		PHALCON_INIT_VAR(row);
		PHALCON_CALL_FUNC_PARAMS_1(row, "current", rows);
		Z_UNSET_ISREF_P(rows);
		if (zend_is_true(row)) {
			Z_SET_ISREF_P(rows);
			PHALCON_CALL_FUNC_PARAMS_1_NORETURN("next", rows);
			Z_UNSET_ISREF_P(rows);
			phalcon_update_property_zval(this_ptr, SL("_activeRow"), row TSRMLS_CC);
			PHALCON_MM_RESTORE();
			RETURN_TRUE;
		}
	}
	PHALCON_MM_RESTORE();
	RETURN_FALSE;
}

/**
 * Returns current row in the resultset
 *
 * @return Phalcon\Mvc\Model
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, current){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_activeRow"), PH_NOISY_CC);
	
	RETURN_CCTOR(t0);
}

/**
 * Moves cursor to next row in the resultset
 *
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, next){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_pointer"), PH_NOISY_CC);
	PHALCON_SEPARATE_NMO(t0);
	increment_function(t0);
	phalcon_update_property_zval(this_ptr, SL("_pointer"), t0 TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

/**
 * Gets pointer number of active row in the resultset
 *
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, key){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_pointer"), PH_NOISY_CC);
	
	RETURN_CCTOR(t0);
}

/**
 * Rewinds resultset to its beginning
 *
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, rewind){

	zval *result = NULL;
	zval *t0 = NULL, *t1 = NULL;
	zval *c0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_type"), PH_NOISY_CC);
	if (zend_is_true(t0)) {
		PHALCON_INIT_VAR(result);
		phalcon_read_property(&result, this_ptr, SL("_result"), PH_NOISY_CC);
		if (Z_TYPE_P(result) != IS_BOOL || (Z_TYPE_P(result) == IS_BOOL && Z_BVAL_P(result))) {
			phalcon_update_property_long(this_ptr, SL("pointer"), 1 TSRMLS_CC);
			
			PHALCON_INIT_VAR(c0);
			ZVAL_LONG(c0, 0);
			PHALCON_CALL_METHOD_PARAMS_1_NORETURN(result, "dataseek", c0, PH_NO_CHECK);
		}
	} else {
		PHALCON_ALLOC_ZVAL_MM(t1);
		phalcon_read_property(&t1, this_ptr, SL("_rows"), PH_NOISY_CC);
		Z_SET_ISREF_P(t1);
		PHALCON_CALL_FUNC_PARAMS_1_NORETURN("reset", t1);
		Z_UNSET_ISREF_P(t1);
	}
	
	PHALCON_MM_RESTORE();
}

/**
 * Changes internal pointer to a specific position in the resultset
 *
 * @param int $position
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, seek){

	zval *position = NULL, *int_position = NULL, *type = NULL, *result = NULL, *rows = NULL;
	zval *i = NULL;
	zval *r0 = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &position) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(int_position);
	PHALCON_CALL_FUNC_PARAMS_1(int_position, "intval", position);
	phalcon_update_property_zval(this_ptr, SL("_pointer"), int_position TSRMLS_CC);
	
	PHALCON_INIT_VAR(type);
	phalcon_read_property(&type, this_ptr, SL("_type"), PH_NOISY_CC);
	if (zend_is_true(type)) {
		PHALCON_INIT_VAR(result);
		phalcon_read_property(&result, this_ptr, SL("_result"), PH_NOISY_CC);
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(result, "dataseek", position, PH_NO_CHECK);
	} else {
		PHALCON_INIT_VAR(rows);
		phalcon_read_property(&rows, this_ptr, SL("_rows"), PH_NOISY_CC);
		Z_SET_ISREF_P(rows);
		PHALCON_CALL_FUNC_PARAMS_1_NORETURN("reset", rows);
		Z_UNSET_ISREF_P(rows);
		
		PHALCON_INIT_VAR(i);
		ZVAL_LONG(i, 0);
		fs_ebc1_0:
			
			PHALCON_INIT_VAR(r0);
			is_smaller_function(r0, i, position TSRMLS_CC);
			if (!zend_is_true(r0)) {
				goto fe_ebc1_0;
			}
			Z_SET_ISREF_P(rows);
			PHALCON_CALL_FUNC_PARAMS_1_NORETURN("next", rows);
			Z_UNSET_ISREF_P(rows);
			PHALCON_SEPARATE(i);
			increment_function(i);
			goto fs_ebc1_0;
		fe_ebc1_0:
		if(0){}
	}
	
	PHALCON_MM_RESTORE();
}

/**
 * Counts how many rows are in the resultset
 *
 * @return int
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, count){

	zval *count = NULL, *type = NULL, *result = NULL, *number_rows = NULL, *rows = NULL;
	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(count);
	phalcon_read_property(&count, this_ptr, SL("_count"), PH_NOISY_CC);
	if (Z_TYPE_P(count) == IS_NULL) {
		PHALCON_INIT_VAR(count);
		ZVAL_LONG(count, 0);
		
		PHALCON_INIT_VAR(type);
		phalcon_read_property(&type, this_ptr, SL("_type"), PH_NOISY_CC);
		if (zend_is_true(type)) {
			PHALCON_INIT_VAR(result);
			phalcon_read_property(&result, this_ptr, SL("_result"), PH_NOISY_CC);
			if (Z_TYPE_P(result) != IS_BOOL || (Z_TYPE_P(result) == IS_BOOL && Z_BVAL_P(result))) {
				PHALCON_INIT_VAR(number_rows);
				PHALCON_CALL_METHOD(number_rows, result, "numrows", PH_NO_CHECK);
				
				PHALCON_INIT_VAR(count);
				PHALCON_CALL_FUNC_PARAMS_1(count, "intval", number_rows);
			}
		} else {
			PHALCON_INIT_VAR(rows);
			phalcon_read_property(&rows, this_ptr, SL("_rows"), PH_NOISY_CC);
			
			PHALCON_INIT_VAR(count);
			phalcon_fast_count(count, rows TSRMLS_CC);
		}
		
		phalcon_update_property_zval(this_ptr, SL("_count"), count TSRMLS_CC);
	}
	
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_count"), PH_NOISY_CC);
	
	RETURN_CCTOR(t0);
}

/**
 * Checks whether offset exists in the resultset
 *
 * @param int $index
 * @return boolean
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, offsetExists){

	zval *index = NULL, *count = NULL, *exists = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(count);
	PHALCON_CALL_METHOD(count, this_ptr, "count", PH_NO_CHECK);
	
	PHALCON_INIT_VAR(exists);
	is_smaller_function(exists, index, count TSRMLS_CC);
	if (zend_is_true(exists)) {
		PHALCON_MM_RESTORE();
		RETURN_TRUE;
	}
	
	PHALCON_MM_RESTORE();
	RETURN_FALSE;
}

/**
 * Gets row in a specific position of the resultset
 *
 * @param int $index
 * @return Phalcon\Mvc\Model
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, offsetGet){

	zval *index = NULL, *count = NULL, *exists = NULL, *valid = NULL, *current = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(count);
	PHALCON_CALL_METHOD(count, this_ptr, "count", PH_NO_CHECK);
	
	PHALCON_INIT_VAR(exists);
	is_smaller_function(exists, index, count TSRMLS_CC);
	if (zend_is_true(exists)) {
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(this_ptr, "seek", index, PH_NO_CHECK);
		
		PHALCON_INIT_VAR(valid);
		PHALCON_CALL_METHOD(valid, this_ptr, "valid", PH_NO_CHECK);
		if (Z_TYPE_P(valid) != IS_BOOL || (Z_TYPE_P(valid) == IS_BOOL && Z_BVAL_P(valid))) {
			PHALCON_INIT_VAR(current);
			PHALCON_CALL_METHOD(current, this_ptr, "current", PH_NO_CHECK);
			
			RETURN_CCTOR(current);
		} else {
			PHALCON_MM_RESTORE();
			RETURN_FALSE;
		}
	}
	
	PHALCON_THROW_EXCEPTION_STR(phalcon_mvc_model_exception_ce, "The index does not exist in the cursor");
	return;
}

/**
 * Resulsets cannot be changed. It has only been implemented to meet the definition of the ArrayAccess interface
 *
 * @param int $index
 * @param Phalcon\Mvc\Model $value
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, offsetSet){

	zval *index = NULL, *value = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &index, &value) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_THROW_EXCEPTION_STR(phalcon_mvc_model_exception_ce, "Cursor is an immutable ArrayAccess object");
	return;
}

/**
 * Resulsets cannot be changed. It has only been implemented to meet the definition of the ArrayAccess interface
 *
 * @param int $offset
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, offsetUnset){

	zval *offset = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_THROW_EXCEPTION_STR(phalcon_mvc_model_exception_ce, "Cursor is an immutable ArrayAccess object");
	return;
}

/**
 * Get first row in the resultset
 *
 * @return Phalcon\Mvc\Model
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, getFirst){

	zval *valid = NULL, *current = NULL;

	PHALCON_MM_GROW();
	PHALCON_CALL_METHOD_NORETURN(this_ptr, "rewind", PH_NO_CHECK);
	
	PHALCON_INIT_VAR(valid);
	PHALCON_CALL_METHOD(valid, this_ptr, "valid", PH_NO_CHECK);
	if (Z_TYPE_P(valid) != IS_BOOL || (Z_TYPE_P(valid) == IS_BOOL && Z_BVAL_P(valid))) {
		PHALCON_INIT_VAR(current);
		PHALCON_CALL_METHOD(current, this_ptr, "current", PH_NO_CHECK);
		
		RETURN_CCTOR(current);
	}
	
	PHALCON_MM_RESTORE();
	RETURN_FALSE;
}

/**
 * Get last row in the resultset
 *
 * @return Phalcon\Mvc\Model
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, getLast){

	zval *count = NULL, *pre_count = NULL, *valid = NULL, *current = NULL;
	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(count);
	PHALCON_CALL_METHOD(count, this_ptr, "count", PH_NO_CHECK);
	
	PHALCON_INIT_VAR(t0);
	ZVAL_LONG(t0, 1);
	
	PHALCON_INIT_VAR(pre_count);
	sub_function(pre_count, count, t0 TSRMLS_CC);
	PHALCON_CALL_METHOD_PARAMS_1_NORETURN(this_ptr, "seek", pre_count, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(valid);
	PHALCON_CALL_METHOD(valid, this_ptr, "valid", PH_NO_CHECK);
	if (Z_TYPE_P(valid) != IS_BOOL || (Z_TYPE_P(valid) == IS_BOOL && Z_BVAL_P(valid))) {
		PHALCON_INIT_VAR(current);
		PHALCON_CALL_METHOD(current, this_ptr, "current", PH_NO_CHECK);
		
		RETURN_CCTOR(current);
	}
	
	PHALCON_MM_RESTORE();
	RETURN_FALSE;
}

/**
 * Tell if the resultset if fresh or an old cached
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, isFresh){

	zval *type = NULL, *is_fresh = NULL;
	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(type);
	phalcon_read_property(&type, this_ptr, SL("_type"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(t0);
	ZVAL_LONG(t0, 1);
	
	PHALCON_INIT_VAR(is_fresh);
	is_equal_function(is_fresh, type, t0 TSRMLS_CC);
	
	RETURN_NCTOR(is_fresh);
}

/**
 * Returns the associated cache for the resultset
 *
 * @return Phalcon\Cache\Backend
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, getCache){

	zval *cache = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(cache);
	phalcon_read_property(&cache, this_ptr, SL("_cache"), PH_NOISY_CC);
	
	RETURN_CCTOR(cache);
}

/**
 * Returns an instance of the model that is used to generate each of the results
 *
 * @return Phalcon\Mvc\Model
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, getSourceModel){

	zval *model = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(model);
	phalcon_read_property(&model, this_ptr, SL("_model"), PH_NOISY_CC);
	
	RETURN_CCTOR(model);
}

/**
 * Serializing a resultset will dump all related rows into a big array
 *
 * @return string
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, serialize){

	zval *records = NULL, *valid = NULL, *current = NULL, *cache = NULL, *data = NULL, *serialized = NULL;
	zval *r0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(records);
	array_init(records);
	PHALCON_CALL_METHOD_NORETURN(this_ptr, "rewind", PH_NO_CHECK);
	ws_ebc1_1:
		
		PHALCON_INIT_VAR(r0);
		PHALCON_CALL_METHOD(r0, this_ptr, "valid", PH_NO_CHECK);
		PHALCON_CPY_WRT(valid, r0);
		if (Z_TYPE_P(valid) != IS_BOOL || (Z_TYPE_P(valid) == IS_BOOL && !Z_BVAL_P(valid))) {
			goto we_ebc1_1;
		}
		PHALCON_INIT_VAR(current);
		PHALCON_CALL_METHOD(current, this_ptr, "current", PH_NO_CHECK);
		phalcon_array_append(&records, current, PH_SEPARATE TSRMLS_CC);
		PHALCON_CALL_METHOD_NORETURN(this_ptr, "next", PH_NO_CHECK);
		goto ws_ebc1_1;
	we_ebc1_1:
	
	PHALCON_INIT_VAR(cache);
	phalcon_read_property(&cache, this_ptr, SL("_cache"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(data);
	array_init(data);
	phalcon_array_update_string(&data, SL("cache"), &cache, PH_COPY | PH_SEPARATE TSRMLS_CC);
	phalcon_array_update_string(&data, SL("rows"), &records, PH_COPY | PH_SEPARATE TSRMLS_CC);
	
	PHALCON_INIT_VAR(serialized);
	PHALCON_CALL_FUNC_PARAMS_1(serialized, "serialize", data);
	
	RETURN_CCTOR(serialized);
}

/**
 * Unserializing a resultset will allow to only works on the rows present in the saved state
 *
 * @param string $data
 */
PHP_METHOD(Phalcon_Mvc_Model_Resultset, unserialize){

	zval *data = NULL, *resultset = NULL, *rows = NULL, *cache = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &data) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	phalcon_update_property_long(this_ptr, SL("_type"), 0 TSRMLS_CC);
	
	PHALCON_INIT_VAR(resultset);
	PHALCON_CALL_FUNC_PARAMS_1(resultset, "unserialize", data);
	if (Z_TYPE_P(resultset) == IS_ARRAY) { 
		PHALCON_INIT_VAR(rows);
		phalcon_array_fetch_string(&rows, resultset, SL("rows"), PH_NOISY_CC);
		phalcon_update_property_zval(this_ptr, SL("_rows"), rows TSRMLS_CC);
		
		PHALCON_INIT_VAR(cache);
		phalcon_array_fetch_string(&cache, resultset, SL("cache"), PH_NOISY_CC);
		phalcon_update_property_zval(this_ptr, SL("_cache"), cache TSRMLS_CC);
	} else {
		PHALCON_THROW_EXCEPTION_STR(phalcon_mvc_model_exception_ce, "Invalid serialization data");
		return;
	}
	
	PHALCON_MM_RESTORE();
}

