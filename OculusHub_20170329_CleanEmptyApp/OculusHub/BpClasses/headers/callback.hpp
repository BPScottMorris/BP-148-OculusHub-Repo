/*==============================================================================
C++ Module that provides the definitions and implementation for an event
handler and notification model.

This file contains the following class types
DELEGATES are classes that hold information to indirectly execute the code for a
global function or an objects member function.
The code makes use of C++ templates to effectively create an implementation for
each type of class or function the compiler encounters.
Delegates are then used by EVENTS and CALLBACKS to provide the method to execute
the code

CALLBACKS are a class that provides a one-to-one caller model for executing a
delegate (unlike Events that provide one-to-many). As no list of Delegates is held,
the memory footprint is smaller than an Event and also faster to execute.

--------------------------------------------------------------------------------
Background...
Based on code from a web articles at ...
http://www.codeproject.com/Articles/6614/Generic-Observer-Pattern-and-Events-in-C?msg=877439
http://www.codeproject.com/Articles/8262/Implementation-of-Delegates-in-C-using-Signal-and
describing "Generic Observer Pattern and events in C++" by Eduard Baranovsky

In Object Oriented Programming an “Events and Delegates” concept is one of the
most frequently used concepts in programming, sometimes referred to as “Observer”
or “Document/View” design pattern. Classical formulation of it could be found 
in “Design Patterns, Elements of Reusable Object Oriented Software” by 
Erich Gamma, Richard Helm, Ralph Johnson, and John Vlissides (The Gang of Four).

This concept is used when you want some information stored in one object, 
called “model” (subject) to be watched by others, called “views” (observers). 
Each time when information is changed in the “model”, “views” attached to the 
model should receive notification and update there states accordingly to the changed “model”.

16/09/2013 - Created V1.0 of file, RSh (based on the above)
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef CALLBACK_HPP
#define CALLBACK_HPP

//Include common type definitions and macros
#include "common.h"

//==============================================================================
//Declaration...
//==============================================================================

//==============================================================================
//Zero Parameter Classes
//==============================================================================
/*!-----------------------------------------------------------------------------
Declare an abstract base class from which all EventHandlers derive, allowing us
to attach different types of event handles (member functions with same types of 
parameters from different classes) to the same event. 
We use templates to make it generic for any combination of parameter types in 
“delegate” or call back method. There are different event types for every number
of arguments in callback function.
*/
template <typename ReturnT>
class CDelegate0
{
	public:
		virtual ~CDelegate0() {};
		virtual ReturnT Call() = 0;
};

/*!-----------------------------------------------------------------------------
Delegate a class that uses templates to create an implementation for each type
of handler needed for methods in different derived Listener classes
*/
template <typename ListenerT, typename ReturnT>
class CDelegateMethod0 : public CDelegate0<ReturnT>
{	
	private:
		typedef ReturnT (ListenerT::*PMember)();
		ListenerT* _object;
		PMember _member;

	public:
		//Constructor
		CDelegateMethod0(ListenerT* object, PMember member) : _object(object), _member(member) { }

		//Notification function
		ReturnT Call() { return (_object->*_member)(); }	
};

/*!-----------------------------------------------------------------------------
Delegate a class that uses templates to create an implementation for each type
of handler needed for static/global functions (not in classes)
*/
template <typename ReturnT>
class CDelegateFunction0 : public CDelegate0<ReturnT>
{	
	private:
		typedef ReturnT (*PFunction)();		
		PFunction _function;

	public:
		//Constructor
		CDelegateFunction0(PFunction func) : _function(func) { }

		//Notification function
		ReturnT Call()
		{
			return (*_function)();		
		}	
};

/*!-----------------------------------------------------------------------------
Class that stores a pointer to a delegate object, and executes it when the Call
method is called
This class is coded to accept no delegate parameters
*/
template <typename ReturnT>
class CCallback0 {
	private :		
		typedef CDelegate0<ReturnT>* PDelegate;
		PDelegate _delegate;
		
	public :
		//Constructor
		CCallback0() { _delegate = NULL; }
		
		//Destructor
		~CCallback0() { this->Clear(); }
		
		//Function called to execute the callback and return its value (or the default value of the return type if not defined)
		ReturnT Call() { if(_delegate) return _delegate->Call(); else return ReturnT();}
		
		//Clear Function
		inline void Clear() { if(_delegate) { delete _delegate; } }
		
		//Set the Callback to execute an objects member function
		template <typename ListenerT>
		void Set(ListenerT* object, ReturnT (ListenerT::*member)())
		{
			this->Clear();
			_delegate = new CDelegateMethod0<ListenerT, ReturnT>(object, member); 			
		}
		
		//Sets the Callback to execute a static or global function
		void Set(ReturnT (*func)())
		{
			this->Clear();
			_delegate = new CDelegateFunction0<ReturnT>(func);			
		}			
};

//==============================================================================
//One Parameter Classes
//==============================================================================
/*!-----------------------------------------------------------------------------
Declare an abstract base class from which all EventHandlers derive, allowing us
to attach different types of event handles (member functions with same types of 
parameters from different classes) to the same event. 
We use templates to make it generic for any combination of parameter types in 
“delegate” or call back method. There are different event types for every number
of arguments in callback function.
*/
template <typename ReturnT, typename Param1T>
class CDelegate1
{
	public:
		virtual ~CDelegate1() {};
		virtual ReturnT Call(Param1T param) = 0;
};

/*!-----------------------------------------------------------------------------
Delegate a class that uses templates to create an implementation for each type
of handler needed for methods in different derived Listener classes
*/
template <typename ListenerT, typename ReturnT, typename Param1T>
class CDelegateMethod1 : public CDelegate1<ReturnT, Param1T>
{	
	private:
		typedef ReturnT (ListenerT::*PMember)(Param1T);
		ListenerT* _object;
		PMember _member;

	public:
		//Constructor
		CDelegateMethod1(ListenerT* object, PMember member) : _object(object), _member(member) { }		

		//Notification function
		ReturnT Call(Param1T param) { return (_object->*_member)(param); }	
};

/*!-----------------------------------------------------------------------------
Delegate a class that uses templates to create an implementation for each type
of handler needed for static/global functions (not in classes)
*/
template <typename ReturnT, typename Param1T>
class CDelegateFunction1 : public CDelegate1<ReturnT, Param1T>
{	
	private:
		typedef ReturnT (*PFunction)(Param1T);		
		PFunction _function;

	public:
		//Constructor
		CDelegateFunction1(PFunction func) : _function(func) { }

		//Notification function
		ReturnT Call(Param1T param) { return (*_function)(param); }	
};


/*!-----------------------------------------------------------------------------
Class that stores a pointer to a delegate object, and executes it when the Call
method is called
This class is coded to accept no delegate parameters
*/
template <typename ReturnT, typename Param1T>
class CCallback1 {
	private :		
		typedef CDelegate1<ReturnT, Param1T>* PDelegate;
		PDelegate _delegate;
		
	public :
		//Constructor
		CCallback1() { _delegate = NULL; }
		
		//Destructor
		~CCallback1() { this->Clear(); }
		
		//Function called to execute the callback and return its value (or the default value of the return type if not defined)
		ReturnT Call(Param1T param1) { if(_delegate) return _delegate->Call(param1); else return ReturnT();}
		
		//Clear Function
		inline void Clear() { if(_delegate) { delete _delegate; } }
		
		//Set the Callback to execute an objects member function
		template <typename ListenerT>
		void Set(ListenerT* object, ReturnT (ListenerT::*member)(Param1T))
		{
			this->Clear();
			_delegate = new CDelegateMethod1<ListenerT, ReturnT, Param1T>(object, member); 			
		}
		
		//Sets the Callback to execute a static or global function
		void Set(ReturnT (*func)(Param1T))
		{
			this->Clear();
			_delegate = new CDelegateFunction1<ReturnT, Param1T>(func);			
		}			
};

//==============================================================================
//Common Event Type Definitions
//==============================================================================
/*! Define a simple callback for functions that take no parameters, and return nothing */
typedef CCallback0<void> CCallback;

/*! Define a pointer to a simple callback */
typedef CCallback* PCallback;

//------------------------------------------------------------------------------
/*! Define a standard single parameter Notification event - the first param may hold a pointer to the sender object */
struct TNotifyCallbackParams {
	pointer Sender;
};

/*! Define a simple notification event, that can pass the sender */
typedef CCallback1<void, TNotifyCallbackParams&> CNotifyCallback;

//==============================================================================
#endif
