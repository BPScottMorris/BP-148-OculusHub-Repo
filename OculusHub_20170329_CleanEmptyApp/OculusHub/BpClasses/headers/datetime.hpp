/*==============================================================================
File that implements function relating to Date and Time, and Timing mechanisms

The type TDATETIME is a double, based on the Delphi standard, the integral part
of a TDateTime value is the number of days that have passed since December 30, 1899.
The fractional part of a TDateTime value is the time of day.
The maximal correct date supported by TDateTime values is limited
to 12/31/9999 23:59:59:999.
The Delphi TDateTime value is a double where...
  0 = December 30, 1899; 12:00 A.M.
  2.75 = January 1, 1900; 6:00 P.M.
  -1.25 = December 29, 1899; 6:00 A.M.
  35065 = January 1, 1996; 12:00 A.M.

For easier inter-platform comms, the TDATETIMEFIELDS, TDATEFIELDS and TTIMEFIELDS
structures break down a date a time value into a set of fields containing each
component part. In total 9 bytes are needed for a full Date and Time with 1ms
accuracy.

The Unix functions use a Uint32 type and refer to the number elapsed seconds
since the unix epoch (1/1/1970). This format is stored in the RTC and has no
fractional seconds component
NB: Using unsigned 32 bit storage, Unix times can only represent up to 06:28:15 UTC on
Sun, 7 February 2106.

The Ticks functions are defined as a UInt64 and specify the time in a resolution
of 1/32768 of a second (the interval of the RTC prescalar).

--------------------------------------------------------------------------------
As its timing base, the DateTime functions solely use the RTC as the time source
as this contains a 32-bit seconds counter, that counts in UNIX time (seconds
since 1/1/1970), and a 15-bit prescalar that counts the number of 1/32768 time
periods in each second.

The GetNow() family of functions access the RTC and read this value out.

The GetTicks() function forms the basis of any polled system timing mechanism, as
it concatenates the RTC values to produce a Unix like value, but to a resolution
of 1/32768 of a second being 1 count.

The SysTick function of the ARM hardware is not used.

--------------------------------------------------------------------------------
For further details on DateTime formats, see...
  http://en.wikipedia.org/wiki/System_time
  http://msdn.microsoft.com/en-gb/library/windows/desktop/ms724950%28v=vs.85%29.aspx
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef DATETIME_HPP
#define DATETIME_HPP

//Include system libraries
#include <stdio.h>		//For snprintf function

//Include common type definitions and macros
#include "common.h"
#include "maths.hpp"

//==============================================================================
//Global Definitions
//==============================================================================
//Days between 1/1/0001 and 12/31/1899
#define DT_DATE_DELTA		693594

//Sets UnixStartDate to TDateTime of 01/01/1970
//Days between TDateTime basis (12/31/1899) and Unix time_t basis (1/1/1970)
#define DT_UNIX_START		25569

//Define the resolution of a single tick count, in seconds
//#define DT_TICKS_PER_SEC	32768.0
//#define DT_TICK_RES			(1.0 / DT_TICKS_PER_SEC)
//#define DT_TICKS_PER_MSEC	(DT_TICKS_PER_SEC / 1000.0)

//Units of time
#define DT_HOURS_PER_DAY	24.0
#define DT_MINS_PER_HOUR	60.0
#define DT_SECS_PER_MIN		60.0
#define DT_MSECS_PER_SEC	1000.0
#define DT_MINS_PER_DAY		(DT_HOURS_PER_DAY * DT_MINS_PER_HOUR)
#define DT_SECS_PER_DAY		(DT_MINS_PER_DAY * DT_SECS_PER_MIN)
#define DT_SECS_PER_HOUR	(DT_SECS_PER_MIN * DT_MINS_PER_HOUR)
#define DT_MSECS_PER_DAY    (DT_SECS_PER_DAY * DT_MSECS_PER_SEC)

//Definitions used internally within the DateTime to Fields conversion function
//that contains the whole number of days within yearly periods, account for
//leap year rules.
#define DT_D1	365						/* Days in 1 year period */
#define DT_D4	((DT_D1 * 4) + 1)		/* Days in 4 year period */
#define DT_D100	((DT_D4 * 25) - 1)		/* Days in 100 year period */
#define DT_D400	((DT_D100 * 4) + 1)     /* Days in 400 year period */

//Define a result used for a date time null/error value
#define DT_INVALID			-1

//==============================================================================
//Class Definition...
//==============================================================================
/*! Type that holds a unix time, in seconds since 1/1/1970 */
typedef uint32 TTimeUnix;

/*! Define a pointer to a Unix time */
typedef TTimeUnix* PTimeUnix;

/*!
Type that represents the time in ticks. The period of a tick is undefined and
dependent on the hardware generating it.
*/
typedef int64 TTimeTicks;

/*! Define a pointer to a ticks value */
typedef TTimeTicks* PTimeTicks;

/*!
Define a type to hold Date and Time values (using the Delphi format/epoch)
The whole part of the number represents days.
The fractional part of the number represent the time in a 24 hour period.
*/
typedef double TDateTime;

/*!
Pointer to a TDateTime object
*/
typedef TDateTime* PDateTime;

/*! Structure that contains an expanded date in individual fields */
struct TDateFields {
	uint16 year;
	uint8 month;
	uint8 day;
};

/*! Structure that contains an expanded time in individual fields */
struct TTimeFields {
	uint8 hour;
	uint8 minute;
	uint8 second;
	uint16 millisecond;
};

/*! Structure that contains an expanded date and time in individual fields */
struct TDateTimeFields{
	TDateFields date;
	TTimeFields time;
};

/*! Define a pointer to an expanded fate and time value */
typedef TDateTimeFields* PDateTimeFields;

/*! Structure than can express a time span */
struct TTimeSpan {
	int32 days;
	int8 hours;
	int8 minutes;
	int8 seconds;
	int16 milliseconds;
};

/*! Define a pointer to a time span */
typedef TTimeSpan* PTimeSpan;

/*! Enumeration defining types of Date format */
enum EDateFormat {
	dfNumericBE,	//Big endian, numeric (yyyy-mm-dd)
	dfNumericME,	//Middle endian, numeric (mm-dd-yyyy)
	dfNumericLE,	//Little endian, numeric (dd-mm-yyyy)
	dfShortLE,		//Little endian, short (dd MMM yyyy)
	dfLongLE,		//Little endian, long (d MMMMM yyyy)
	dfISO,			//ISO 8601 format (yyyymmdd)
	dfNone
};

/*! Enumeration defining types of Time format */
enum ETimeFormat {
	tfLong24,		//24 hour long (hh:mm:ss)
	tfLong12,		//12 hour long (h:mm:ss am/pm)
	tfShort24,		//24 hour short (hh:mm)
	tfShort12,		//12 hour short (h:mm am/pm)
	tfISO,			//ISO 8601 24 hour (hhmmss)
	tfNone
};


/*!
Class of functions that manipulate a date/time function
*/
class CDateTime {
	private:
		//Lookup table for number of days in a month
		static uint8 _monthDaysNormal[12];
		static uint8 _monthDaysLeap[12];
		//static uint16 _daysToMonth365[13];
		//static uint16 _daysToMonth366[13];

	public:
		//Static methods...
		static double CalcTickInterval(double freq);
		static uint8 DaysInMonth(uint8 month, int16 year = 0);
		static TDateTime FromDateFields(const TDateFields& value);
		static TDateTime FromDateTimeFields(const TDateTimeFields& value);
		static TDateTime FromTimeFields(const TTimeFields& value);
		static TDateTime FromUnix(TTimeUnix value);
		static bool IsLeapYear(uint16 year);
		static TDateFields ToDateFields(TDateTime value);
		static void ToDateString(TDateTime value, EDateFormat format, string& result);
		static void ToDateString(TDateFields value, EDateFormat format, string& result);
		static TDateTimeFields ToDateTimeFields(TDateTime value);
		static uint32 ToFatTime(TDateTimeFields fields);
		static void ToString(TDateTime value, EDateFormat dateFormat, ETimeFormat timeFormat, string& result);
		static void ToString(TDateTimeFields value, EDateFormat dateFormat, ETimeFormat timeFormat, string& result);
		static TTimeFields ToTimeFields(TDateTime value);
		static void ToTimeString(TDateTime value, ETimeFormat format, string& result);
		static void ToTimeString(TTimeFields value, ETimeFormat format, string& result);
		static TTimeUnix ToUnix(TDateTime value);
};

/*!
Class that provides helper functions for working with timespan objects
*/
class CTimeSpan {
	public:
		static TTimeSpan FromDateTime(TDateTime value);
		static TTimeSpan FromSeconds(double value);
};

//==============================================================================
#endif
