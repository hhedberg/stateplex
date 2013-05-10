#include "gtest/gtest.h"
#include "../../examples/hello/greeteractor.h"
#include "../../stateplex/core/dispatcher.h"


class DISABLED_DispatcherTest : public testing::Test {
protected:
	static Stateplex::Dispatcher *dispatcher;

	static void SetUpTestCase()
	{
	        dispatcher = new Stateplex::Dispatcher();
	}

	static void TearDownTestCase()
	{
	        delete dispatcher;
	}
	
	virtual void SetUp() {}

	virtual void TearDown() {}
	
};

Stateplex::Dispatcher *DISABLED_DispatcherTest::dispatcher;

TEST_F(DISABLED_DispatcherTest, ActivateActorTest)
{
	GreeterActor greeter(dispatcher);
	EXPECT_EQ(true, greeter.isAlive());
}
