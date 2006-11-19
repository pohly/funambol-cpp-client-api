/*
 * Copyright (C) 2005 Patrick Ohly
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestListener.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFailure.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#ifdef AUTOTOOLS
# include <config.h>
#endif

#include <base/fscapi.h>
#include <base/Log.h>
#include <base/test.h>

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#include <string>
#include <stdexcept>
using namespace std;

class ClientOutputter : public CppUnit::CompilerOutputter {
public:
    ClientOutputter(CppUnit::TestResultCollector *result, std::ostream &stream) :
        CompilerOutputter(result, stream) {}
    void write() {
        // ensure that output goes to console again
        LOG.setLogName("test.log");
        CompilerOutputter::write();
    }
};

class ClientListener : public CppUnit::TestListener {
public:
    ClientListener() :
        m_failed(false) {
#ifdef HAVE_SIGNAL_H
        // install signal handler which turns an alarm signal into a runtime exception
        // to abort tests which run too long
        const char *alarm = getenv("CLIENT_TEST_ALARM");
        m_alarmSeconds = alarm ? atoi(alarm) : -1;

        struct sigaction action;
        memset(&action, 0, sizeof(action));
        action.sa_handler = alarmTriggered;
        action.sa_flags = SA_NOMASK;
        sigaction(SIGALRM, &action, NULL);
#endif
    }

    void addAllowedFailures(string allowedFailures) {
        size_t start = 0, end;
        while ((end = allowedFailures.find(',', start)) != allowedFailures.npos) {
            size_t len = end - start;
            if (len) {
                m_allowedFailures.insert(allowedFailures.substr(start, len));
            }
            start = end + 1;
        }
        if (allowedFailures.size() > start) {
            m_allowedFailures.insert(allowedFailures.substr(start));
        }
    }

    void startTest (CppUnit::Test *test) {
        m_currentTest = test->getName();
        LOG.setLogName("test.log");
        cerr << m_currentTest;
        string logfile = m_currentTest + ".log";
        simplifyFilename(logfile);
        remove(logfile.c_str());
        LOG.setLogName(logfile.c_str());
        m_testFailed = false;

#ifdef HAVE_SIGNAL_H
        if (m_alarmSeconds > 0) {
            alarm(m_alarmSeconds);
        }
#endif HAVE_SIGNAL_H
    }

    void addFailure(const CppUnit::TestFailure &failure) {
        m_testFailed = true;
    }

    void endTest (CppUnit::Test *test) {
#ifdef HAVE_SIGNAL_H
        if (m_alarmSeconds > 0) {
            alarm(0);
        }
#endif

        LOG.setLogName("test.log");
        if (m_testFailed) {
            if (m_allowedFailures.find(m_currentTest) == m_allowedFailures.end()) {
                cerr << " *** failed ***";
                m_failed = true;
            } else {
                cerr << " *** failure ignored ***";
            }
        }
        cerr << "\n";
    }

    bool hasFailed() { return m_failed; }
    const string &getCurrentTest() const { return m_currentTest; }

private:
    set<string> m_allowedFailures;
    bool m_failed, m_testFailed;
    string m_currentTest;
    int m_alarmSeconds;

    static void alarmTriggered(int signal) {
        CPPUNIT_ASSERT_MESSAGE(false, "test timed out");
    }
} syncListener;

const string &getCurrentTest() {
    return syncListener.getCurrentTest();
}

void simplifyFilename(string &filename)
{
    size_t pos = 0;
    while (true) {
        pos = filename.find(":", pos);
        if (pos == filename.npos ) {
            break;
        }
        filename.replace(pos, 1, "_");
    }
}

int main(int argc, char* argv[])
{
  // Get the top level suite from the registry
  CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

  // Adds the test to the list of test to run
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( suite );

  // Change the default outputter to a compiler error format outputter
  runner.setOutputter( new ClientOutputter( &runner.result(),
                                            std::cerr ) );

  // track current test and failure state
  const char *allowedFailures = getenv("CLIENT_TEST_FAILURES");
  if (allowedFailures) {
      syncListener.addAllowedFailures(allowedFailures);
  }
  runner.eventManager().addListener(&syncListener);

  try {
      // Run the tests.
      if (argc <= 1) {
          // all tests
          runner.run("", false, true, false);
      } else {
          // run selected tests individually
          for (int test = 1; test < argc; test++) {
              runner.run(argv[test], false, true, false);
          }
      }

      // Return error code 1 if the one of test failed.
      return syncListener.hasFailed() ? 1 : 0;
  } catch (invalid_argument e) {
      // Test path not resolved
      std::cerr << std::endl  
                << "ERROR: " << e.what()
                << std::endl;
      return 1;
  }
}
