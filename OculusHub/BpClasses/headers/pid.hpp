/*==============================================================================
Module that implements a PID controller as part of a motor control system.
The functions have been written to allow the UPDATE function to be called on
a timer interrupt basis, and provide thread-safe access to variables.

Based on code and algorithms by I.Pearson (15/09/2009)

v1.0 - 08-08-2016, R.Sharphouse & I.Pearson

==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef PID_HPP
#define PID_HPP

//Include common type definitions and macros
#include "common.h"

//==============================================================================
//Class Definition...
//==============================================================================
/*! Structure the can hold the state of the PID controller */
struct TPidState {
	float Kp;
	float Ki;
	float Kd;
	float Period;
	float OutputError;
	float OutputP;
	float OutputI;
	float OutputD;
	float OutputVal;
};

/*! Define a pointer to a PID state structure */
typedef TPidState* PPidState;


/*! Define a class to implement a PID controller */
class CPid {
	private:
		float _Ti;					// Integral action time (seconds)
		float _Td;					// Derivative action time (seconds)
		float _Ki;					// 'constant' period / Ti
		float _Kd;					// 'constant' Td / period
		float _Kp;					// Proportional gain
		float _KpKi;				//   = Kp*Ki to save on-line calculation time
		float _KpKd;				//   = Kp*Kd to save on-line calculation time
		float _period;				// Sample period (seconds)
		float _limitOutputMin;		// Don't allow output to be less than this
		float _limitOutputMax;		// Don't allow output to exceed this
		float _limitTermsMin;
		float _limitTermsMax;
		bool _hasI;					// Has a non-zero Ki
		bool _hasD;					// Has a non-zero Kd
		bool _enableI;				// True to integrate
		bool _enableD;				// True to differentiate

		float _outputError;
		float _outputP;
		float _outputI;
		float _outputD;
		float _outputVal;

	protected:
		void Configure();

	public:
		//Constructor and Destructor
		CPid();
		~CPid();

		//Methods
		void Configure(float kp, float ti, float td);
		void GetState(PPidState state);
		void Reset();
		void SetEnableD(bool value);
		void SetEnableI(bool value);
		void SetGainP(float value);						/*!< Sets the Proportional Gain constant Kp */
		void SetLimits(float min, float max);			/*!< Sets the min and max output and components terms of the PID output - equivalent to calling LimitOutput and LimitTerms */
		void SetLimitsOutput(float min, float max);		/*!< Sets the min and max limits used to clip the output value */
		void SetLimitsTerms(float min, float max);		/*!< Sets the min and max limits used to clip the P, I and D controller component terms */
		void SetPeriod(float value);					/*!< Sets the period the PID compute Update function will be called at */
		void SetTimeD(float value);						/*!< Sets the Derivative Action Time constant Td, from which Kd is computed based on the sample/update period */
		void SetTimeI(float value);						/*!< Sets the Integral Action Time constant Ti, from which Ki is computed based on the sample/update period */
		float Update(float feedback, float demand);		/*!< Updates the controller outputs based on new feedback and demand values from the system and control logic */
};

//Define a pointer to a PID class
typedef CPid* PPid;

//==============================================================================
#endif
