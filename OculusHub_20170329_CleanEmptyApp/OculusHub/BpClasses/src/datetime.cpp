#include "datetime.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
//CDateTime
//==============================================================================
//Initialise static variables...

//Lookup table for number of days in a month (for date conversion functions)
uint8 CDateTime::_monthDaysNormal[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
uint8 CDateTime::_monthDaysLeap[12]   = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//uint16 CDateTime::_daysToMonth365[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
//uint16 CDateTime::_daysToMonth366[13] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

/*!-----------------------------------------------------------------------------
Function that returns the number of days in the specified month of the specified
year.
If no year is specified, it is assumed the year is not a leap year
@param month The month value to look up (from 1 to 12)
@param year The year to take into account
@result The number of days in the month
*/
uint8 CDateTime::DaysInMonth(uint8 month, int16 year)
{
	//Determine if we have a leap year
	bool leapYear = CDateTime::IsLeapYear(year);	
	
	//Get a pointer to a lookup table with the number of days in
	puint8 pDays;
	if(!leapYear)
		pDays = &CDateTime::_monthDaysNormal[0];
	else
		pDays = &CDateTime::_monthDaysLeap[0];
	
	if(month >= 1 && month <= 12)
		return pDays[month];
	else
		return 0; //throw CException("Invalid month specified");
}

/*!-----------------------------------------------------------------------------
Function that converts a Date expressed in fields into a TDateTime value
*/
TDateTime CDateTime::FromDateFields(const TDateFields& value)
{	
	TDateTime result = 0;
	
	//Determine if we have a leap year
	bool leapYear = CDateTime::IsLeapYear(value.year);	
	
	//Get a pointer to a lookup table with the number of days in
	puint8 pDays;
	if(!leapYear)
		pDays = &CDateTime::_monthDaysNormal[0];
	else
		pDays = &CDateTime::_monthDaysLeap[0];
	
	//Check the date is valid
	bool valid;
	valid = (value.year >= 1);
	valid &= (value.year <= 9999);
	valid &= (value.month >= 1);
	valid &= (value.month <= 12);
	valid &= (value.day >= 1);
	valid &= (value.day <= pDays[value.month - 1]) ;
	
	if(valid) { 				
		//Start with the current day
		result = value.day;
		
		//Add month durations
		for(uint8 m = 1; m < value.month; m++)
			result += pDays[m-1];
			
		////Get number of whole days in the years
		int16 i = value.year - 1;		
		result += (TDateTime)((i * 365) + (i / 4) - (i / 100) + (i / 400) - DT_DATE_DELTA);
				
		return result;
	}
	else
		return DT_INVALID;	
}

/*!-----------------------------------------------------------------------------
Function that converts a Date and Time expressed as fields into a TDateTime value
*/
TDateTime CDateTime::FromDateTimeFields(const TDateTimeFields& value)
{
	TDateTime result;
	result = CDateTime::FromDateFields(value.date);
	result += CDateTime::FromTimeFields(value.time);
	return result;
}

/*!-----------------------------------------------------------------------------
Function that converts a Time expressed as fields into a TDateTime value
*/
TDateTime CDateTime::FromTimeFields(const TTimeFields& value)
{
	TDateTime result;
	
	result = value.hour / DT_HOURS_PER_DAY;
	result += value.minute / DT_MINS_PER_DAY;
	result += value.second / DT_SECS_PER_DAY;
	result += value.millisecond / DT_MSECS_PER_DAY;
	
	return result;
}

/*!-----------------------------------------------------------------------------
Convert a Unix date and time value to a DateTime value
@param value The unix date/time value (ref 1/1/1970)
*/
inline TDateTime CDateTime::FromUnix(TTimeUnix value)
{	
	return ((double)value / DT_SECS_PER_DAY) + DT_UNIX_START; 
}

/*!-----------------------------------------------------------------------------
Function that returns true if the specified year is a leap year
Years that are integer multiples of 100 are not leap years, unless they are also
integer multiples of 400, in which case they are leap years.
For example, 1600 and 2000 were leap years, but 1700, 1800 and 1900 were not. 
Similarly, 2100, 2200, 2300, 2500, 2600, 2700, 2900 and 3000 will not be leap
years, but 2400 and 2800 will be.
*/
inline bool CDateTime::IsLeapYear(uint16 year)
{
	 return ((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0));
}
		
/*!-----------------------------------------------------------------------------
Function that converts a TDateTime to fields in a TDateFields structure
@param value The DateTime value to convert
@result Struct containing the Date broken down into fields
*/
TDateFields CDateTime::ToDateFields(TDateTime value) 
{	 		
	TDateFields date;
	uint16 i;
	uint16 d;
	bool leapYear;
	puint8 pDays;
		
	//Discard the time part of the input
	int32 valueDate = (int32)value + DT_DATE_DELTA;		
	
	if(valueDate < 0) {
		//Invalid date
		date.year = 0;
		date.month = 0;
		date.day = 0;	
	}
	else {
		//Try to decode the date		
		
		//Compute the day of the week...
		//DayOfWeek = (valueDate % 7) + 1;		
		
		//Compute the years...
		valueDate--;		
		date.year = 1;
		
		while(valueDate >= DT_D400) {
			valueDate -= DT_D400;
			date.year += 400;
		}
		
		i = valueDate / DT_D100;
		d = valueDate % DT_D100;				
		if(i == 4) {
			i--;
			d += DT_D100;
		}
		date.year += i * 100;
		
		i = valueDate / DT_D4;
		d = valueDate % DT_D4;			
		date.year += i * 4;
		
		i = d / DT_D1;
		d = d % DT_D1;	
		if(i == 4) {
			i--;
			d += DT_D1;
		}	
		date.year += i;
		
		//Compute the month...
		leapYear = CDateTime::IsLeapYear(date.year);
		
		//Get a pointer to a lookup table with the number of days in		
		if(!leapYear)
			pDays = &CDateTime::_monthDaysNormal[0];
		else
			pDays = &CDateTime::_monthDaysLeap[0];
	
		date.month = 1;
		while(true) {
			i = pDays[date.month - 1];
			if(d < i)
				break;
			d -= i;
			date.month++;
		}
		
		date.day = d + 1;
	}
	
	return date;
}

/*!-----------------------------------------------------------------------------
Function that converts a DateTime value into a formatted date string
@param value The value to convert to a string
@param format The style the string should be constructed in
@param result Reference to a string to receive the value
*/
void CDateTime::ToDateString(TDateTime value, EDateFormat format, string& result)
{
	TDateFields fields = CDateTime::ToDateFields(value);
	CDateTime::ToDateString(fields, format, result);
}

/*!-----------------------------------------------------------------------------
Function that converts a Date fields value into a formatted date string
@param value The value to convert to a string
@param format The style the string should be constructed in
@param result Reference to a string to receive the value
*/
void CDateTime::ToDateString(TDateFields value, EDateFormat format, string& result)
{
	char chars[32];
	char month[16];
	
	result.clear();
	
	switch(format) {
		case dfNumericBE : {
			//Big endian, numeric (yyyy-mm-dd)
			snprintf(chars, 32, "%0.4d-%0.2d-%0.2d", value.year, value.month, value.day);	
			break;
		}
		case dfNumericME : {
			//Middle endian, numeric (mm-dd-yyyy)
			snprintf(chars, 32, "%0.2d-%0.2d-%0.4d", value.month, value.day, value.year);	
			break;
		}
		case dfNumericLE : {
			//Little endian, numeric (dd-mm-yyyy)	
			snprintf(chars, 32, "%0.2d-%0.2d-%0.4d", value.day, value.month, value.year);	
			break;
		}
		case dfShortLE : {
			//Little endian, short (dd MMM yyyy)
			
			//Get the month name
			switch(value.month) {
				case 1 : snprintf(month, 16, "Jan"); break;
				case 2 : snprintf(month, 16, "Feb"); break;
				case 3 : snprintf(month, 16, "Mar"); break;
				case 4 : snprintf(month, 16, "Apr"); break;
				case 5 : snprintf(month, 16, "May"); break;
				case 6 : snprintf(month, 16, "Jun"); break;
				case 7 : snprintf(month, 16, "Jul"); break;
				case 8 : snprintf(month, 16, "Aug"); break;
				case 9 : snprintf(month, 16, "Sep"); break;
				case 10 : snprintf(month, 16, "Oct"); break;
				case 11 : snprintf(month, 16, "Nov"); break;
				case 12 : snprintf(month, 16, "Dec"); break;
				default : snprintf(month, 16, "---"); break;
			}			
			
			snprintf(chars, 32, "%0.2d %s %0.4d", value.day, month, value.year);						
			break;
		}
		case dfLongLE : {
			//Little endian, long (d MMMMM yyyy)
			
			//Get the month name
			switch(value.month) {
				case 1 : snprintf(month, 16, "January"); break;
				case 2 : snprintf(month, 16, "February"); break;
				case 3 : snprintf(month, 16, "March"); break;
				case 4 : snprintf(month, 16, "April"); break;
				case 5 : snprintf(month, 16, "May"); break;
				case 6 : snprintf(month, 16, "June"); break;
				case 7 : snprintf(month, 16, "July"); break;
				case 8 : snprintf(month, 16, "August"); break;
				case 9 : snprintf(month, 16, "September"); break;
				case 10 : snprintf(month, 16, "October"); break;
				case 11 : snprintf(month, 16, "November"); break;
				case 12 : snprintf(month, 16, "December"); break;
				default : snprintf(month, 16, "Error"); break;
			}
			
			snprintf(chars, 32, "%d %s %0.4d", value.day, month, value.year);			
			break;
		}
		case dfISO : {
			//ISO 8601 format (yyyyddmm)
			snprintf(chars, 32, "%0.4d%0.2d%0.2d", value.year, value.month, value.day);	
			break;
		}
		default : {
			return;
		}
	}
	
	//Create an output string
	result.assign(chars);
}

/*!-----------------------------------------------------------------------------
Function that converts a TDateTime to fields in a TDateTimeFields structure
@param value The DateTime value to convert
@result Struct containing the Date and Time broken down into fields
*/
TDateTimeFields CDateTime::ToDateTimeFields(TDateTime value)
{
	TDateTimeFields fields;
	fields.date = CDateTime::ToDateFields(value);
	fields.time = CDateTime::ToTimeFields(value);
	return fields;
}

/*!-----------------------------------------------------------------------------
Retrieves a valid date and time for the FatFS file system.
*/
uint32 CDateTime::ToFatTime(TDateTimeFields fields) 
{
	uint32 fattime = 0;
	
	// The date and time format is as of here
	// http://elm-chan.org/fsw/ff/en/fattime.html	
	fattime |= ((fields.date.year - 1980) << 25);
	fattime |= (fields.date.month << 21);
	fattime |= (fields.date.day << 16);
	fattime |= (fields.time.hour << 11);
	fattime |= (fields.time.minute << 5);
	fattime |= (fields.time.second >> 1);
	
	return fattime;
}

/*!-----------------------------------------------------------------------------
Function that converts a DateTime value into a formatted string
@param value The value to convert to a string
@param dateFormat The style the date part of the string should be constructed in
@param timeFormat The style the time part of the string should be constructed in
@param result Reference to a string to receive the value
*/
void CDateTime::ToString(TDateTime value, EDateFormat dateFormat, ETimeFormat timeFormat, string& result)
{
	TDateTimeFields fields = CDateTime::ToDateTimeFields(value);
	CDateTime::ToString(fields, dateFormat, timeFormat, result);
}

/*!-----------------------------------------------------------------------------
Function that converts a DateTime fields value into a formatted string
@param value The value to convert to a string
@param format The style the string should be constructed in
@param result Reference to a string to receive the value
*/
void CDateTime::ToString(TDateTimeFields value, EDateFormat dateFormat, ETimeFormat timeFormat, string& result)
{
	string date;
	string time;
		
	CDateTime::ToDateString(value.date, dateFormat, date);
	CDateTime::ToTimeString(value.time, timeFormat, time);
	
	result.clear();
	result = date + ' ' + time;	
}

/*!-----------------------------------------------------------------------------
Function that converts a TDateTime to fields in a TTimeFields structure
@param value The DateTime value to convert
@result Struct containing the Time broken down into fields
*/
TTimeFields CDateTime::ToTimeFields(TDateTime value)
{
	TTimeFields time;
	
	//Extract the time part of the value
	double timeValue = CMaths::Fraction((double)value);
	
	timeValue *= 24.0;
	time.hour = (uint8)timeValue;
	
	timeValue -= time.hour;
	timeValue *= 60;
	time.minute = (uint8)timeValue;
	
	timeValue -= time.minute;
	timeValue *= 60;
	time.second = (uint8)timeValue;
	
	timeValue -= time.second;
	timeValue *= 1000;
	time.millisecond = (uint16)timeValue;
	
	return time;
}

/*!-----------------------------------------------------------------------------
Function that converts a DateTime value into a formatted time string
@param value The value to convert to a string
@param format The style the string should be constructed in
@param result Reference to a string to receive the value
*/
void CDateTime::ToTimeString(TDateTime value, ETimeFormat format, string& result)
{
	TTimeFields fields = CDateTime::ToTimeFields(value);
	CDateTime::ToTimeString(fields, format, result);
}

/*!-----------------------------------------------------------------------------
Function that converts a Time fields value into a formatted time string
@param value The value to convert to a string
@param format The style the string should be constructed in
@param result Reference to a string to receive the value
*/
void CDateTime::ToTimeString(TTimeFields value, ETimeFormat format, string& result)
{
	char chars[16];
	char suffix[3];
	uint8 hour12;
	
	result.clear();
		
	//Handle 12 hour formats
	if(format == tfLong12 || format == tfShort12) {		
		hour12 = value.hour % 12;
		if(value.hour == 0)
			hour12 = 12;		
				
		if(value.hour < 12)
			suffix[0] = 'a';
		else 
			suffix[0] = 'p';
		suffix[1] = 'm';
		suffix[2] = 0;
	}		
	
	//Create characters for the output string
	switch(format) {
		case tfLong24 : {		
			//24 hour long (hh:mm:ss)
			snprintf(chars, 16, "%0.2d:%0.2d:%0.2d", value.hour, value.minute, value.second);
			break;
		}
		case tfShort24 : {		
			//24 hour short (hh:mm)
			snprintf(chars, 16, "%0.2d:%0.2d", value.hour, value.minute);
			break;
		}	
		case tfLong12 :	{	
			//12 hour long (h:mm:ss am/pm)
			snprintf(chars, 16, "%d:%0.2d:%0.2d %s", hour12, value.minute, value.second, suffix);
			break;
		}		
		case tfShort12 : {		
			//12 hour short (h:mm am/pm)
			snprintf(chars, 16, "%d:%0.2d %s", hour12, value.minute, suffix);
			break;
		}
		case tfISO : {
			//ISO8601 format (hhmmss)
			snprintf(chars, 16, "%0.2d%0.2d%0.2d", value.hour, value.minute, value.second);
			break;
		}
		default :  {
			return;
		}
	}	
	
	//Create an output string
	result.assign(chars);
}

/*!-----------------------------------------------------------------------------
Converts a DateTime value to a Unix date and time value
*/
TTimeUnix CDateTime::ToUnix(TDateTime value)
{	
	return (TTimeUnix)CMaths::Round((value - DT_UNIX_START) * DT_SECS_PER_DAY); 
} 

//==============================================================================
//CTimeSpan
//==============================================================================
/*!-----------------------------------------------------------------------------
Function that parses a DateTime into a timespan fields structure
@param value The value to convert
@result A timespan fields structure
*/
TTimeSpan CTimeSpan::FromDateTime(TDateTime value)
{
	TTimeSpan span;
	
	//Get the number of whole days
	span.days = (int32)value;
	
	//Extract the time part of the value
	double timeValue = CMaths::Fraction((double)value);
	
	timeValue *= 24.0;
	span.hours = (int8)timeValue;
	
	timeValue -= span.hours;
	timeValue *= 60;
	span.minutes = (int8)timeValue;
	
	timeValue -= span.minutes;
	timeValue *= 60;
	span.seconds = (int8)timeValue;
	
	timeValue -= span.seconds;
	timeValue *= 1000;
	span.milliseconds = (int16)timeValue;
	
	return span;
}

/*!-----------------------------------------------------------------------------
Function that converts the specified number of seconds to a timespan object
*/
TTimeSpan CTimeSpan::FromSeconds(double value)
{
	TTimeSpan span;
	
	//Compute the whole number of days
	span.days = (int32)(value / DT_SECS_PER_DAY);
	//Compute the remaining hours
	value -= span.days * DT_SECS_PER_DAY;
	span.hours = (int8)(value / DT_SECS_PER_HOUR);
	//Compute the remaining minutes
	value -= span.hours * DT_SECS_PER_HOUR;
	span.minutes = (int8)(value / DT_SECS_PER_MIN);
	//Compute the remaining seconds
	value -= span.minutes * DT_SECS_PER_MIN;
	span.seconds = (int8)(value);
	//Compute the milliseconds
	value -= span.seconds;
	value *= 1000.0;
	span.milliseconds = (int16)(value);
	
	return span;
}

//==============================================================================
