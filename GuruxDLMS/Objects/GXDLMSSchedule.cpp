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

#include "../GXDLMSVariant.h"
#include "../GXDLMSClient.h"
#include "GXDLMSSchedule.h"

//Constructor.
CGXDLMSSchedule::CGXDLMSSchedule() : CGXDLMSObject(OBJECT_TYPE_SCHEDULE)
{
}

//SN Constructor.
CGXDLMSSchedule::CGXDLMSSchedule(unsigned short sn) : CGXDLMSObject(OBJECT_TYPE_SCHEDULE, sn)
{

}

//LN Constructor.
CGXDLMSSchedule::CGXDLMSSchedule(basic_string<char> ln) : CGXDLMSObject(OBJECT_TYPE_SCHEDULE, ln)
{

}

// Get value of COSEM Data object.
vector<CGXDLMSScheduleEntry>& CGXDLMSSchedule::GetEntries()
{
    return m_Entries;
}

// Set value of COSEM Data object.
void CGXDLMSSchedule::SetEntries(vector<CGXDLMSScheduleEntry>& value)
{
    m_Entries = value;
}

// Returns amount of attributes.
int CGXDLMSSchedule::GetAttributeCount()
{
	return 2;
}

// Returns amount of methods.
int CGXDLMSSchedule::GetMethodCount()
{
	return 3;
}

void CGXDLMSSchedule::GetValues(vector<string>& values)
{
	values.clear();
	string ln;
	GetLogicalName(ln);
	values.push_back(ln);
	//TODO: values.push_back(m_Entries.ToString());
}

void CGXDLMSSchedule::GetAttributeIndexToRead(vector<int>& attributes)
{
	//LN is static and read only once.
	if (CGXOBISTemplate::IsLogicalNameEmpty(m_LN))
    {
        attributes.push_back(1);
    }
	//Entries
    if (CanRead(2))
    {
        attributes.push_back(2);
    }
}

int CGXDLMSSchedule::GetDataType(int index, DLMS_DATA_TYPE& type)
{
	if (index == 1)
	{
		type = DLMS_DATA_TYPE_OCTET_STRING;
		return ERROR_CODES_OK;
	}
    if (index == 2)
	{			
		type = DLMS_DATA_TYPE_ARRAY;
		return ERROR_CODES_OK;
	}
	return ERROR_CODES_INVALID_PARAMETER;
}

// Returns value of given attribute.
int CGXDLMSSchedule::GetValue(int index, int selector, CGXDLMSVariant& parameters, CGXDLMSVariant& value)
{
	if (index == 1)
	{
		GXHelpers::AddRange(value.byteArr, m_LN, 6);
		value.vt = DLMS_DATA_TYPE_OCTET_STRING;
		return ERROR_CODES_OK;
	}        
	//TODO:
	return ERROR_CODES_INVALID_PARAMETER;
}

// Set value of given attribute.
int CGXDLMSSchedule::SetValue(int index, CGXDLMSVariant& value)
{
	if (index == 1)
	{			
		if (value.vt != DLMS_DATA_TYPE_OCTET_STRING || value.GetSize() != 6)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}
		memcpy(m_LN, &value.byteArr[0], 6);		
	}
    else if (index == 2)
	{
        m_Entries.clear();
        CGXDLMSVariant tmp;
		for (std::vector<CGXDLMSVariant >::iterator it = value.Arr.begin(); it != value.Arr.end(); ++it)                
        {     
            CGXDLMSScheduleEntry item;
            item.SetIndex((*it).Arr[0].ToInteger());
            item.SetEnable((*it).Arr[1].boolVal);
            CGXDLMSClient::ChangeType((*it).Arr[2].byteArr, DLMS_DATA_TYPE_OCTET_STRING, tmp);
            item.SetLogicalName(tmp.ToString());
            item.SetScriptSelector((*it).Arr[3].ToInteger());
            CGXDLMSClient::ChangeType((*it).Arr[4].byteArr, DLMS_DATA_TYPE_DATETIME, tmp);
            item.SetSwitchTime(tmp.dateTime);
            item.SetValidityWindow((*it).Arr[5].ToInteger());
            item.SetExecWeekdays((*it).Arr[6].strVal);
            item.SetExecSpecDays((*it).Arr[7].strVal);
            CGXDLMSClient::ChangeType((*it).Arr[8].byteArr, DLMS_DATA_TYPE_DATETIME, tmp);
            item.SetBeginDate(tmp.dateTime);
            CGXDLMSClient::ChangeType((*it).Arr[9].byteArr, DLMS_DATA_TYPE_DATETIME, tmp);
            item.SetEndDate(tmp.dateTime);
            m_Entries.push_back(item);
        }
	}	
	else
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}
	return ERROR_CODES_OK;
}