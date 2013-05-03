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
        Stateplex::WriteBuffer<> *inputBuffer1 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<1024> *inputBuffer2 = new Stateplex::WriteBuffer<1024>(myActor);

        //append cString
        inputBuffer1->append("Marja ");
        EXPECT_EQ(inputBuffer1->length(), 6);

        //append cString with a length
        inputBuffer1->append("is coding this complex program for ever", 10);
        EXPECT_TRUE(inputBuffer1->equals("Marja is coding this complex program for ever", 16));
        EXPECT_EQ(inputBuffer1->length(), 16);

        //append Stateplex::String
        inputBuffer1->append(Stateplex::String::copy(myActor->allocator(), "and Kari "));
       // EXPECT_STREQ(inputBuffer2->popPointer(), "Kari");
        EXPECT_TRUE(inputBuffer1->equals("Marja is coding and Kari "));
        EXPECT_EQ(inputBuffer1->length(), 25);

        //append buffers
        inputBuffer2->append("is testing.");
        inputBuffer1->append(inputBuffer2);
        EXPECT_TRUE(inputBuffer1->equals("Marja is coding and Kari is testing."));
        EXPECT_EQ(inputBuffer1->length(), 36);

        delete inputBuffer1;
        delete inputBuffer2;
}
/*
TEST_F(BufferTest, insertAndCompareTests)
{
        Stateplex::WriteBuffer<> *buffer1 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *buffer2 = new Stateplex::WriteBuffer<>(myActor);

        //Insert -tests fail
        buffer1->append("TestiCString");
        buffer1->insert(2, "Marja");//
        std::cout << "buffer1 contains " << buffer1->popPointer() << "\n";
        EXPECT_TRUE(buffer1->equals("TestiCString"));
        EXPECT_EQ(0, buffer1->compare("TestiCString"));
        //EXPECT_EQ(0, buffer1->compare("TestiCString", 12));

        buffer2->append("This is text.");
        buffer2->insert(2, "Ritaharju school is best", 16 );//Should all of the string be inserted?
        std::cout << "buffer2 contains " << buffer2->popPointer() << "\n";

        //buffer3->insert(2, buffer1);
                //tests/core/../../stateplex/core/writebuffer.h:275:86: error: no type named 'allocateMemory' in 'class Stateplex::Buffer<>::Block'
        // EXPECT_EQ(buffer1->asString(0xF867, 15), "Ritaharju school");
        // EXPECT_TRUE(buffer1->equals("Ritaharju school"));
        //EXPECT_EQ(buffer3->equals());

        //buffer4->insert(0xF867, buffer2, 0xF900, 16);
                //tests/core/../../stateplex/core/writebuffer.h:275:86: error: no type named 'allocateMemory' in 'class Stateplex::Buffer<>::Block'
        // EXPECT_TRUE(buffer1->equals("Ritaharju school"));

        delete buffer1;
        delete buffer2;
}
*/

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
        EXPECT_EQ(buffer1->popLength(), 19);

        char c = buffer1->peek();
        EXPECT_EQ(c, 'F');
        if (c == 'F')buffer1->pop();

        buffer1->popped(buffer1->popLength());
        EXPECT_EQ(buffer1->popLength(), 0);
        EXPECT_EQ(buffer1->length(), 0);

        buffer2->append("My first Vappu");
        EXPECT_TRUE(buffer2->equals("My first Vappu"));

        //EXPECT_STREQ(buffer2->popPointer(), "My first Vappu"); Returns "My first Vappuassle"

        buffer2->poppedAll();
        EXPECT_EQ(buffer2->popLength(), 0);

        delete buffer1;
        delete buffer2;
}

TEST_F(BufferTest, miscellaneousTests)
{
        Stateplex::WriteBuffer<> *buffer = new Stateplex::WriteBuffer<>(myActor);
        EXPECT_EQ(buffer->actor(), myActor);
        EXPECT_EQ(buffer->allocator(), myActor->allocator());
        EXPECT_EQ(buffer->blockSize(), 1024);

        buffer->append("abcd");
        EXPECT_EQ(buffer->charAt(2), 'c');
        Stateplex::String *myString = buffer->asString();
        EXPECT_STREQ(myString->chars(), "abcd");
        //myString = buffer->asString(2); buffer.h:513:27:   required from 'Stateplex::String* Stateplex::Buffer<mBlockSize>::asString(Stateplex::Size) const [with short unsigned int mBlockSize = 1024u; Stateplex::Size = long unsigned int]'
        //EXPECT_STREQ(myString->chars(), "ab");

        EXPECT_EQ(buffer->offsetOf('d', 0), 3);
        //private: Buffer::blockByOffset
        //private: Buffer::blockOffset

        delete buffer;
}

//Check split - it doesn't work
TEST_F(BufferTest, splitTest)
{
        Stateplex::WriteBuffer<> *buffer = new Stateplex::WriteBuffer<>(myActor);
        buffer->append("Ritaharju school");

        buffer->split('a', 4);
        EXPECT_TRUE(buffer->equals("Rita"));

        delete buffer;
}


TEST_F(BufferTest, regionTests)
{
        //Don't know if region should work like this!
        Stateplex::WriteBuffer<> *inputBuffer = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *inputBuffer2 = new Stateplex::WriteBuffer<>(myActor);

        inputBuffer->append("Tomorrow I am not going to test at all.");
        inputBuffer2 = inputBuffer->region(8, 31);
        //std::cout << "length is " << inputBuffer2->length() << "\n";
        EXPECT_STREQ(inputBuffer2->popPointer(), " I am not going to test at all.");
        EXPECT_EQ(inputBuffer2->length(), 39);

        delete inputBuffer;
        delete inputBuffer2;
}

//Buffer Tests
/*
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


