/*
 * bufferTests.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: maaalto
 */
#include <iostream>
#include <unistd.h>
#include <fstream>
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
        static Stateplex::Actor *myActor;

        static void SetUpTestCase()
        {
                dispatcher = new Stateplex::Dispatcher();
                myActor = new Stateplex::Actor(dispatcher);

        }
        static void TearDownTestCase()
        {
                delete myActor;
                delete dispatcher;
        }

        virtual void SetUp(){}
        virtual void TearDown(){}

};

Stateplex::Dispatcher *BufferTest::dispatcher;
Stateplex::Actor *BufferTest::myActor;


//Buffer tests

TEST_F(BufferTest, zeroLengthTest)
{
        Stateplex::WriteBuffer<1024> inputBuffer(myActor);
        EXPECT_EQ(0, inputBuffer.length());
}

//WriteBuffer tests
TEST_F(BufferTest, appendAndEqualsTests)
{
        //append cString
        Stateplex::WriteBuffer<1024> inputBuffer(myActor);
        inputBuffer.append("Marja");
        EXPECT_EQ(inputBuffer.length(), 5);

        //append String
        Stateplex::WriteBuffer<1024> inputBuffer1(myActor);
        inputBuffer1.append(Stateplex::String::copy(myActor->allocator(), "Marja"));
        EXPECT_NE(inputBuffer1.length(), 0);
        EXPECT_TRUE(inputBuffer1.equals(Stateplex::String::copy(myActor->allocator(), "Marja")));
        EXPECT_EQ(inputBuffer1.length(), 5);

        //append buffers
        Stateplex::WriteBuffer<> *buffer1 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *buffer2 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *buffer3 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *buffer4 = new Stateplex::WriteBuffer<>(myActor);

        buffer1->append("Marja ");
        EXPECT_TRUE(buffer1->equals("Marja "));
        EXPECT_EQ(buffer1->length(), 6);

        buffer2->append("does coding and fails", 12);
        buffer1->append(buffer2);
        EXPECT_TRUE(buffer1->equals("Marja does coding and fails", 12));
        EXPECT_EQ(buffer1->length(), 18);

        buffer3->append(Stateplex::String::copy(myActor->allocator(), "and succeeds in it."));
        EXPECT_TRUE(buffer3->equals(Stateplex::String::copy(myActor->allocator(), "and succeeds in it.")));
        EXPECT_EQ(buffer3->length(), 19);

        buffer1->append(buffer3);
        buffer4->append("Marja does coding and succeeds in it.");
        EXPECT_TRUE(buffer1->equals(buffer4));
        EXPECT_EQ(buffer1->length(), 37);

        delete buffer1;
        delete buffer2;
        delete buffer3;
        delete buffer4;
}

TEST_F(BufferTest, insertTests)
{
        Stateplex::WriteBuffer<> *buffer1 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *buffer2 = new Stateplex::WriteBuffer<>(myActor);

        buffer1->insert(0xF867, "TestiCString");
        EXPECT_TRUE(buffer1->equals("TestiCString"));
        EXPECT_EQ(0, buffer1->compare("TestiCString"));
        EXPECT_EQ(0, buffer1->compare("TestiCString", 12));

        buffer2->insert(0xF900,"Ritaharju school is best", 16 );
        EXPECT_EQ(buffer2->length(), 16);
        EXPECT_TRUE(buffer2->equals("Ritaharju school is best", 16));

        //buffer1->insert(0xF867, buffer2);
                //tests/core/../../stateplex/core/writebuffer.h:275:86: error: no type named 'allocateMemory' in 'class Stateplex::Buffer<>::Block'
        // EXPECT_EQ(buffer1->asString(0xF867, 15), "Ritaharju school");
        // EXPECT_TRUE(buffer1->equals("Ritaharju school"));

        //buffer1->insert(0xF867, buffer2, 0xF900, 16);
                //tests/core/../../stateplex/core/writebuffer.h:275:86: error: no type named 'allocateMemory' in 'class Stateplex::Buffer<>::Block'
        // EXPECT_TRUE(buffer1->equals("Ritaharju school"));

        delete buffer1;
        delete buffer2;
}

//ReadBuffer tests
TEST_F(BufferTest, pushTests)
{
        ssize_t size = 0;
        int fd = open("myFile", std::fstream::in);

        Stateplex::WriteBuffer<> *wBuffer = new Stateplex::WriteBuffer<>(myActor);
        wBuffer->ensurePushLength(wBuffer->length());

        if (fd > 0)
                size = ::read(fd, wBuffer->pushPointer(), wBuffer->pushLength());
        else return;

        if (size>0)
                wBuffer->pushed(size);

        EXPECT_TRUE(wBuffer->length() == 30); //myFile contains 30 characters

        close(fd);
        delete wBuffer;
}

TEST_F(BufferTest, popTests)
{
        Stateplex::WriteBuffer<> *buffer1 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *buffer2 = new Stateplex::WriteBuffer<>(myActor);

        buffer1->append("First of May hassle");
        EXPECT_STREQ(buffer1->popPointer(), "First of May hassle");
        EXPECT_EQ(buffer1->popLength(), buffer1->length());

        char c = buffer1->peek();
        EXPECT_EQ(c, 'F');
        if (c == 'F')buffer1->pop();

        buffer1->popped(buffer1->popLength());
        EXPECT_EQ(buffer1->popLength(), 0);
        EXPECT_EQ(buffer1->length(), 0);

        buffer2->append("My first Vappu");
        EXPECT_STREQ(buffer2->popPointer(), "My first Vappu");
        buffer2->poppedAll();
        EXPECT_EQ(buffer2->popLength(), 0);

        delete buffer1;
        delete buffer2;

}
/*
void ensurePushLength(Size16 length);
        char *pushPointer() const;
        Size16 pushLength() const;
        void pushed(Size16 length);
*/
TEST_F(BufferTest, miscellaneousTests)
{
        Stateplex::WriteBuffer<> *buffer = new Stateplex::WriteBuffer<>(myActor);
        EXPECT_EQ(buffer->actor(), myActor);
        EXPECT_EQ(buffer->allocator(), myActor->allocator());

        buffer->append("abcd");
        Stateplex::String *myString = buffer->asString();
       // Stateplex::String *myString1 = buffer->asString(2); //Doesn't compile
       // Stateplex::String *myString2 = buffer->asString(0xF900, 3); //Doesn't compile

        delete buffer;
}

TEST_F(BufferTest, splitTest)
{
        Stateplex::WriteBuffer<> *buffer = new Stateplex::WriteBuffer<>(myActor);
        buffer->append("Ritaharju school");

        buffer->split('a', 4);// offsetOf to be done
        EXPECT_TRUE(buffer->equals("Rita"));

        delete buffer;
}

TEST_F(BufferTest, regionTest)
{
        Stateplex::WriteBuffer<> *inputBuffer = new Stateplex::WriteBuffer<>(myActor);

        inputBuffer->region(0xF900, 100);
        EXPECT_EQ(inputBuffer->length(), 0);
        inputBuffer->append("foo");
        EXPECT_EQ(inputBuffer->length(), 3);

        delete inputBuffer;
}
/*
//Buffer Tests
        String *asString() const;
        String *asString(Size length) const;
        String *asString(Size offset, Size length) const;
        char charAt(Size offset) const;
        int compare(const char *cString) const;
        int compare(const char *cString, Size length) const;
        int compare(const String *string) const;
        int compare(const Buffer *buffer) const;
        bool equals(const char *cString) const;
        bool equals(const char *cString, Size length) const;
        bool equals(const String *string) const;
        bool equals(const Buffer *buffer) const;
        Size length() const;
        Size offsetOf(char c, Size fromOffset = 0);

        WriteBuffer<mBlockSize> *region(Size offset, Size length);
        void region(Size offset, Size length, WriteBuffer<mBlockSize> *buffer);
        Array<WriteBuffer<mBlockSize> *> *split(char delimiter, Size maxElements);
        Size split(char delimiter, Array<WriteBuffer<mBlockSize> *> *elements);

        Size16 blockSize() const;



        //WriteBuffer tests
        TEST_F(BufferTest, WriteBufferTest)
        {

        }

        //ReadBuffer tests
        TEST_F(BufferTest, blockSizeTest)
        {
           std::size_t size;
        }
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

    //Iterator Tests
            TEST_F(BufferTest, IteratorTest)
        {

        }
           void advance(Size length = 1);
                Size offset();
                Size charBlockLength();
                const char *charBlock();
                Buffer<mBlockSize> *buffer() const;
                bool hasCurrent();
                char current();
    */


