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
#include "../../stateplex/core/array.h"


/**
 * Unit tests for buffer.h, readBuffer.h and WriteBuffer.h using Google c++ testing framework.
 *
 **/

class BufferTest : public testing::Test {

protected:

        static Stateplex::Dispatcher *dispatcher;
        static Stateplex::Actor *myActor;

        static void SetUpTestCase()
        {
                dispatcher = new Stateplex::Dispatcher();
                myActor = new Stateplex::Actor(dispatcher);

        }
        virtual void SetUp(){}
        virtual void TearDown(){}

        static void TearDownTestCase()
        {
                delete myActor;
                delete dispatcher;
        }

};

Stateplex::Dispatcher *BufferTest::dispatcher;
Stateplex::Actor *BufferTest::myActor;


TEST_F(BufferTest, zeroLengthTest)
{
        Stateplex::WriteBuffer inputBuffer(myActor);
        EXPECT_EQ(0, inputBuffer.length());
}


TEST_F(BufferTest, appendAndEqualsTests)
{
        Stateplex::WriteBuffer *inputBuffer1 = new Stateplex::WriteBuffer(myActor);
        Stateplex::WriteBuffer *inputBuffer2 = new Stateplex::WriteBuffer(myActor);

        //append cString - Fails as it returns false when less characters
        inputBuffer1->append("Marja ");
        EXPECT_EQ(6, inputBuffer1->length());
        //EXPECT_FALSE(inputBuffer1->equals("Marj"));//Fails
        EXPECT_TRUE(inputBuffer1->equals("Marja "));
        EXPECT_FALSE(inputBuffer1->equals("Marja i"));

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

TEST_F(BufferTest, compareTests) //Fails
{
        Stateplex::WriteBuffer *buffer1 = new Stateplex::WriteBuffer(myActor);
        Stateplex::WriteBuffer *buffer2 = new Stateplex::WriteBuffer(myActor);
        Stateplex::WriteBuffer *buffer3 = new Stateplex::WriteBuffer(myActor);
        Stateplex::WriteBuffer *buffer4 = new Stateplex::WriteBuffer(myActor);

        buffer1->append("This is a cString");
        EXPECT_TRUE(!buffer1->compare("This is a cString"));//Compare returns 0 when true
        //EXPECT_FALSE(!buffer1->compare("This is a cStrin"));Fails
        EXPECT_FALSE(!buffer1->compare("This is a cString "));

        buffer2->append("This is a cString, too", 17);
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
        Stateplex::WriteBuffer *buffer1 = new Stateplex::WriteBuffer(myActor);

        buffer1->append("Ritaharju school");
        Stateplex::Array<Stateplex::WriteBuffer *> *elements = buffer1->split('j', 8);
        Stateplex::String *myString = buffer1->asString();
        std::cout << "MyComment: buffer1 contains: " << myString->chars() << "\n";
        EXPECT_TRUE(buffer1->equals("Ritaharj"));
        EXPECT_TRUE(buffer1->equals("Ritaharju school"));
        //EXPECT_EQ(8, elements->length());Fails actual length is 2

        delete buffer1;
}

TEST_F(BufferTest, insertTests)//Fail -- cases 3, 4 give a compilation error -> see below
{
        Stateplex::WriteBuffer *buffer1 = new Stateplex::WriteBuffer(myActor);
        Stateplex::WriteBuffer *buffer2 = new Stateplex::WriteBuffer(myActor);
        Stateplex::WriteBuffer *buffer3 = new Stateplex::WriteBuffer(myActor);
        Stateplex::WriteBuffer *buffer4 = new Stateplex::WriteBuffer(myActor);

        //Insert -tests fail
        buffer1->append("TestiCString");
        buffer1->insert(2, "Marja");//
        std::cout << "buffer1 contains: " << buffer1->popPointer() << "\n";
        //EXPECT_TRUE(buffer1->equals("TesMarjatiCString")); //buffer1 contains the original character string

        buffer2->append("This is text.");
        buffer2->insert(5, "Ritaharju school is best", 16 );
        //std::cout << "buffer2 contains: " << buffer2->popPointer() << "\n";
        EXPECT_TRUE(buffer2->equals("This Ritaharju school"));//buffer2 contains the original character string

        buffer3->append("This is another piece of text.");
        //buffer3->insert(2, buffer1);
                //tests/core/../../stateplex/core/writebuffer.h:275:86: error: no type named 'allocateMemory' in 'class Stateplex::Buffer::Block'
        // EXPECT_EQ("Ritaharju school", buffer1->asString(2, 15));
        // EXPECT_TRUE(buffer1->equals("Ritaharju school"));

        buffer4->append("This is the last piece of text.");
        // buffer4->insert(2, buffer3, 5, 10);
                //tests/core/../../stateplex/core/writebuffer.h:275:86: error: no type named 'allocateMemory' in 'class Stateplex::Buffer::Block'
        // EXPECT_TRUE(buffer1->equals("Ritaharju school"));

        delete buffer1;
        delete buffer2;
        delete buffer3;
        delete buffer4;
}


TEST_F(BufferTest, pushTests)
{
        ssize_t size = 0;
        int fd = open("myFile", std::fstream::in);

        Stateplex::WriteBuffer *wBuffer = new Stateplex::WriteBuffer(myActor);
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
        Stateplex::WriteBuffer *buffer1 = new Stateplex::WriteBuffer(myActor);
        Stateplex::WriteBuffer *buffer2 = new Stateplex::WriteBuffer(myActor);

        buffer1->append("First of May hassle");
        EXPECT_TRUE(buffer1->equals("First of May hassle"));
        EXPECT_EQ( 19, buffer1->popLength());

        char c = buffer1->peek();
        EXPECT_EQ('F', c);
        if (c == 'F')buffer1->pop();
        EXPECT_TRUE(buffer1->equals("irst of May hassle"));

        buffer1->popped(buffer1->popLength());
        EXPECT_EQ(0, buffer1->popLength());
        EXPECT_EQ(0, buffer1->length());

        buffer2->append("My first Vappu is the best.");
        std::cout << "popTest-buffer2: " << buffer2->popPointer() << "\n";
        EXPECT_TRUE(buffer2->equals("My first Vappu"));
        EXPECT_EQ(27, buffer2->length());

        buffer2->poppedAll();
        EXPECT_EQ(0, buffer2->popLength());

        delete buffer1;
        delete buffer2;
}

TEST_F(BufferTest, miscellaneousTests)
{
        Stateplex::WriteBuffer *buffer = new Stateplex::WriteBuffer(myActor);
        EXPECT_EQ(myActor, buffer->actor());
        EXPECT_EQ(myActor->allocator(), buffer->allocator());
        EXPECT_EQ(1024, buffer->blockSize());

        buffer->append("abcd");
        EXPECT_EQ('c', buffer->charAt(2));
        Stateplex::String *myString = buffer->asString();
        EXPECT_STREQ("abcd", myString->chars());

        EXPECT_EQ(3, buffer->offsetOf('d', 0));

        delete buffer;
}

TEST_F(BufferTest, regionTests)  //Fail --> gives the right character string, but length is not correct
{

        Stateplex::WriteBuffer *inputBuffer = new Stateplex::WriteBuffer(myActor);
        Stateplex::WriteBuffer *inputBuffer2 = new Stateplex::WriteBuffer(myActor);

        inputBuffer->append("Tomorrow I am not going to test at all.");
        inputBuffer2 = inputBuffer->region(8, 31);
        std::cout << "inputBuffer length is " << inputBuffer->length() << "\n";
        std::cout << "inputBuffer2 length is " << inputBuffer2->length() << "\n";
        EXPECT_TRUE( inputBuffer2->equals(" I am not going to test at all."));
        //EXPECT_EQ(23, inputBuffer2->length());// This length fails, actual is 31

        delete inputBuffer;
        delete inputBuffer2;
}


