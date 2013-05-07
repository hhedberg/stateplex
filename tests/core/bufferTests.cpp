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
        EXPECT_EQ(6, inputBuffer1->length());

        //append cString with a length
        inputBuffer1->append("is coding this complex program for ever", 10);
        EXPECT_TRUE(inputBuffer1->equals("Marja is coding this complex program for ever", 16));
        EXPECT_EQ(16, inputBuffer1->length());

        //append Stateplex::String
        inputBuffer1->append(Stateplex::String::copy(myActor->allocator(), "and Kari "));
        EXPECT_TRUE(inputBuffer1->equals("Marja is coding and Kari "));
        EXPECT_EQ(25, inputBuffer1->length());

        //append buffers
        inputBuffer2->append("is testing.");
        inputBuffer1->append(inputBuffer2);
        EXPECT_TRUE(inputBuffer1->equals("Marja is coding and Kari is testing."));
        EXPECT_EQ(36, inputBuffer1->length());

        delete inputBuffer1;
        delete inputBuffer2;
}
TEST_F(BufferTest, compareTests)
{
        Stateplex::WriteBuffer<> *buffer1 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *buffer2 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *buffer3 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *buffer4 = new Stateplex::WriteBuffer<>(myActor);

        buffer1->append("This is a cString");
        buffer2->append("This is a cString, too", 17);
        EXPECT_TRUE(!buffer1->compare("This is a cString"));//Compare returns 0 when true
        EXPECT_TRUE(!buffer2->compare("This is a cString"));//Compare returns 0 when true

        Stateplex::String *myString = Stateplex::String::copy(myActor->allocator(), "Summer holidays are approaching!");
        buffer3->append(myString);
        EXPECT_TRUE(!buffer3->compare(myString));//Compare returns 0 when true

        buffer4->append(buffer3);
        EXPECT_TRUE(!buffer4->compare(buffer3));//Compare returns 0 when true

        delete buffer1;
        delete buffer2;
        delete buffer3;
        delete buffer4;
}

TEST_F(BufferTest, splitTest)//Fail -- buffer contains the original character string, 16 characters as 8 characters expected
{
        Stateplex::WriteBuffer<> *buffer = new Stateplex::WriteBuffer<>(myActor);
        buffer->append("Ritaharju school");

        buffer->split('j', 8);

        EXPECT_TRUE(buffer->equals("Ritaharj"));
        EXPECT_TRUE(buffer->equals("Ritaharju school"));
        //EXPECT_EQ(8, buffer->length());

        delete buffer;
}

TEST_F(BufferTest, insertTests)//Fail -- 3, 4 cases give a compilation error -> see below
{
        Stateplex::WriteBuffer<100> *buffer1 = new Stateplex::WriteBuffer<100>(myActor);
        Stateplex::WriteBuffer<100> *buffer2 = new Stateplex::WriteBuffer<100>(myActor);
        Stateplex::WriteBuffer<100> *buffer3 = new Stateplex::WriteBuffer<100>(myActor);
        Stateplex::WriteBuffer<100> *buffer4 = new Stateplex::WriteBuffer<100>(myActor);

        //Insert -tests fail
        buffer1->append("TestiCString");
        buffer1->insert(2, "Marja");//
        //std::cout << "buffer1 contains: " << buffer1->popPointer() << "\n";
        //EXPECT_TRUE(buffer1->equals("TesMarjatiCString")); //buffer1 contains the original character string

        buffer2->append("This is text.");
        buffer2->insert(5, "Ritaharju school is best", 16 );
        //std::cout << "buffer2 contains: " << buffer2->popPointer() << "\n";
        EXPECT_TRUE(buffer2->equals("This Ritaharju school"));//buffer2 contains the original character string

        buffer3->append("This is another piece of text.");
        //buffer3->insert(2, buffer1);
                //tests/core/../../stateplex/core/writebuffer.h:275:86: error: no type named 'allocateMemory' in 'class Stateplex::Buffer<>::Block'
        // EXPECT_EQ("Ritaharju school", buffer1->asString(2, 15));
        // EXPECT_TRUE(buffer1->equals("Ritaharju school"));

        buffer4->append("This is the last piece of text.");
        // buffer4->insert(2, buffer3, 5, 10);
                //tests/core/../../stateplex/core/writebuffer.h:275:86: error: no type named 'allocateMemory' in 'class Stateplex::Buffer<>::Block'
        // EXPECT_TRUE(buffer1->equals("Ritaharju school"));

        delete buffer1;
        delete buffer2;
        delete buffer3;
        delete buffer4;
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

TEST_F(BufferTest, popTests) //Fail
{
        Stateplex::WriteBuffer<> *buffer1 = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *buffer2 = new Stateplex::WriteBuffer<>(myActor);

        buffer1->append("First of May hassle");
        EXPECT_TRUE(buffer1->equals("First of May hassle"));
        EXPECT_EQ( 19, buffer1->popLength());

        char c = buffer1->peek();
        EXPECT_EQ('F', c);
        if (c == 'F')buffer1->pop();

        buffer1->popped(buffer1->popLength());
        EXPECT_EQ(0, buffer1->popLength());
        EXPECT_EQ(0, buffer1->length());

        buffer2->append("My first Vappu");
        EXPECT_TRUE(buffer2->equals("My first Vappu"));
        EXPECT_EQ(14, buffer2->length());

        buffer2->poppedAll();
        EXPECT_EQ(0, buffer2->popLength());

        delete buffer1;
        delete buffer2;
}

TEST_F(BufferTest, miscellaneousTests)
{
        Stateplex::WriteBuffer<> *buffer = new Stateplex::WriteBuffer<>(myActor);
        EXPECT_EQ(myActor, buffer->actor());
        EXPECT_EQ(myActor->allocator(), buffer->allocator());
        EXPECT_EQ(1024, buffer->blockSize());

        buffer->append("abcd");
        EXPECT_EQ('c', buffer->charAt(2));
        Stateplex::String *myString = buffer->asString();
        EXPECT_STREQ("abcd", myString->chars());
        //myString = buffer->asString(2); buffer.h:513:27:   required from 'Stateplex::String* Stateplex::Buffer<mBlockSize>::asString(Stateplex::Size) const [with short unsigned int mBlockSize = 1024u; Stateplex::Size = long unsigned int]'
        //EXPECT_STREQ("ab", myString->chars());

        EXPECT_EQ(3, buffer->offsetOf('d', 0));
        //private: Buffer::blockByOffset
        //private: Buffer::blockOffset

        delete buffer;
}

TEST_F(BufferTest, regionTests)  //Fail --> gives the right character string, but length is different
{

        Stateplex::WriteBuffer<> *inputBuffer = new Stateplex::WriteBuffer<>(myActor);
        Stateplex::WriteBuffer<> *inputBuffer2 = new Stateplex::WriteBuffer<>(myActor);

        inputBuffer->append("Tomorrow I am not going to test at all.");
        inputBuffer2 = inputBuffer->region(8, 31);
        //std::cout << "inputBuffer length is " << inputBuffer->length() << "\n";
        //std::cout << "inputBuffer2 length is " << inputBuffer2->length() << "\n";
        EXPECT_STREQ( " I am not going to test at all.", inputBuffer2->popPointer());
        //EXPECT_EQ(23, inputBuffer2->length());

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


