/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Johann-Peter Hartmann <hartmann@php.net>                     |
  +----------------------------------------------------------------------+
*/

/* $Id: header 297205 2010-03-30 21:09:07Z johannes $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_strcmp95.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define NOTNUM(c)   ((c>57) || (c<48))
#define INRANGE(c)  ((c>0)  && (c<91))

/* If you declare any globals in php_strcmp95.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(strcmp95)
*/

/* True global resources - no need for thread safety here */
static int le_strcmp95;

/* {{{ strcmp95_functions[]
 *
 * Every user visible function must have an entry in strcmp95_functions[].
 */
const zend_function_entry strcmp95_functions[] = {
	PHP_FE(strcmp95_compare,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in strcmp95_functions[] */
};
/* }}} */

/* {{{ strcmp95_module_entry
 */
zend_module_entry strcmp95_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"strcmp95",
	strcmp95_functions,
	PHP_MINIT(strcmp95),
	PHP_MSHUTDOWN(strcmp95),
	PHP_RINIT(strcmp95),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(strcmp95),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(strcmp95),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_STRCMP95
ZEND_GET_MODULE(strcmp95)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("strcmp95.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_strcmp95_globals, strcmp95_globals)
    STD_PHP_INI_ENTRY("strcmp95.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_strcmp95_globals, strcmp95_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_strcmp95_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_strcmp95_init_globals(zend_strcmp95_globals *strcmp95_globals)
{
	strcmp95_globals->global_value = 0;
	strcmp95_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(strcmp95)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(strcmp95)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(strcmp95)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(strcmp95)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(strcmp95)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "strcmp95 support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

/* {{{ proto float strcmp95_compare(string ying, string yang [, bool probability [, bool ignorecase] ])
    */
PHP_FUNCTION(strcmp95_compare)
{
	char *ying = NULL;
	char *yang = NULL;
	int argc = ZEND_NUM_ARGS();
	int ying_arg_len;
	int yang_arg_len;
	zend_bool probability;
    zend_bool ignorecase;

	if (zend_parse_parameters(argc TSRMLS_CC, "ss|bb", &ying, &ying_arg_len, &yang, &yang_arg_len, &probability, &ignorecase) == FAILURE) 
		return;
        
	static  int     pass=0, adjwt[91][91];
	static  char    sp[39][2] =
		{'A','E',  'A','I',  'A','O',  'A','U',  'B','V',  'E','I',  'E','O',  'E','U',
		 'I','O',  'I','U',  'O','U',  'I','Y',  'E','Y',  'C','G',  'E','F',
		 'W','U',  'W','V',  'X','K',  'S','Z',  'X','S',  'Q','C',  'U','V',
		 'M','N',  'L','I',  'Q','O',  'P','R',  'I','J',  '2','Z',  '5','S',
		 '8','B',  '1','I',  '1','L',  '0','O',  '0','Q',  'C','K',  'G','J',
		 'E',' ',  'Y',' ',  'S',' '};

	char*    ying_hold,
		*   yang_hold,
		*   ying_flag,
		*   yang_flag;

	double  weight, Num_sim;

	long    minv,   search_range,   lowlim,    ying_length,
		hilim,  N_trans,        Num_com,   yang_length;

	int     yl1,    yi_st,  N_simi;

	register        int     i,      j,      k;

	/* Initialize the adjwt array on the first call to the function only.
	   The adjwt array is used to give partial credit for characters that
	   may be errors due to known phonetic or character recognition errors.
	   A typical example is to match the letter "O" with the number "0"           */
	if (!pass) {
		pass++;
		for (i=0; i<91; i++) for (j=0; j<91; j++) adjwt[i][j] = 0;
		for (i=0; i<36; i++) {
			adjwt[sp[i][0]][sp[i][1]] = 3;
			adjwt[sp[i][1]][sp[i][0]] = 3;
		} }

	/* Identify the strings to be compared by stripping off all leading and
	   trailing spaces.                                                           */
	k = ying_arg_len - 1;
	for(j = 0;((ying[j]==' ') && (j < k));j++);
	for(i = k;((ying[i]==' ') && (i > 0));i--);
	ying_length = i + 1 - j;
	yi_st = j;

	for(j = 0;((yang[j]==' ') && (j < k));j++);
	for(i = k;((yang[i]==' ') && (i > 0));i--);
	yang_length = i + 1 - j;
	/* If either string is blank - return - added in Version 2                    */
	if(ying_length <= 0 || yang_length <= 0) RETURN_FALSE;

	ying_hold = (char*) emalloc(sizeof(char) * (ying_length + 1));
	yang_hold = (char*) emalloc(sizeof(char) * (yang_length + 1));

	strncpy(ying_hold,&ying[yi_st],ying_length);
	strncpy(yang_hold,&yang[j],yang_length);

	if (ying_length > yang_length) {
		search_range = ying_length;
		minv = yang_length;
	}
	else {
		search_range = yang_length;
		minv = ying_length;
	}

	/* If either string is blank - return                                         */
	/* if (!minv) return(0.0);                   removed in version 2             */

	/* Blank out the flags                                                        */
	ying_flag = (char*) emalloc(sizeof(char) * search_range);
	yang_flag = (char*) emalloc(sizeof(char) * search_range);

	memset(ying_flag, ' ', search_range);
	memset(yang_flag, ' ', search_range);
	search_range = (search_range/2) - 1;
	if (search_range < 0) search_range = 0;   /* added in version 2               */

	/* Convert all lower case characters to upper case.                           */
	if (!ignorecase) {
		for (i = 0;i < ying_length;i++) if (islower(ying_hold[i])) ying_hold[i] -= 32;
		for (j = 0;j < yang_length;j++) if (islower(yang_hold[j])) yang_hold[j] -= 32;
	}

		/* Looking only within the search range, count and flag the matched pairs.    */
		Num_com = 0;
		yl1 = yang_length - 1;
		for (i = 0;i < ying_length;i++) {
			lowlim = (i >= search_range) ? i - search_range : 0;
			hilim = ((i + search_range) <= yl1) ? (i + search_range) : yl1;
			for (j = lowlim;j <= hilim;j++)  {
				if ((yang_flag[j] != '1') && (yang_hold[j] == ying_hold[i])) {
					yang_flag[j] = '1';
					ying_flag[i] = '1';
					Num_com++;
					break;
				} } }

		/* If no characters in common - return                                        */
		if (!Num_com){
			efree(ying_hold);
			efree(yang_hold);
			efree(ying_flag);
			efree(yang_flag);
			RETURN_DOUBLE(0.0);
		}

		/* Count the number of transpositions                                         */
		k = N_trans = 0;
		for (i = 0;i < ying_length;i++) {
			if (ying_flag[i] == '1') {
				for (j = k;j < yang_length;j++) {
					if (yang_flag[j] == '1') {
						k = j + 1;
						break;
					} }
				if (ying_hold[i] != yang_hold[j]) N_trans++;
			} }
		N_trans = N_trans / 2;

		/* adjust for similarities in nonmatched characters                           */
		N_simi = 0;
		if (minv > Num_com) {
			for (i = 0;i < ying_length;i++) {
				if (ying_flag[i] == ' ' && INRANGE(ying_hold[i])) {
					for (j = 0;j < yang_length;j++) {
						if (yang_flag[j] == ' ' && INRANGE(yang_hold[j])) {
							if (adjwt[ying_hold[i]][yang_hold[j]] > 0) {
								N_simi += adjwt[ying_hold[i]][yang_hold[j]];
								yang_flag[j] = '2';
								break;
							} } } } } }
		Num_sim = ((double) N_simi)/10.0 + Num_com;

		/* Main weight computation.                                                   */
		weight= Num_sim / ((double) ying_length) + Num_sim / ((double) yang_length)
			+ ((double) (Num_com - N_trans)) / ((double) Num_com);
		weight = weight / 3.0;

		/* Continue to boost the weight if the strings are similar                    */
		if (weight > 0.7) {

			/* Adjust for having up to the first 4 characters in common                 */
			j = (minv >= 4) ? 4 : minv;
			for (i=0;((i<j)&&(ying_hold[i]==yang_hold[i])&&(NOTNUM(ying_hold[i])));i++);
			if (i) weight += i * 0.1 * (1.0 - weight);

			/* Optionally adjust for long strings.                                      */
			/* After agreeing beginning chars, at least two more must agree and
			   the agreeing characters must be > .5 of remaining characters.          */
			if ((!probability) && (minv>4) && (Num_com>i+1) && (2*Num_com>=minv+i))
				if (NOTNUM(ying_hold[0]))
					weight += (double) (1.0-weight) *
						((double) (Num_com-i-1) / ((double) (ying_length+yang_length-i*2+2)));
		}

    efree(ying_hold);
    efree(yang_hold);
    efree(ying_flag);
    efree(yang_flag);
    RETURN_DOUBLE(weight);

}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
