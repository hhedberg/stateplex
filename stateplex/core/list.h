/*
 * Stateplex - A server-side actor model library.
 *
 * core/list.h
 *
 * (c) 2013 Henrik Hedberg <henrik.hedberg@innologies.fi>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Authors: Henrik Hedberg
 */

#ifndef INCLUDED_STATEPLEX_LIST_H
#define INCLUDED_STATEPLEX_LIST_H

#include <cstddef>

namespace Stateplex {

template<typename T>
class List;

template<typename T>
class ListIterator;

class ListItem {
	template<typename T> friend class List;

	ListItem *mNext;
	ListItem *mPrevious;

	void addBetween(ListItem *previous, ListItem *next);

public:
	ListItem();
	virtual ~ListItem();

	void addBefore(ListItem *existing_item);
	void addAfter(ListItem *existing_item);
	void remove();
};

template<typename T>
class List {
	template<typename S> friend class ListIterator;

	ListItem mItems;

	void addBetween(ListItem *previous, ListItem *next);
	ListItem *next(ListItem *item);
	ListItem *previous(ListItem *item);

public:
	List();
	
	T *first();
	T *last();
	
	void addHead(T *item);
	void addTail(T *item);
	void spliceHead(List<T> *list);
	void spliceTail(List<T> *list);

	bool isEmpty();
};

template<typename T>
class ListIterator {
	List<T> *mList;
	ListItem *mCurrent;
	ListItem *mSubsequent;
	bool mBackwards;
	
public:
	ListIterator(List<T> *list, bool backwards = false);

	List<T> *list();
	T *current();
	T *subsequent();
	bool hasCurrent();
	bool hasSubsequent();
	bool backwards();
};

}

/*** Inline implementations ***/

namespace Stateplex {

inline ListItem::ListItem()
	: mNext(this), mPrevious(this)
{ }

inline ListItem::~ListItem()
{ }

inline void ListItem::addBetween(ListItem *previous, ListItem *next)
{
	previous->mNext = this;
	mPrevious = previous;
	mNext = next;
	next->mPrevious = this;
}

inline void ListItem::addBefore(ListItem *existing_item)
{
	ListItem *previous = existing_item->mPrevious;

	mPrevious->mNext = mNext;
	mNext->mPrevious = mPrevious;
	addBetween(previous, existing_item);
}

inline void ListItem::addAfter(ListItem *existing_item)
{
	ListItem *next = existing_item->mNext;

	mNext->mPrevious = mPrevious;
	mPrevious->mNext = mNext;
	addBetween(existing_item, next);
}

inline void ListItem::remove()
{
	mPrevious->mNext = mNext;
	mNext->mPrevious = mPrevious;
	mNext = this;
	mPrevious = this;
}

template<typename T>
List<T>::List()
{ }

template<typename T>
T *List<T>::first()
{
	return static_cast<T *>(mItems.mNext != &mItems ? mItems.mNext : 0);
}

template<typename T>
T *List<T>::last()
{
	return static_cast<T *>(mItems.mPrevious != &mItems ? mItems.mPrevious : 0);
}

template<typename T>
ListItem *List<T>::next(ListItem *item)
{
	return (item->mNext != &mItems ? item->mNext : 0);
}

template<typename T>
ListItem *List<T>::previous(ListItem *item)
{
	return (item->mPrevious != &mItems ? item->mPrevious : 0);
}

template<typename T>
void List<T>::addBetween(ListItem *previous, ListItem *next)
{
	previous->mNext = mItems.mNext;
	mItems.mNext->mPrevious = previous;
	mItems.mPrevious->mNext = next;
	next->mPrevious = mItems.mPrevious;

	mItems.mNext = mItems.mPrevious = &mItems;
}

template<typename T>
void List<T>::addHead(T *item)
{
	item->addAfter(&mItems);
}

template<typename T>
void List<T>::addTail(T *item)
{
	item->addBefore(&mItems);
}

template<typename T>
void List<T>::spliceHead(List<T> *list)
{
	list->addBetween(&mItems, mItems.mNext);
}

template<typename T>
void List<T>::spliceTail(List<T> *list)
{
	list->addBetween(mItems.mPrevious, &mItems);
}

template<typename T>
bool List<T>::isEmpty()
{
	return mItems.mNext == &mItems;
}

template<typename T>
ListIterator<T>::ListIterator(List<T> *list, bool backwards)
	: mList(list), mBackwards(backwards)
{
	if (!backwards) {
		mCurrent = mList->first();
		if (mCurrent)
			mSubsequent = mList->next(mCurrent);
	} else {
		mCurrent = list->last();
		if (mCurrent)
			mSubsequent = mList->previous(mCurrent);
	}
}

template<typename T>
List<T> *ListIterator<T>::list()
{
	return mList;
}

template<typename T>
T *ListIterator<T>::current()
{
	return static_cast<T *>(mCurrent);
}

template<typename T>
T *ListIterator<T>::subsequent()
{
	mCurrent = mSubsequent;
	if (mCurrent)
		mSubsequent = (!mBackwards ? mList->next(mCurrent) : mList->previous(mCurrent));

	return static_cast<T *>(mCurrent);
}

template<typename T>
bool ListIterator<T>::hasCurrent()
{
	return mCurrent != 0;
}

template<typename T>
bool ListIterator<T>::hasSubsequent()
{
	return mSubsequent != 0;
}

template<typename T>
bool ListIterator<T>::backwards()
{
	return mBackwards;
}

}

#endif
