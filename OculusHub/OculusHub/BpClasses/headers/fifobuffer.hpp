//==============================================================================
//
//==============================================================================
//Prevent multiple inclusions of this file
#ifndef FIFOBUFFER_HPP
#define FIFOBUFFER_HPP

//Include common type definitions and macros
#include "common.h"

//==============================================================================
//Class Definition...
//==============================================================================
/*!
Class that implements a FIFO style buffer with a fixed presettable capacity
Elements are pushed onto the buffer, and popped off
*/
template <class T>
class CFifoBuffer {
	protected:	
		typedef T* pT;	
		
	private:
		uint32 _capacity;		
		volatile uint32 _count;
		pT _data;
		volatile uint32 _idxRead;
		volatile uint32 _idxWrite;
		
	protected:
		
	public:
		CFifoBuffer(uint32 capacity);
		~CFifoBuffer();	
		void Clear();
		inline uint32 GetCapacity();
		inline uint32 GetCount();
		inline uint32 GetFree();
		inline bool IsEmpty();
		inline bool IsFull();
		bool Peek(T* pvalue, uint32 index);
		bool Push(T value);	
		bool Pop(T* pvalue);	
		void SetCapacity(uint32 capacity);
};

//Declare other types of FIFO Buffer
typedef CFifoBuffer<uint8> CByteFifoBuffer;
typedef CByteFifoBuffer* PByteFifoBuffer;

typedef CFifoBuffer<int8> CInt8FifoBuffer;
typedef CInt8FifoBuffer* PInt8FifoBuffer;

typedef CFifoBuffer<int16> CInt16FifoBuffer;
typedef CInt16FifoBuffer* PInt16FifoBuffer;

typedef CFifoBuffer<int32> CInt32FifoBuffer;
typedef CInt32FifoBuffer* PInt32FifoBuffer;

typedef CFifoBuffer<uint8> CUInt8FifoBuffer;
typedef CUInt8FifoBuffer* PUInt8FifoBuffer;

typedef CFifoBuffer<uint16> CUInt16FifoBuffer;
typedef CUInt16FifoBuffer* PUInt16FifoBuffer;

typedef CFifoBuffer<uint32> CUInt32FifoBuffer;
typedef CUInt32FifoBuffer* PUInt32FifoBuffer;

typedef CFifoBuffer<float> CFloatFifoBuffer;
typedef CFloatFifoBuffer* PFloatFifoBuffer;

typedef CFifoBuffer<double> CDoubleFifoBuffer;
typedef CDoubleFifoBuffer* PDoubleFifoBuffer;

//==============================================================================
//Class Implementation...
//==============================================================================
//CFifoBuffer
//==============================================================================
/*!-----------------------------------------------------------------------------
Create the ring bugger and allocate memory resources
*/
template <class T>
CFifoBuffer<T>::CFifoBuffer(uint32 capacity)
{
	//Allocate the capacity and initialise
	_data = NULL;
	this->SetCapacity(capacity);
}

/*!-----------------------------------------------------------------------------
Destroy the ring buffer and release resources used
*/
template <class T>
CFifoBuffer<T>::~CFifoBuffer()
{
	//Release the data used
	free(_data);	
}

/*!-----------------------------------------------------------------------------
Function that clears the contents of the buffer
*/
template <class T>
void CFifoBuffer<T>::Clear()
{
	_count = 0;
	_idxRead = 0;
	_idxWrite = 0;
}

/*!-----------------------------------------------------------------------------
Function that returns the capacity of the buffer
@result The number of elements the buffer can hold
*/
template <class T>
uint32 CFifoBuffer<T>::GetCapacity()
{
	return _capacity;
}

/*!-----------------------------------------------------------------------------
Function that returns how many elements the buffer currently contains
*/
template <class T>
uint32 CFifoBuffer<T>::GetCount()
{
	return _count;
}

/*!-----------------------------------------------------------------------------
Function that returns the number of free elements on the buffer
*/
template <class T>
uint32 CFifoBuffer<T>::GetFree()
{
	return _capacity - _count;
}

/*!-----------------------------------------------------------------------------
Function that returns if the buffer is empty
*/
template <class T>
bool CFifoBuffer<T>::IsEmpty()
{
	return (_count == 0);
}

/*!-----------------------------------------------------------------------------
Function that returns if the buffer is full
*/
template <class T>
bool CFifoBuffer<T>::IsFull()
{
	return (_count >= _capacity);
}

/*!-----------------------------------------------------------------------------
Function that returns a value at the specified index (relative to the start/head
of the buffer), without removing the value from the buffer
@param pvalue Pointer to where the element should be stored
@param index The offset relative to the front of the buffer, where the value should be retreived
@result False if the buffer was empty and an element couldn't be removed
*/
template <class T>
bool CFifoBuffer<T>::Peek(T* pvalue, uint32 index)
{
	if(index < _count) {
		uint32 idxPeek = _idxRead + index;
		if(idxPeek >= _capacity)
			idxPeek -= _capacity;
		*pvalue = _data[idxPeek];
		return true;
	}
	else {
		//The index is beyond the end of the buffer, so no data available
		return false;
	}	
}

/*!-----------------------------------------------------------------------------
Function that pushes an element onto the end of the buffer
@param value The element to push onto the buffer
@result False is the buffer is full and the element couldn't be added
*/
template <class T>
bool CFifoBuffer<T>::Push(T value)
{
	if(_count < _capacity) {
		//Put the element onto the buffer
		_data[_idxWrite] = value;
		_count++;
		_idxWrite++;
		if(_idxWrite >= _capacity)
			_idxWrite -= _capacity;
		return true;
	}
	else {
		//The buffer is full
		return false;
	}
}

/*!-----------------------------------------------------------------------------
Function that pops an element of the start of the buffer
@param pvalue Pointer to where the element should be stored
@result False if the buffer was empty and an element couldn't be removed
*/
template <class T>
bool CFifoBuffer<T>::Pop(T* pvalue) 
{
	if(_count > 0) {
		*pvalue = _data[_idxRead];
		_count--;
		_idxRead++;
		if(_idxRead >= _capacity)
			_idxRead -= _capacity;
		return true;
	}
	else {
		//The buffer is empty, so nothing can be removed
		return false;
	}
}

/*!-----------------------------------------------------------------------------
Function that sets the maximum storage capacity of the buffer
@param capacity The new capacity of the buffer
@result True if the buffers capacity was sucessfully changed, false if not and existing contents are preserved
*/
template <class T>
void CFifoBuffer<T>::SetCapacity(uint32 capacity)
{
	//Store the new capacity
	_capacity = capacity;
	
	//Allocate (if _data is null) or reallocate (and copy the contents) of the data
	pT dataNew = (pT)realloc(_data, _capacity * sizeof(T));	

	/*
	if(!dataNew) {
		//Memory could not be allocated, so return failure
		throw CException("The ring buffer capacity cannot be set");
	}
	*/
	
	//Memory successfully allocated, so store new pointer and return success
	_data = dataNew;

	//Clear the data
	this->Clear();
}
		
//==============================================================================
#endif
