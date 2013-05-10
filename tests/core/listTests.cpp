/*
 * bufferTests.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: maaalto
 */
#include <iostream>
#include "gtest/gtest.h"
#include "../../stateplex/core/list.h"


/**
 * Unit tests for list.h, readBuffer.h and WriteBuffer.h using Google c++ testing framework.
 *
 **/

class ListTest : public testing::Test {

protected:

        virtual void SetUp(){}

        virtual void TearDown(){}

        template<typename T>
        int countItems(Stateplex::List<T> *list)
        {
                int noItems = 0;

                for (Stateplex::ListIterator<T> iterator(list); iterator.hasCurrent(); iterator.subsequent()) {
                        noItems++;
                }

                return noItems;
        }

        Stateplex::ListItem *item = new Stateplex::ListItem(); //Compiler gives a warning: non-static data member initializers only available with -std=c++11 or -std=gnu++11 [enabled by default]
        Stateplex::ListItem *item1 = new Stateplex::ListItem();
        Stateplex::ListItem *item2 = new Stateplex::ListItem();
        Stateplex::List<Stateplex::ListItem> list;
        Stateplex::List<Stateplex::ListItem> list1;
};

/*
 * List tests start here ------------------------------------>
 */

/*
 * Declared list is first empty.
 */
TEST_F(ListTest, isEmptyTest)
{
        EXPECT_EQ(true, list.isEmpty());
        EXPECT_EQ(0, list.first());
        EXPECT_EQ(0, list.last());
}

/*
 * ListItem is added to the head of the list: there is one listItem in the list.
 */
TEST_F(ListTest, addHeadTest)
{
        list.addHead(item);
        EXPECT_EQ( true, !list.isEmpty());
        EXPECT_EQ( list.first(), item);
        EXPECT_EQ( list.last(), item);
}

/*
 * ListItem is added to the tail of the list: there is two listItems in the list.
 */
TEST_F(ListTest, addTailTest)
{
        list.addTail(item);
        EXPECT_EQ(list.last(), item);
}

/*
 * First item in the list.
 */
TEST_F(ListTest, getFirstItemTest)
{
        list.addHead(item);
        EXPECT_EQ(list.first(), item);
}

/*
 * //Last item in the list.
 */
TEST_F(ListTest, getLastItemTest)
{
        list.addTail(item);
        EXPECT_EQ(list.last(), item);
}

/*
 * Next item in the list.
 */
TEST_F(ListTest, nextTest)
{
        list.addHead(item);
        list.addTail(item1);
        EXPECT_EQ(list.next(list.first()), item1);
}

/*
 * Previous item in the list.
 */
TEST_F(ListTest, previousTest)
{
        list.addHead(item);
        list.addTail(item1);
        EXPECT_EQ(list.previous(list.last()), item);
}

/*
 * List added before another list.
 */
TEST_F(ListTest, spliceHeadTest)
{
        list.addHead(item);
        list1.addHead(item1);
        list.spliceHead(&list1);
        EXPECT_EQ(item1, list.first());
        EXPECT_EQ(list.last(), item);
}

/*
 * List added to the tail of a list.
 */
TEST_F(ListTest, spliceTailTest)
{
        list.addHead(item);
        list1.addHead(item1);
        list.spliceTail(&list1);
        EXPECT_EQ(list.last(),item1);
        EXPECT_EQ(list.first(), item);

}

/*
 * ListTests end here <----------------------------------------
 */

/*
 * ListItem tests start here ---------------------------------->
 */

/*
 * Add an item before another item in a list.
 */
TEST_F(ListTest, addBeforeTest)
{
        list.addHead(item);
        item1->addBefore(list.first());
        EXPECT_EQ(list.first(), item1);
}

/*
 * Add an item after another item in a list.
 */
TEST_F(ListTest, addAfterTest)
{
        int noItems=0;
        list.addTail(item);
        item2->addAfter(list.first());
        noItems = countItems(&list);

        EXPECT_EQ(list.next(list.first()), item2);
}

/*
 * Remove an item in from a list.
 */
TEST_F(ListTest, removeTest)
{
        list.addHead(item);
        item->remove();
        EXPECT_EQ(true, list.isEmpty());
}

/*
 * ListItem tests end here <----------------------------------
 */

/*
 * ListIterator tests start here ------------------------------>
 */

/*
 * Attaching a list iterator to a list.
 */
TEST_F(ListTest, listIteratorListTest)
{
        Stateplex::ListIterator<Stateplex::ListItem> li(&list);
        const Stateplex::List<Stateplex::ListItem> *const_list = &list;

        EXPECT_EQ(li.list(), const_list);

}

/*
 * Testing the functioning of an list iterator.
 */
TEST_F(ListTest, listIteratorTests)
{
        int noItems = 0;
        list.addHead(item);
        list.addTail(item1);
        Stateplex::ListIterator<Stateplex::ListItem> li(&list);

        EXPECT_EQ(li.hasCurrent(), true);
        EXPECT_EQ(li.current(), item);
        EXPECT_EQ(li.subsequent(), item1);
        EXPECT_EQ(li.hasSubsequent(), false);
        EXPECT_EQ(li.isBackwards(), false);
        list.addTail(item2);
        EXPECT_EQ(li.hasCurrent(),true);
        EXPECT_EQ(li.current(),item1);

        EXPECT_EQ(li.hasSubsequent(),false);
        noItems = countItems(&list);
        std::cout << "noItems is " << noItems << std::endl;
}

