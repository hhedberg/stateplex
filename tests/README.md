Googletest Tests
================

Here are all the test cases that have been used for the Stateplex library.

First, you must create a folder to this folder. After that place your module's tests in to the folder you created. If you want you can sort out your tests by placing them in to different folders.

How to write new test cases
===========================
	
Step one
--------
Edit your configure file and set variable gtest_dir to point to your installation of Google unit testing framework. e.g. gtest_dir=/home/user/gtest/gtest-1.6.0/src
		
Step two
--------
Your tests should include at least one <testcase>.cpp file for declaring the test class for a selected set of similar tests, and a runalltests.cpp for containing the main function for running all test cases.

Step three
----------
Explicitly include file gtest/gtest.h, from your local drive where you have installed it, to all your testcase files: <testcase>.cpp and runalltests.cpp.
	
Step four
---------
Declare a new test class to a file <testcase>.cpp for similar set of tests e.g. class ListTest, and  inherit it publicly from the Google test framework, testing::Test, e.g.  class ListTest : public testing::Test.
    
Step five
---------
In the test class, you can define two functions: virtual void SetUp(){} and virtual void TearDown(){} to make set ups and cleaning ups for each tests: framework executes  SetUp() in the beginning of each test and TearDown() in the end of each test respectively.

Step six
--------
Define your tests using a TEST(<test case name>, <test name>){} -macro or test fixture macro TEST_F(<test case name>, <test name>){}. These macros take two parameters: the test case name and the test name. You should use test fixtures if you are planning several tests that are using the same data in tests.

Step seven
--------
Include header file of the file you are going to test - usually one test class per each tested header&cpp file is fine - to your test case file (<testcase>.cpp).

Step eight
--------
Add your tests to the body of the macros using assertions (ASSERT_*, which, if fails, stops the execution, or EXCPECT_* , which, if fails, continues the execution from the next test, and provides a short description of the reason for failure). Each test fixture in a test case is an individual test, and it is deleted after the test is finished.

Step nine
--------
For sharing resources between several tests, two static void functions are provided: void static SetUpTestCases(){} and void static TearDownTestCases(){}. SetUpTestCases() is for declaring shared variables, which are declared in the beginning of the test case and not deleted in between different tests. TeadDownTestCases() is for doing cleaning up such as deleting dynamically created memory. It is done only in the end of the test case.

Step ten
--------
make test and run your test from build folder.

More details
------------
See http://code.google.com/p/googletest/ for more details


