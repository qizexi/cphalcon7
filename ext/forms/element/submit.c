
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

#include "forms/element/submit.h"
#include "forms/element.h"
#include "forms/elementinterface.h"
#include "forms/element/helpers.h"
#include "kernel/main.h"

/**
 * Phalcon\Forms\Element\Submit
 *
 * Component INPUT[type=submit] for forms
 */
zend_class_entry *phalcon_forms_element_submit_ce;

PHP_METHOD(Phalcon_Forms_Element_Submit, render);

static const zend_function_entry phalcon_forms_element_submit_method_entry[] = {
	PHP_ME(Phalcon_Forms_Element_Submit, render, arginfo_phalcon_forms_elementinterface_render, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Phalcon\Forms\Element\Submit initializer
 */
PHALCON_INIT_CLASS(Phalcon_Forms_Element_Submit){

	PHALCON_REGISTER_CLASS_EX(Phalcon\\Forms\\Element, Submit, forms_element_submit, phalcon_forms_element_ce, phalcon_forms_element_submit_method_entry, 0);

	zend_class_implements(phalcon_forms_element_submit_ce, 1, phalcon_forms_elementinterface_ce);

	return SUCCESS;
}

/**
 * Renders the element widget
 *
 * @param array $attributes
 * @return string
 */
PHP_METHOD(Phalcon_Forms_Element_Submit, render){

	phalcon_forms_element_render_helper("submitbutton", 0, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
