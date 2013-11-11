//
// --------------------------------------------------------------------------
//  Gurux Ltd
// 
//
//
// Filename:        $HeadURL$
//
// Version:         $Revision$,
//                  $Date$
//                  $Author$
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
//
//  DESCRIPTION
//
// This file is a part of Gurux Device Framework.
//
// Gurux Device Framework is Open Source software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License 
// as published by the Free Software Foundation; version 2 of the License.
// Gurux Device Framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// More information of Gurux products: http://www.gurux.org
//
// This code is licensed under the GNU General Public License v2. 
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------

#include "GXDateTime.h"
#include <string.h>

// Constructor.
CGXDateTime::CGXDateTime()
{
	m_Skip = DATETIME_SKIPS_NONE;
	memset(&m_Value, 0xFF, sizeof(m_Value));		
}

// Constructor.
CGXDateTime::CGXDateTime(struct tm value)
{
    m_Value = value;
	m_Skip = DATETIME_SKIPS_NONE;
}

// Constructor.    
CGXDateTime::CGXDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
{	
	int skip = DATETIME_SKIPS_NONE;
    if (year == -1)
    {
        skip |= DATETIME_SKIPS_YEAR;
        year = 0;
    }
    if (month == -1)
    {
        skip |= DATETIME_SKIPS_MONTH;
        month = 1;
    }
    if (day == -1)
    {
        skip |= DATETIME_SKIPS_DAY;
        day = 1;
    }
    if (hour == -1)
    {
        skip |= DATETIME_SKIPS_HOUR;
        hour = 0;
    }
    if (minute == -1)
    {
        skip |= DATETIME_SKIPS_MINUTE;
        minute = 0;
    }
    if (second == -1)
    {
        skip |= DATETIME_SKIPS_SECOND;
        second = 0;
    }
    if (millisecond == -1)
    {
        skip |= DATETIME_SKIPS_MS;
        millisecond = 0;
    }
	m_Skip = (DATETIME_SKIPS) skip;
	m_Value.tm_year = 1900 + year;
	m_Value.tm_mon = month - 1;
	m_Value.tm_mday = day;
	m_Value.tm_hour = hour;
	m_Value.tm_min = minute;
	m_Value.tm_sec = second;
}

// Used date time value.
struct tm& CGXDateTime::GetValue()
{
	return m_Value;
}

void CGXDateTime::SetValue(struct tm& value)
{
	m_Value = value;
}

// Skip selected date time fields.
DATETIME_SKIPS CGXDateTime::GetSkip()
{
	return m_Skip;
}

void CGXDateTime::SetSkip(DATETIME_SKIPS value)
{
	m_Skip = value;
}

basic_string<char> CGXDateTime::ToString()
{
    if (m_Skip != DATETIME_SKIPS_NONE)
    {
		basic_string<char> str;			
        if ((m_Skip & DATETIME_SKIPS_YEAR) == 0)
        {
            str += "%yyyy";
        }
        if ((m_Skip & DATETIME_SKIPS_MONTH) == 0)
        {
			if (str.size() != 0)
			{
				str.append("-");
			}
            str += "%M";
        }
        if ((m_Skip & DATETIME_SKIPS_DAY) == 0)
        {
			if (str.size() != 0)
			{
				str.append("-");
			}
            str += "d";
        }
        if ((m_Skip & DATETIME_SKIPS_HOUR) == 0)
        {
			if (str.size() != 0)
			{
				str.append(" ");
			}
            str += "%H";
        }
        if ((m_Skip & DATETIME_SKIPS_MINUTE) == 0)
        {
			if (str.size() != 0)
			{
				str.append(":");
			}
            str += "%M";
        }
        if ((m_Skip & DATETIME_SKIPS_SECOND) == 0)
        {
			if (str.size() != 0)
			{
				str.append(":");
			}
            str += "%s";
        }
		char buff[20];
		strftime(buff, 20, str.c_str(), &m_Value);
		return buff;
    }		
	char buff[20];
	strftime(buff, 20, "%Y-%m-%d %h:%M %s", &m_Value);
    return buff;
}

CGXDateTime CGXDateTime::Now()
{
	time_t tm1 = time(NULL);
#if _MSC_VER > 1000
	struct tm dt;
	localtime_s(&dt, &tm1);
#else
	struct tm dt = *localtime(&tm1);
#endif	
	CGXDateTime now(dt);
	return now;
}
