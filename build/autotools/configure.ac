dnl Process this file with autoconf to produce a configure script.
AC_INIT(../../src/c++/common/base/base64.cpp)
AM_INIT_AUTOMAKE(sync4j, 2.0)
AM_CONFIG_HEADER(config.h)

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL
dnl AM_GNU_GETTEXT - not currently used

# check for libcurl now instead of during make
AC_MSG_CHECKING([for libcurl])
if CURL_LDFLAGS=`sh -c 'curl-config --libs' 2>&AS_MESSAGE_LOG_FD` && \
   CURL_CFLAGS=`sh -c 'curl-config --cflags' 2>&AS_MESSAGE_LOG_FD`; then
        AC_MSG_RESULT([found])
        AC_SUBST(CURL_LDFLAGS)
        AC_SUBST(CURL_CFLAGS)
else
        AC_MSG_RESULT([missing])
        AC_MSG_ERROR([libcurl is required, check that its development package is installed and curl-config is in your PATH])
fi

dnl Check for certain headers.
AC_CHECK_HEADERS(time.h)


AC_OUTPUT(Makefile src/Makefile include/Makefile)
