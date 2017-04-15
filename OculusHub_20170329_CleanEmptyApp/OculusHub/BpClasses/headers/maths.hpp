/*==============================================================================

==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef MATHS_HPP
#define MATHS_HPP

//Include the system maths library
//#include <math.h>

//Include common type definitions and macros
#include "common.h"

//==============================================================================
//Mathematical Definitions
//==============================================================================
//#define PI 		3.14159265358979
#define PI			3.1415926535897932384626433832795
#define TWO_PI		(PI * 2)
#define HALF_PI		(PI / 2)

#define RAD_TO_DEG	(180.0 / PI)	/* Multiply radians by this to get degrees */
#define DEG_TO_RAD  (PI / 180.0)	/* Multiply degrees by this to get radians */

//==============================================================================
//Definitions...
//==============================================================================
/*!-----------------------------------------------------------------------------
Macro that allows quick checking that a value lies inclusively between
the specified min and max values.
This effectively specifies a "Closed Interval" range (see "http://en.wikipedia.org/wiki/Interval_(mathematics)")
*/
#define MATHS_RANGE_CHECK(VALUE, VALMIN, VALMAX)	(((VALUE) >= (VALMIN)) && ((VALUE) <= (VALMAX)))

/*!-----------------------------------------------------------------------------
Macro that allows quick clipping of a value to the limits of the specified
range
*/
#define MATHS_RANGE_CLIP(VALUE, VALMIN, VALMAX)		{ \
	if((VALUE) < (VALMIN)) \
		VALUE = (VALMIN); \
	else if((VALUE) > (VALMAX)) \
		VALUE = (VALMAX); \
}

/*!-----------------------------------------------------------------------------
Macro that allows quick clipping of a value to the limits of the specified
range
*/
#define MATHS_RANGE_CLIPMAX(VALUE, VALMAX)		{ \
	if((VALUE) > (VALMAX)) \
		VALUE = (VALMAX); \
}

/*!-----------------------------------------------------------------------------
Macro that performs a cyclic modulus function on a value between a min
and max values.
Note that the value will never equal the max value - i.e.
	MATHS_RANGE_MODULUS(3, 5, 8) = 7
	MATHS_RANGE_MODULUS(-270.0, -180.0, 180.0) = 90.0;
	MATHS_RANGE_MODULUS(360.0, 0.0, 360.0) = 360.0;
*/
#define MATHS_RANGE_MODULUS(VALUE, VALMIN, VALMAX)	{ \
	while(VALUE < VALMIN) \
		VALUE += ((VALMAX) - (VALMIN)); \
	while(VALUE >= VALMAX) \
		VALUE -= ((VALMAX) - (VALMIN)); \
}

//------------------------------------------------------------------------------
/*!
Define a class that provide mathematical helper functions
*/
class CMaths {
	public:
		static inline float DegreesToRadians(float angle);
		static inline double DegreesToRadians(double angle);

		static float Fraction(float value);
		static double Fraction(double value); // { return value - (double)((int32)value); }

		static inline float InverseSqrt(float number);

		static inline float NormaliseDegrees(float angle);
		static inline double NormaliseDegrees(double angle);
		static inline float NormaliseRadians(float angle);
		static inline double NormaliseRadians(double angle);

		static inline float RadiansToDegrees(float angle);
		static inline double RadiansToDegrees(double angle);

		/* See "MATHS_RANGE_XXXX" macros
		template <typename T>
		static inline void RangeCyclic(T* value, T max);

		template <typename T>
		static inline void RangeLimit(T* value, T min, T max);

		template <typename T>
		static inline bool RangeValidate(T value, T min, T max);
		*/

		static inline float RolloverDegrees(float angle);
		static inline double RolloverDegrees(double angle);
		static inline float RolloverRadians(float angle);
		static inline double RolloverRadians(double angle);

		static int32 Round(float value);
		static int32 Round(double value);

		static inline int32 RoundMultipleUp(int32 value, int32 multiple);

		static inline float Sign(float value);
		static inline double Sign(double value);
};

//==============================================================================
#endif
