/*
 * bufferTests.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: maaalto
 */
#include <iostream>
#include "gtest/gtest.h"
#include "../../stateplex/core/buffer.h"
#include "../../stateplex/core/readbuffer.h"
#include "../../stateplex/core/writebuffer.h"
#include "../../stateplex/core/dispatcher.h"
#include "../../stateplex/core/actor.h"
#include "../../stateplex/core/string.h"

class BufferTest : public testing::Test {

protected:

        static Stateplex::Dispatcher *dispatcher;
      //  static Stateplex::Actor myActor;

        static void SetUpTestCase()
        {
                dispatcher = new Stateplex::Dispatcher();
        //        myActor = new Stateplex::Actor(dispatcher);

        }
        static void TearDownTestCase()
        {
        //      delete myActor;
                delete dispatcher;
        }

        virtual void SetUp(){}
        virtual void TearDown(){}

};

Stateplex::Dispatcher *BufferTest::dispatcher;
//Stateplex::Dispatcher *BufferTest::myActor;

        //Buffer tests

TEST_F(BufferTest, zeroLengthTest)
{
        Stateplex::Actor myActor(dispatcher);
        Stateplex::WriteBuffer<1024> inputBuffer1(&myActor);
        EXPECT_EQ(0, inputBuffer1.length());
}

TEST_F(BufferTest, appendCStringTest)
{
        Stateplex::Actor actor1(dispatcher);
        //Stateplex::Actor actor2(&dispatcher);

        Stateplex::WriteBuffer<1024> inputBuffer2(&actor1);
        //Stateplex::WriteBuffer<1024> inputBuffer4(&actor1);

        inputBuffer2.append("Marja");
        EXPECT_NE(inputBuffer2.length(), 0);

}

TEST_F(BufferTest, appendStringTest)
{
        Stateplex::Actor actor2(dispatcher);
        Stateplex::WriteBuffer<1024> inputBuffer3(&actor2);
        inputBuffer3.append(Stateplex::String::copy(actor2.allocator(), "Marja"));
        EXPECT_NE(inputBuffer3.length(), 0);
        EXPECT_TRUE(inputBuffer3.equals("Marja"));
        EXPECT_EQ(inputBuffer3.length(), 5);
}

TEST_F(BufferTest, appendBuffersTest)
{
        Stateplex::Actor actor3(dispatcher);
        Stateplex::Actor actor4(dispatcher);
        Stateplex::Actor actor5(dispatcher);
        Stateplex::WriteBuffer<> *buffer3 = new Stateplex::WriteBuffer<>(&actor3);
        Stateplex::WriteBuffer<> *buffer4 = new Stateplex::WriteBuffer<>(&actor4);
        Stateplex::WriteBuffer<> *buffer5 = new Stateplex::WriteBuffer<>(&actor5);

        buffer3->append("Marja ");
        EXPECT_TRUE(buffer3->equals("Marja "));
        EXPECT_EQ(buffer3->length(), 6);

        buffer4->append(Stateplex::String::copy(actor4.allocator(), "tries coding."));

        buffer3->append(buffer4);
        EXPECT_TRUE(buffer3->equals("Marja tries coding."));
        EXPECT_EQ(buffer3->length(), 19);

        buffer5->append(" And succeeds in it.", 13);
        buffer3->append(buffer5);

        EXPECT_EQ(buffer3->length(), 32);
        EXPECT_TRUE(buffer3->equals("Marja tries coding. And succeeds"));

        delete buffer3;
        delete buffer4;
        delete buffer5;
}
/*
//Iterator tests
TEST_F(BufferTest, regionTest)
{
        Stateplex::Actor *actor(dispatcher);
        Stateplex::WriteBuffer<> inputBuffer(actor);
        //Stateplex::WriteBuffer<> *buffer5 = new Stateplex::WriteBuffer<>.region(0, 1024);
        Stateplex::WriteBuffer<> *buffer6 = new inputBuffer.region(0, 1024);
        inputBuffer.popped(1024);
        EXPECT_EQ(inputBuffer.length(), 0);

        //delete buffer5;
        delete buffer6;
}

        //WriteBuffer tests
        TEST_F(BufferTest, splitTest)
        {

        }

        //ReadBuffer tests
        TEST_F(BufferTest, blockSizeTest)
        {
           std::size_t size;
        }
        TEST_F(BufferTest, IteratorTest)
        {

        }

        //WriteBuffer tests
        TEST_F(BufferTest, WriteBufferTest)
        {

        }

        //ReadBuffer tests
        TEST_F(BufferTest, ReadBufferTest)
        {

        }
*/


