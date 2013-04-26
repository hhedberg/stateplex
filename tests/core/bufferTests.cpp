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

TEST_F(BufferTest, appendBuffersAndEqualsTest)
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

TEST_F(BufferTest, insertTest)
{
        Stateplex::Actor actor1(dispatcher);
        Stateplex::WriteBuffer<> *buffer1 = new Stateplex::WriteBuffer<>(&actor1);
        Stateplex::WriteBuffer<> *buffer2 = new Stateplex::WriteBuffer<>(&actor1);
        buffer1->insert(0xF867, "TestiCString");
        EXPECT_TRUE(buffer1->equals("TestiCString"));
        EXPECT_EQ(0, buffer1->compare("TestiCString"));
        EXPECT_EQ(0, buffer1->compare("TestiCString", 12));

        buffer2->insert(0xF867,"Ritaharju school", 6 );
        EXPECT_EQ(buffer2->length(), 6);
        EXPECT_TRUE(buffer2->equals("Ritaharju school", 6));

      //  buffer2->insert(0xF900, buffer1);
       // EXPECT_TRUE(buffer2->equals("TestiCString"));

        //EXPECT_EQ(buffer1->asString(1000, 12), String myString.copy("TestiCString");
        //EXPECT_EQ(buffer1->;

        delete buffer1;
        delete buffer2;
}

TEST_F(BufferTest, miscellaneousTests)
{
        Stateplex::Actor testActor(dispatcher);
        Stateplex::WriteBuffer<> *buffer = new Stateplex::WriteBuffer<>(&testActor);
        buffer->append("aaaab");
        EXPECT_EQ(buffer->actor(), &testActor);
        EXPECT_EQ(buffer->allocator(), testActor.allocator());
       // EXPECT_EQ(buffer->asString(), Stateplex::String::copy));
        delete buffer;
}

TEST_F(BufferTest, splitTest)
{
        Stateplex::Actor myActor2(dispatcher);
        Stateplex::WriteBuffer<> *buffer2 = new Stateplex::WriteBuffer<>(&myActor2);
        buffer2->append("Ritaharju school");
        buffer2->split('a', 4);
        EXPECT_TRUE(buffer2->equals("Rita"));
        delete buffer2;
}

TEST_F(BufferTest, regionTest)
{
        Stateplex::Actor actor(dispatcher);
        Stateplex::WriteBuffer<> *inputBuffer = new Stateplex::WriteBuffer<>(&actor);

        inputBuffer->region(0xF900, 100);
        EXPECT_EQ(inputBuffer->length(), 0);
        inputBuffer->append("foo");
        EXPECT_EQ(inputBuffer->length(), 3);

        delete inputBuffer;
}
/*
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

            TEST_F(BufferTest, pushToBlockTest)
        {
        }

    //Block Tests: room, ensureBlock, pushed, popped, split, allocateMemory, pushToBlock, splitBlock
        TEST_F(BufferTest, splitBlockTest)
        {

        }
     */


