#include "pid.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
//CPid
//==============================================================================
/*!-----------------------------------------------------------------------------
Constructor
*/
CPid::CPid()
{
	_period = 1.0;
	_limitOutputMax = 1.0;
	_limitOutputMin = 0.0;
	_limitTermsMax = 1.0;
	_limitTermsMin = -1.0;

	_enableI = true;
	_enableD = true;

	this->Configure(0.0, 0.0, 0.0);
	this->Reset();
}

/*!-----------------------------------------------------------------------------
Destructor
*/
CPid::~CPid()
{
}

/*!-----------------------------------------------------------------------------
Function that computes the internal variables needed by the PID controller based
on the specified control constants.
*/
void CPid::Configure()
{
	IRQ_DISABLE;

	//Compute Kd and Ki terms based on timing parameters
	_Ki = (_hasI) ? (_period / _Ti) : 0.0;
	_Kd = (_hasD) ? (_Td / _period) : 0.0;

	//Precompute scalar constants
	_KpKi = _Kp * _Ki;
	_KpKd = _Kp * _Kd;

	IRQ_ENABLE;
}

/*!-----------------------------------------------------------------------------
Function that computes the internal variables needed by the PID controller based
on the specified control constants.
*/
void CPid::Configure(float kp, float ti, float td)
{
	IRQ_DISABLE;

	_Kp = kp;
	_Ti = ti;
	_Td = td;
	_hasI = (bool)(_Ti != 0.0);
	_hasD = (bool)(_Td != 0.0);
	this->Configure();

	IRQ_ENABLE;
}

/*!-----------------------------------------------------------------------------
Function that snapshots the state of the PID controller for external use
*/
void CPid::GetState(PPidState state)
{
	IRQ_DISABLE;

	state->Kp = _Kp;
	state->Ki = _Ki;
	state->Kd = _Kd;
	state->Period = _period;
	state->OutputError = _outputError;
	state->OutputP = _outputP;
	state->OutputI = _outputI;
	state->OutputD = _outputD;
	state->OutputVal = _outputVal;

	IRQ_ENABLE;
}

/*!-----------------------------------------------------------------------------
Function that resets controller values back to their defaults
*/
void CPid::Reset()
{
	IRQ_DISABLE;

	_outputError = 0.0;
	_outputP = 0.0;
	_outputI = 0.0;
	_outputD = 0.0;
	_outputVal = 0.0;

	IRQ_ENABLE;
}

/*!-----------------------------------------------------------------------------
Function that enabled the derivative action of the PID controller
*/
void CPid::SetEnableD(bool value)
{
	_enableD = value;
}

/*!-----------------------------------------------------------------------------
Function that enabled the integral action of the PID controller
*/
void CPid::SetEnableI(bool value)
{
	_enableI = value;
}

/*!-----------------------------------------------------------------------------
Function that sets the Proportional Gain constant Kp
*/
void CPid::SetGainP(float value)
{
	this->Configure(value, _Ti, _Td);
}

/*!-----------------------------------------------------------------------------
Function that sets the min and max limits for the controller output and
simply computes the limiting terms used to make up the controller output.
*/
void CPid::SetLimits(float min, float max)
{
	IRQ_DISABLE;
	this->SetLimitsOutput(min, max);
	this->SetLimitsTerms(-max, max);
	IRQ_ENABLE;
}

/*!-----------------------------------------------------------------------------
Function that sets the min and max limits used to clip controller output
*/
void CPid::SetLimitsOutput(float min, float max)
{
	IRQ_DISABLE;
	_limitOutputMin = min;
	_limitOutputMax = max;
	IRQ_ENABLE;
}

/*!-----------------------------------------------------------------------------
Function that sets the min and max limits used to clip the P, I and D
controller output component terms
*/
void CPid::SetLimitsTerms(float min, float max)
{
	IRQ_DISABLE;
	_limitTermsMin = min;
	_limitTermsMax = max;
	IRQ_ENABLE;
}

/*!-----------------------------------------------------------------------------
Function that sets the period the PID compute Update function will be called at
*/
void CPid::SetPeriod(float value)
{
	_period = value;
	this->Configure();
}

/*!-----------------------------------------------------------------------------
Function that sets the Derivative Action Time constant Td, from which Kd is
computed based on the sample/update period
*/
void CPid::SetTimeD(float value)
{
	this->Configure(_Kp, _Ti, value);
}

/*!-----------------------------------------------------------------------------
Function that sets the Integral Action Time constant Ti, from which Ki is
computed based on the sample/update period
*/
void CPid::SetTimeI(float value)
{
	this->Configure(_Kp, value, _Td);
}

/*!-----------------------------------------------------------------------------
Function that updates the controller outputs based on new feedback and demand
values from the system and control logic
*/
float CPid::Update(float feedback, float demand)
{
	IRQ_DISABLE;

	//Compute the Error
	//float error = feedback - demand;
	float error = demand - feedback;

	/*
	//First-order Filter the error if required
	float errorFlt = (error - _outputError) * _errorGain;
	error = errorFlt;
	*/

	/*
	//Impose a deadband (to reduce hunting) if required
	float errorAbs = (error < 0.0) ? -error : error;
	if(errorAbs < _errorDeadband) {
		error = 0.0;
	}
	*/

	//Compute change in Error (for D terms)
	float errorDelta = error - _outputError;

	//Store the error
	_outputError = error;

	//Compute proportional term, and apply limits
	_outputP = error * _Kp;
	if(_outputP > _limitTermsMax)
		_outputP = _limitTermsMax;
	else if(_outputP < _limitTermsMin)
		_outputP = _limitTermsMin;

	//Compute Integral term (accumulate error), and apply limits
	if(_hasI) {
		if(_enableI) {
			_outputI = _outputI + (error * _KpKi);

			if(_outputI > _limitTermsMax)
				_outputI = _limitTermsMax;
			else if(_outputI < _limitTermsMin)
				_outputI = _limitTermsMin;
		}
	}
	else {
		_outputI = 0.0;
	}

	//Compute Differential term (change in error), and apply limits
	if(_hasD) {
		if(_enableD) {
			_outputD = errorDelta * _KpKd;

			if(_outputD > _limitTermsMax)
				_outputD = _limitTermsMax;
			else if(_outputD < _limitTermsMin)
				_outputD = _limitTermsMin;
		}
	}
	else {
		_outputD = 0.0;
	}

	//Compute the filter output and apply limits
	_outputVal = _outputP + _outputI + _outputD;

	if(_outputVal > _limitOutputMax)
		_outputVal = _limitOutputMax;
	else if(_outputVal < _limitOutputMin)
		_outputVal = _limitOutputMin;

	IRQ_ENABLE;

	return _outputVal;
}

//==============================================================================
