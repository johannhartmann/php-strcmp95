dnl $Id$
dnl config.m4 for extension strcmp95

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(strcmp95, for strcmp95 support,
dnl Make sure that the comment is aligned:
dnl [  --with-strcmp95             Include strcmp95 support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(strcmp95, whether to enable strcmp95 support,
dnl Make sure that the comment is aligned:
[  --enable-strcmp95           Enable strcmp95 support])

if test "$PHP_STRCMP95" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-strcmp95 -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/strcmp95.h"  # you most likely want to change this
  dnl if test -r $PHP_STRCMP95/$SEARCH_FOR; then # path given as parameter
  dnl   STRCMP95_DIR=$PHP_STRCMP95
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for strcmp95 files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       STRCMP95_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$STRCMP95_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the strcmp95 distribution])
  dnl fi

  dnl # --with-strcmp95 -> add include path
  dnl PHP_ADD_INCLUDE($STRCMP95_DIR/include)

  dnl # --with-strcmp95 -> check for lib and symbol presence
  dnl LIBNAME=strcmp95 # you may want to change this
  dnl LIBSYMBOL=strcmp95 # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $STRCMP95_DIR/lib, STRCMP95_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_STRCMP95LIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong strcmp95 lib version or lib not found])
  dnl ],[
  dnl   -L$STRCMP95_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(STRCMP95_SHARED_LIBADD)

  PHP_NEW_EXTENSION(strcmp95, strcmp95.c, $ext_shared)
fi
