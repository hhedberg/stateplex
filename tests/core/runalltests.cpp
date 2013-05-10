/*
 * runalltests.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: maaalto
 */
#include "gtest/gtest.h"

/*
 * Main function to run all the Google unit tests
 */
int main(int argc, char ** argv)
{
	::testing::InitGoogleTest(&argc,argv);
	return RUN_ALL_TESTS();
}
