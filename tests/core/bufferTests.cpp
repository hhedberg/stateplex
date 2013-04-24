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

        static void SetUpTestCase()
        {
        }
        static void TearDownTestCase()
        {
        }

        virtual void SetUp()
        {

         }
        virtual void TearDown()
        {

        }
        Stateplex::Dispatcher dispatcher;


};
        //Buffer tests

TEST_F(BufferTest, zeroLengthTest)
{


//      Stateplex::WriteBuffer<> inputBuffer2(&actor);
        Stateplex::Actor myActor(&dispatcher);
        Stateplex::WriteBuffer<1024> inputBuffer1(&myActor);
        EXPECT_EQ(0, inputBuffer1.length());
        delete &myActor;
}

TEST_F(BufferTest, appendTest)
{
        std::cout << "tama on append test";
        Stateplex::Actor actor2(&dispatcher);
        //Stateplex::Actor actor2(&dispatcher);

        Stateplex::WriteBuffer<1024> inputBuffer3(&actor2);
        //Stateplex::WriteBuffer<1024> inputBuffer4(&actor2);

        inputBuffer3.append("Marja");
        EXPECT_NE(inputBuffer3.length(), 0);
        delete &actor2;

        //inputBuffer4.append("yrittaa koodata turhaan", 15);
       // EXPECT_EQ(inputBuffer3.equals("Marja"), true);
        //std::cout << "buffer 1 " << inputBuffer3.length();
        //std::cout << "buffer 2 " << inputBuffer4.length();
        //EXPECT_EQ(inputBuffer3.length(), inputBuffer4.length());
}

TEST_F(BufferTest, appendStringTest)
{
        Stateplex::Actor actor1(&dispatcher);
        Stateplex::WriteBuffer<1024> inputBuffer2(&actor1);
        inputBuffer2.append(Stateplex::String::copy(actor1.allocator(), "Marja"));
        EXPECT_NE(inputBuffer2.length(), 0);
        delete &actor1;
}


/*
TEST_F(BufferTest, BufferTest1)
{
        Stateplex::Actor actor3(&dispatcher);
        Stateplex::Actor actor4(&dispatcher);
        Stateplex::WriteBuffer<> *buffer3 = new Stateplex::WriteBuffer<>(&actor3);
        Stateplex::WriteBuffer<> *buffer4 = new Stateplex::WriteBuffer<>(&actor4);
        //Stateplex::WriteBuffer<> *buffer3 = new Stateplex::WriteBuffer<>.region(0, 1024);
        //Stateplex::WriteBuffer<> *buffer4 = new Stateplex::WriteBuffer<>.region(0, 1024);

        buffer3->append(buffer4);
        //EXPECT_EQ(buffer3->length()+buffer4->length(), buffer3->length());
}

TEST_F(BufferTest, BufferTest2)
{

}
        //Iterator tests


        TEST_F(BufferTest, regionTest)
        {

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


