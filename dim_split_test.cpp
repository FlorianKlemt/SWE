/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "cxxtest";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_dim_split_test_init = false;
#include "/home/unknown/Dokumente/git/SWE/src/examples/dim_split_test.h"

static dim_split_test suite_dim_split_test;

static CxxTest::List Tests_dim_split_test = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_dim_split_test( "./src/examples/dim_split_test.h", 20, "dim_split_test", suite_dim_split_test, Tests_dim_split_test );

static class TestDescription_suite_dim_split_test_testSWE_dimsplit : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_dim_split_test_testSWE_dimsplit() : CxxTest::RealTestDescription( Tests_dim_split_test, suiteDescription_dim_split_test, 28, "testSWE_dimsplit" ) {}
 void runTest() { suite_dim_split_test.testSWE_dimsplit(); }
} testDescription_suite_dim_split_test_testSWE_dimsplit;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
