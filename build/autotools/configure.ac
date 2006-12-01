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

AC_ARG_ENABLE(unit-tests,
              AS_HELP_STRING([--enable-unit-tests],
                             [enables tests embedded in the source code of the library (changes content of the library)]),
              enable_unit_tests="$enableval", enable_unit_tests="no")
AC_ARG_ENABLE(integration-tests,
              AS_HELP_STRING([--enable-integration-tests],
                             [enables tests outside of the library (can be used together with normal builds of the library)]),
              enable_integration_tests="$enableval", enable_integration_tests="no")

# cppunit needed?
if test $enable_unit_tests == "yes" || test $enable_integration_tests == yes; then
        CPPUNIT_CXXFLAGS=`cppunit-config --cflags` || AC_MSG_ERROR("cppunit-config --cflags failed - is it installed?")
        CPPUNIT_LDFLAGS=`cppunit-config --libs` || AC_MSG_ERROR("cppunit-config --libs failed - is it installed?")
        CPPUNIT_TESTS=client-test
        AC_CHECK_HEADERS(signal.h)
fi
AC_SUBST(CPPUNIT_CXXFLAGS)
AC_SUBST(CPPUNIT_LDFLAGS)
AC_SUBST(CPPUNIT_TESTS)

if test $enable_unit_tests == "yes"; then
        AC_DEFINE(ENABLE_UNIT_TESTS, 1, [enable unit tests inside the library's source code])
fi
if test $enable_integration_tests == "yes"; then
        AC_DEFINE(ENABLE_INTEGRATION_TESTS, 1, [enable unit tests outside of the library's source code])
fi


AC_OUTPUT(Makefile src/Makefile include/Makefile test/Makefile)
