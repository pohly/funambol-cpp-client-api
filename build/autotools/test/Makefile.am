TESTS = @CPPUNIT_TESTS@
EXTRA_PROGRAMS = client-test
check_PROGRAMS = @CPPUNIT_TESTS@

LDADD = ../src/libsync4j.la
AM_LDFLAGS = @CPPUNIT_LDFLAGS@
AM_CXXFLAGS = @CPPUNIT_CXXFLAGS@
AM_CPPFLAGS = -I$(srcdir)/../../../src/include/posix -I$(srcdir)/../../../src/include/common -DAUTOTOOLS

vpath %.cpp  $(srcdir)/../../../test

# With --disable-shared autotools links against libsync4j.a which does not
# pull any of the test suites into the test binary, so they would not be
# executed. The workaround is to explicitly set them as undefined on the
# link line.
client_test_SOURCES = client-test-main.cpp client-test.cpp ClientTest.cpp
client_test_LDFLAGS = `nm ../src/.libs/libsync4j.a | grep funambolAutoRegisterRegistry | sed -e 's/.* /-u /'`
# client_test_LDADD = libsync4j.la

# ensure that files are in the current directory before the check runs
check-am check: client-test-files
.PHONY: client-test-files
client-test-files:
	@ [ -e synccompare.pl ] || ln -s $(srcdir)/../../../test/synccompare.pl .
	@ [ -d testcases ] || ln -s $(srcdir)/../../../test/testcases .
	@ [ -d Algorithm ] || ln -s $(srcdir)/../../../test/Algorithm

# also install files in a test sub directory
nobase_pkginclude_HEADERS = \
        test/Algorithm/Artistic \
        test/Algorithm/copyright \
        test/Algorithm/Diff.pm \
        test/Algorithm/README \
	test/testcases/ical20.ics \
	test/testcases/itodo20.ics \
	test/testcases/vcal10.ics \
	test/testcases/vcard21.vcf \
	test/testcases/vcard30.vcf \
	test/testcases/imemo20.ics \
        test/README.txt \
        test/synccompare.pl \
	test/ClientTest.h \
	test/ClientTest.cpp \
	test/client-test-main.cpp

# fix files for public distribution
$(nobase_pkginclude_HEADERS): % : ../../../%
	mkdir -p `dirname $@`
	sed -e 's/WCHAR/char/g' -e 's/= *NULL/= 0/g' -e 's/__declspec([^)]*)//g' -e 's/__cdecl//g' -e 's/EXTRA_SECTION_[0123456789]*//g' -e 's/BOOL/int/g' -e 's/FALSE/0/g' -e 's/TRUE/1/g' $< >$@
