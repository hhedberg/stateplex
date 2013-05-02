#include "gtest/gtest.h"
#include "../../examples/hello/greeteractor.h"
#include "../../stateplex/core/dispatcher.h"

class DispatcherTest : public testing::Test {
protected:
	
	virtual void SetUp() 
	{
		
	}

	virtual void TearDown() { }
	
	Stateplex::Dispatcher dispatcher;
};

TEST_F(DispatcherTest, ActivateActorTest)
{
	GreeterActor greeter(&dispatcher);
	EXPECT_EQ(true, greeter.isAlive());
}
