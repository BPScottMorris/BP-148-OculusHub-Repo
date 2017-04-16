/*==============================================================================
Unlike the standard C++ 'vector' object, the list has an improved capacity
management system, that grows in a specified 'delta' increment when full, and
releases memory when items are removed
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef LIST_HPP
#define LIST_HPP

#include <vector>

//Include common type definitions and macros
#include "common.h"

//==============================================================================
//Class Definition...
//==============================================================================
/*!
Define a class that impliments a dynamically sizing list
The template paramter T can specify any standard fundamental type (i.e uint, int
float, double etc), pointer or struct.
*/
template <class T>
class CList {
	protected:
		typedef vector<T> CListVector;
		typedef CListVector* PListVector;

	private:
		PListVector _list;
		uint32 _capacityDefault;
		uint32 _capacityDelta;
		uint32 _capacityReduce;

	protected:
		virtual int32 DoAdd(const T& value);
		virtual int32 DoInsert(uint32 index, const T& value);
		virtual void DoRemove(uint32 index);

	public:
		//Constructor & destructor
		CList(uint32 capacity = 16, uint32 delta = 8);
		virtual ~CList();

		//Operators
		T& operator[](uint32 index);

		//Methods
		int32 Add(const T& value);
		virtual void Clear();
		int32 Find(const T& value);
		uint32 GetCapacity();
		uint32 GetCount();
		T& GetItem(uint32 index);
		T* GetItemPointer(uint32 index);
		int32 Insert(uint32 index, const T& value);
		void Remove(uint32 index);
		void SetCapacity(uint32 capacity);
		void SetItem(uint32 index, const T& value);
};

/*! Define a list that holds bytes */
typedef CList<uint8> CByteList;

/*! Define a pointer to a list that holds bytes */
typedef CByteList* PByteList;

/*! Define a list that holds integers */
typedef CList<int32> CIntList;

/*! Define a pointer to a list that holds integers */
typedef CIntList* PIntList;

/*! Define a list that holds floats */
typedef CList<float> CFloatList;

/*! Define a pointer to a list that holds floats */
typedef CFloatList* PFloatList;

/*! Define a list that holds doubles */
typedef CList<double> CDoubleList;

/*! Define a pointer to a list that holds doubles */
typedef CDoubleList* PDoubleList;

//==============================================================================
//Class Implementation...
//==============================================================================
//CList
//==============================================================================
/*!-----------------------------------------------------------------------------
*/
template <class T>
CList<T>::CList(uint32 capacity, uint32 delta)
{
	//Store the capacity 'delta' value - for size changing
	_capacityDefault = capacity;
	_capacityDelta = (delta < 1) ? 1 : delta;

	//Initialise the list
	_list = new CListVector();

	//Set the initial capacity
	this->SetCapacity(_capacityDefault);
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
CList<T>::~CList()
{
	//Empty the list
	this->Clear();

	//Destroy the list
	delete _list;
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
T& CList<T>::operator[] (uint32 index)
{
	return this->GetItem(index);
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
int32 CList<T>::Add(const T& value)
{
	//If the list capacity is full, do a capacity increase by the pre-specified
	//delta value, rather than just growing the list by 1
	uint32 capacity = _list->capacity();
	if((_capacityDelta > 1) && (_list->size() == capacity)) {
		this->SetCapacity(capacity + _capacityDelta);
	}

	//Now do the add
	return this->DoAdd(value);
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
void CList<T>::Clear()
{
	//Now clear the list contents
	_list->clear();

	//Reset the capacity
	this->SetCapacity(_capacityDefault);
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
int32 CList<T>::DoAdd(const T& value)
{
	//Add the item at the end of the list
	_list->push_back(value);

	//Return the index the item was added to
	return _list->size();
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
int32 CList<T>::DoInsert(uint32 index, const T& value)
{
	//Insert the item into the list
	_list->insert(_list->begin() + index, value);

	//Return the new size of the list
	return _list->size();
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
void CList<T>::DoRemove(uint32 index)
{
	//Remove the entry from the list
	_list->erase(_list->begin() + index);
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
int32 CList<T>::Find(const T& value)
{
	//Iterate through the list entries
	uint32 len = _list->size();
	for(uint32 index = 0; index < len; index++) {
		//Compare the memory contents of the element to find with the list element,
		//and if the return value is zero (for equal), then return the index
		T listValue = _list->at(index);
		if(!memcmp(&value, &listValue, sizeof(T)))
			return index;
	}

	//If we reach here, return "not found"
	return -1;
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
uint32 CList<T>::GetCapacity()
{
	return _list->capacity();
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
uint32 CList<T>::GetCount()
{
	return _list->size();
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
T& CList<T>::GetItem(uint32 index)
{
	/*
	if(index < _list->size())
		return _list->at(index);
	else
		throw CException("Invalid list index");
	*/
	return _list->at(index);
}

/*!-----------------------------------------------------------------------------
Function that returns a pointer to the item in the list at the specified index.
Dereferencing the pointer allows code to then directly manipulate the list entity
without the need to copy it out of the class
*/
template <class T>
T* CList<T>::GetItemPointer(uint32 index)
{
	return &(this->GetItem(index));
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
int32 CList<T>::Insert(uint32 index, const T& value)
{
	//If the list capacity is full, do a capacity increase by the pre-specified
	//delta value, rather than just growing the list by 1
	uint32 capacity = _list->capacity();
	if((_capacityDelta > 1) && (_list->size() == capacity)) {
		this->SetCapacity(capacity + _capacityDelta);
	}

	//Now do the insertion
	return this->DoInsert(index, value);
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
void CList<T>::Remove(uint32 index)
{
	if(index < _list->size()) {
		//Remove the item
		this->DoRemove(index);

		//If the list capacity has shrunk below a reduction threshold, then resize the list to free memory
		if(_list->size() <= _capacityReduce) {
			this->SetCapacity(_capacityReduce);
		}
	}
}

/*!-----------------------------------------------------------------------------
Function the sets the new capacity of the list (i.e. allocated storage memory,
not the number of elements current stored!)
If the capacity is reduced smaller than the number of current elements, then
the exceeding elements are removed! (Use with care with element objects that
might require disposal)
*/
template <class T>
void CList<T>::SetCapacity(uint32 capacity)
{
	uint32 current = _list->capacity();
	if(capacity > current){
		//Enlarge the list storage capacity
		_list->reserve(capacity);
	}
	else if(capacity < current) {
		//Remove extra elements - call the DoRemove method, so
		//inheriting classes can handle the remove.
		while(_list->size() > capacity) {
			this->DoRemove(_list->size() - 1);
		}

		//Use the "vector swap" trick to free unused memory and set the
		//capacity to the current list size
		vector<T>(*_list).swap(*_list);

		//If we're now to small (i.e. capacity is being reduced, but still
		//larger than number of elements) then re-enlarge the list storage capacity
		_list->reserve(capacity);
	}

	//Compute a new threshold level to use when reducing the size of the list
	_capacityReduce = _list->capacity() - _capacityDelta;
	if(_capacityReduce < _capacityDefault)
		_capacityReduce = _capacityDefault;
}

/*!-----------------------------------------------------------------------------
*/
template <class T>
void CList<T>::SetItem(uint32 index, const T& value)
{
	if(index < _list->size()) {
		_list->at(index) = value;
	}
}

//==============================================================================
#endif
