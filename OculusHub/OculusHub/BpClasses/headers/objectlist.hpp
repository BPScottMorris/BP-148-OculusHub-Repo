/*==============================================================================
Function that encapsulates a standard list to create an object list.
Unlike a standard CList<>, an Object list is a list of pointers representing
objects of the specified type, and the list 'owns' the objects, destroying them
when they are removed from the list.
The classes Add functions and operators allow either an object to be externally
created with the 'new' keyword, or a reference to be used instead, and the object
will be cloned from it.
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef OBJECTLIST_HPP
#define OBJECTLIST_HPP

#include <vector>

//Include common type definitions and macros
#include "common.h"
#include "list.hpp"

//==============================================================================
//Class Definition...
//==============================================================================
/*!
Define a class that holds references to a list of objects, and destroys the
objects as required when removed from the list
*/
template <class T>
class CObjectList {
	private:	
				
	protected:				
		typedef CList<T*> CListInternal;
		typedef CListInternal* PListInternal;	
		
		PListInternal _list;				
		
	public:
		//Construction and Disposal
		CObjectList(uint32 capacity = 8, uint32 delta = 4);
		virtual ~CObjectList();
		
		//Operators
		T& operator[](uint32 index);
		
		//Methods
		uint32 Add(T* pValue);
		uint32 AddCopy(const T& value);
		void Clear();
		int32 Find(const T& value);
		int32 FindPointer(T* pValue);
		uint32 GetCapacity();
		uint32 GetCount();	
		T& GetItem(uint32 index);		
		T* GetItemPointer(uint32 index);
		int32 Insert(uint32 index, T* pValue);
		void Remove(uint32 index);		
		void SetItem(uint32 index, T& value);		
};

/*! Create a class that holds strings - and manages their resources */
typedef CObjectList<string> CStringList; 

/*! Define a pointer to a class that holds strings */
typedef CStringList* PStringList;

//==============================================================================
//Class Implementation...
//==============================================================================
//CObjectList
//==============================================================================
/*!-----------------------------------------------------------------------------
*/
template <class T>
CObjectList<T>::CObjectList(uint32 capacity, uint32 delta)
{	
	_list = new CListInternal(capacity, delta);	
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
CObjectList<T>::~CObjectList()
{
	//Destroy all objects in the list
	this->Clear();
		
	//Destroy the list
	delete _list;	
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
T& CObjectList<T>::operator[](uint32 index)
{
	return this->GetItem(index);
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
uint32 CObjectList<T>::Add(T* pValue)
{
	//Add the pointer to the object to the list
	return _list->Add(pValue);
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
uint32 CObjectList<T>::AddCopy(const T& value)
{
	//Create a new clone of the object reference being passed in
	//P* p = new P(value);	//This requires the object to have a 'copy' constructor
	T* p = new T();
	*p = value;
	
	//Add the object
	return this->Add(p);
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
void CObjectList<T>::Clear()
{	
	//Free up all objects in the list prior to clearing	
	uint32 len = _list->GetCount();
	
	//Destroy the existing object at the index
	for(int32 index = 0; index < len; index++) {
		T* p = _list->GetItem(index);
		if(p)
			delete p;	
	}
	
	//Now clear the list contents
	_list->Clear();
}

/*!-----------------------------------------------------------------------------
Function that attempts to find the specified object in the list using the
object classes "=" operator to do the comparison.
This function is used for lists containing string objects, or simmilar.
*/
template <class T>
int32 CObjectList<T>::Find(const T& value)
{
	uint32 len = _list->GetCount();
	for(uint32 index = 0; index < len; index++) {
		if(value == this->GetItem(index))
			return index;
	}
	return -1;
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
int32 CObjectList<T>::FindPointer(T* pValue)
{
	uint32 len = _list->GetCount();
	for(uint32 index = 0; index < len; index++) {
		if(pValue == this->GetItemPointer(index))
			return index;
	}
	return -1;
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
uint32 CObjectList<T>::GetCapacity()
{
	return _list->GetCapacity();
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
uint32 CObjectList<T>::GetCount()
{
	return _list->GetCount();
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
T& CObjectList<T>::GetItem(uint32 index)
{
	T* p;
	if(index < _list->GetCount()) {
		p = _list->GetItem(index);
		return (*p); 		
	}
	else {
		//throw CException("Invalid object list index");
		return T();
	}
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
T* CObjectList<T>::GetItemPointer(uint32 index)
{	
	if(index < _list->GetCount())
		return _list->GetItem(index);			
	else {		
		return NULL;
		//throw CException("Invalid object list index");
	}
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
void CObjectList<T>::Remove(uint32 index)
{
	if(index < _list->GetCount()) {	
		//Destroy the existing object at the index
		T* p = _list->GetItem(index);
		if(p)
			delete p;	
		
		//Remove the entry from the list
		_list->Remove(index);
	}	
}


/*!-----------------------------------------------------------------------------
*/
template <class T>
int32 CObjectList<T>::Insert(uint32 index, T* pValue)
{
	//Insert the pointer into the list
	return _list->Insert(index, pValue);
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
void CObjectList<T>::SetItem(uint32 index, T& value)
{
	T* p;	
	if(index < _list->GetCount()) {
		//Destroy the existing string at the index
		p = _list->GetItem(index);
		if(p)
			delete p;
		
		//Create a pointer to the new string to store, so it doesn't go out of context
		p = new T();
		*p = value;
	
		//Update the string		
		_list->GetItem(index) = p;		
	}
}

//==============================================================================
#endif
