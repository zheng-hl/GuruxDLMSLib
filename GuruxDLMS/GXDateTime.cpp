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
#include "GXHelpers.h"

#include <string.h>
#include <locale> 
#include <vector>
#include <assert.h>

// Constructor.
CGXDateTime::CGXDateTime()
{
	m_Skip = DATETIME_SKIPS_NONE;
	memset(&m_Value, 0xFF, sizeof(m_Value));
	m_DaylightSavingsBegin = m_DaylightSavingsEnd = false;
	m_Status = GXDLMS_CLOCK_STATUS_OK;

}

// Constructor.
CGXDateTime::CGXDateTime(struct tm value)
{
    m_Value = value;
	m_Skip = DATETIME_SKIPS_NONE;
	m_DaylightSavingsBegin = m_DaylightSavingsEnd = false;
	m_Status = GXDLMS_CLOCK_STATUS_OK;
}

CGXDateTime::CGXDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
{		
	Init(year, month, day, hour, minute, second, millisecond, 0x8000);
}

// Constructor.    
CGXDateTime::CGXDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int devitation)
{
	Init(year, month, day, hour, minute, second, millisecond, devitation);
}

// Constructor.    
void CGXDateTime::Init(int year, int month, int day, int hour, int minute, int second, int millisecond, int devitation)
{		
	memset(&m_Value, 0, sizeof(m_Value));
	m_DaylightSavingsBegin = m_DaylightSavingsEnd = false;
	m_Status = GXDLMS_CLOCK_STATUS_OK;
	int skip = DATETIME_SKIPS_NONE;
    if (year < 1 || year == 0xFFFF)
    {
        skip |= DATETIME_SKIPS_YEAR;
		year = 1900 + Now().m_Value.tm_year;
    }
    if (month < 1 || month == 0xFF)
    {
        skip |= DATETIME_SKIPS_MONTH;
        month = 0;
    }
	else
	{
		--month;
	}
    if (day < 1 || day == 0xFF)
    {
        skip |= DATETIME_SKIPS_DAY;
		day = 1;
    }
	else if (day == 0xFD)
    {        
        day = DaysInMonth(year, month) - 1;
    }
	else if (day == 0xFE)
    {        
        day = DaysInMonth(year, month);
    }	
    if (hour == -1 || hour == 0xFF)
    {
        skip |= DATETIME_SKIPS_HOUR;
		hour = 1;
    }
    if (minute == -1 || minute == 0xFF)
    {
        skip |= DATETIME_SKIPS_MINUTE;
		minute = 0;
    }
    if (second == -1 || second == 0xFF)
    {
        skip |= DATETIME_SKIPS_SECOND;
		second = 0;
    }
    if (millisecond < 1 || millisecond == 0xFF)
    {
        skip |= DATETIME_SKIPS_MS;
		millisecond = 0;
    }
	m_Skip = (DATETIME_SKIPS) skip;
	if (year != 0)
	{
		m_Value.tm_year = year - 1900;
	}
	m_Value.tm_mon = month;
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

unsigned char CGXDateTime::DaysInMonth(int year, short month)
{
    if(month == 0 || month == 2 || month == 4 || 
		month == 6 || month == 7 || month == 9 || month == 11)
	{
        return 31;
	}
    else if(month == 3 || month == 5 || month == 8 || month == 10)
	{
        return 30;
	}
    if(year % 4 == 0) 
	{
        if(year % 100 == 0) 
		{
            if(year % 400 == 0)
			{
                return 29;
			}
            return 28;
        }
        return 29;
    }
    return 28;
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

/** 
 Daylight savings begin.
*/    
bool CGXDateTime::GetDaylightSavingsBegin()
{
    return m_DaylightSavingsBegin;
}
void CGXDateTime::SetDaylightSavingsBegin(bool value)
{
    m_DaylightSavingsBegin = value;
}

/** 
 Daylight savings end.
*/
bool CGXDateTime::GetDaylightSavingsEnd()
{
    return m_DaylightSavingsEnd;
}
void CGXDateTime::SetDaylightSavingsEnd(bool value)
{
    m_DaylightSavingsEnd = value;
}

basic_string<char> CGXDateTime::ToString()
{
	char buff[50];
	if (m_Skip != DATETIME_SKIPS_NONE)
    {
		string str;
		//Add year, month and date if used.
		if ((m_Skip & (DATETIME_SKIPS_YEAR | DATETIME_SKIPS_MONTH | DATETIME_SKIPS_DAY)) != (DATETIME_SKIPS_YEAR | DATETIME_SKIPS_MONTH | DATETIME_SKIPS_DAY))
		{
			int ret = strftime(buff, 50, "%x", &m_Value);
			string year, month, day;
			string date;
			date.append(buff, ret);
			time_base::dateorder order = std::use_facet<std::time_get<char> >(std::locale("")).date_order();
			char separator = 0;
			switch(order)
			{
				case time_base::dmy:
				case time_base::mdy:
				{
					separator = date[date.size() - 5];
				}
				break;		
				case time_base::ymd:
				case time_base::ydm:
				{
					separator = date[4];
				}
				break;
				default:
					separator = 0;
				break;
			}
			//If separator not found.
			if (separator == 0)
			{
				ret = strftime(buff, 50, "%X", &m_Value);
				date.append(" ", 1);
				date.append(buff, ret);
				return date;
			}
			vector< basic_string<char> > strs = GXHelpers::Split(date, separator);			
			switch(order)
			{
				case time_base::dmy:
				{
					if (m_Value.tm_mday != -1 && (m_Skip & DATETIME_SKIPS_DAY) == 0)
					{					
						str = strs[0];
					}
					if (m_Value.tm_mon != -1 && (m_Skip & DATETIME_SKIPS_MONTH) == 0)
					{
						if (str.size() != 0)
						{
							str.append(&separator, 1);
						}
						str.append(strs[1]);
					}				
					if (m_Value.tm_hour != -1 && (m_Skip & DATETIME_SKIPS_YEAR) == 0)
					{			
						if (str.size() != 0)
						{
							str.append(&separator, 1);
						}
						str.append(strs[2]);
					}			
				}
				break;
				case time_base::mdy:
				{
					if (m_Value.tm_mday != -1 && (m_Skip & DATETIME_SKIPS_MONTH) == 0)
					{					
						str = strs[0];
					}
					if (m_Value.tm_mon != -1 && (m_Skip & DATETIME_SKIPS_DAY) == 0)
					{
						if (str.size() != 0)
						{
							str.append(&separator, 1);
						}
						str.append(strs[1]);
					}				
					if (m_Value.tm_hour != -1 && (m_Skip & DATETIME_SKIPS_YEAR) == 0)
					{			
						if (str.size() != 0)
						{
							str.append(&separator, 1);
						}
						str.append(strs[2]);
					}
				}
				break;
				case time_base::ymd:
				{
					if (m_Value.tm_mday != -1 && (m_Skip & DATETIME_SKIPS_YEAR) == 0)
					{					
						str = strs[0];
					}
					if (m_Value.tm_mon != -1 && (m_Skip & DATETIME_SKIPS_MONTH) == 0)
					{
						if (str.size() != 0)
						{
							str.append(&separator, 1);
						}
						str.append(strs[1]);
					}				
					if (m_Value.tm_hour != -1 && (m_Skip & DATETIME_SKIPS_DAY) == 0)
					{			
						if (str.size() != 0)
						{
							str.append(&separator, 1);
						}
						str.append(strs[2]);
					}			
				}
				break;
				case time_base::ydm:
				{
					if (m_Value.tm_mday != -1 && (m_Skip & DATETIME_SKIPS_YEAR) == 0)
					{					
						str = strs[0];
					}
					if (m_Value.tm_mon != -1 && (m_Skip & DATETIME_SKIPS_DAY) == 0)
					{
						if (str.size() != 0)
						{
							str.append(&separator, 1);
						}
						str.append(strs[1]);
					}				
					if (m_Value.tm_hour != -1 && (m_Skip & DATETIME_SKIPS_MONTH) == 0)
					{			
						if (str.size() != 0)
						{
							str.append(&separator, 1);
						}
						str.append(strs[2]);
					}			
				}
				break;
				default:
				{
					ret = strftime(buff, 50, "%X", &m_Value);
					date.append(" ", 1);
					date.append(buff, ret);
					return date;
				}
			}
		}

		string tmp;
		//Add hours.
        if (m_Value.tm_hour != -1 && (m_Skip & DATETIME_SKIPS_HOUR) == 0)
        {			
            tmp = "%H";
        }
		//Add minutes.
        if (m_Value.tm_min != -1 && (m_Skip & DATETIME_SKIPS_MINUTE) == 0)
        {
			if (tmp.size() != 0)
			{
				tmp.append(":");
			}
            tmp += "%M";
        }
		//Add seconds.
        if (m_Value.tm_sec != -1 && (m_Skip & DATETIME_SKIPS_SECOND) == 0)
        {
			if (tmp.size() != 0)
			{
				tmp.append(":");
			}
            tmp += "%S";
        }		
		if (!tmp.empty())
		{
			int ret = strftime(buff, 50, tmp.c_str(), &m_Value);
			if (ret > 0)
			{
				str.append(" ");
				if (buff[0] == '0')
				{				
					str.append(buff, 1, ret - 1);
				}
				else
				{
					str.append(buff, ret);
				}
			}
		}
		return str;
    }		
	//If value is not set return empty string.
	if (m_Value.tm_year == -1)
	{
		return "";
	}
	strftime(buff, 50, "%x %X", &m_Value);
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

// Status of the clock.
GXDLMS_CLOCK_STATUS CGXDateTime::GetStatus()
{
	return m_Status;
}

void CGXDateTime::SetStatus(GXDLMS_CLOCK_STATUS value)
{
	m_Status = value;
}
