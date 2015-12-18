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

#include "GXDLMSLimiter.h"
#include "../GXDLMSClient.h"
#include "GXDLMSActionSet.h"
#include <sstream> 

//Constructor.
CGXDLMSLimiter::CGXDLMSLimiter() : CGXDLMSObject(OBJECT_TYPE_LIMITER)
{
}

//SN Constructor.
CGXDLMSLimiter::CGXDLMSLimiter(unsigned short sn) : CGXDLMSObject(OBJECT_TYPE_LIMITER, sn)
{

}

//LN Constructor.
CGXDLMSLimiter::CGXDLMSLimiter(basic_string<char> ln) : CGXDLMSObject(OBJECT_TYPE_LIMITER, ln)
{

}


/** 
Defines an attribute of an object to be monitored.
*/
CGXDLMSObject* CGXDLMSLimiter::GetMonitoredValue()
{
    return m_MonitoredValue;
}
void CGXDLMSLimiter::SetMonitoredValue(CGXDLMSObject* value)
{
    m_MonitoredValue = value;
}

/** 
 Provides the active threshold value to which the attribute monitored is compared.
*/
CGXDLMSVariant& CGXDLMSLimiter::GetThresholdActive()
{
    return m_ThresholdActive;
}
void CGXDLMSLimiter::SetThresholdActive(CGXDLMSVariant value)
{
    m_ThresholdActive = value;
}

/** 
 Provides the threshold value to which the attribute monitored 
 is compared when in normal operation.
*/
CGXDLMSVariant& CGXDLMSLimiter::GetThresholdNormal()
{
    return m_ThresholdNormal;
}
void CGXDLMSLimiter::SetThresholdNormal(CGXDLMSVariant value)
{
    m_ThresholdNormal = value;
}

/** 
 Provides the threshold value to which the attribute monitored
 is compared when an emergency profile is active.
*/
CGXDLMSVariant& CGXDLMSLimiter::GetThresholdEmergency()
{
    return m_ThresholdEmergency;
}
void CGXDLMSLimiter::SetThresholdEmergency(CGXDLMSVariant value)
{
    m_ThresholdEmergency = value;
}

/** 
 Defines minimal over threshold duration in seconds required
 to execute the over threshold action.
*/
long CGXDLMSLimiter::GetMinOverThresholdDuration()
{
    return m_MinOverThresholdDuration;
}
void CGXDLMSLimiter::SetMinOverThresholdDuration(long value)
{
    m_MinOverThresholdDuration = value;
}

/** 
 Defines minimal under threshold duration in seconds required to
 execute the under threshold action.
*/
long CGXDLMSLimiter::GetMinUnderThresholdDuration()
{
    return m_MinUnderThresholdDuration;
}
void CGXDLMSLimiter::SetMinUnderThresholdDuration(long value)
{
    m_MinUnderThresholdDuration = value;
}
CGXDLMSEmergencyProfile& CGXDLMSLimiter::GetEmergencyProfile()
{
    return m_EmergencyProfile;
}
void CGXDLMSLimiter::SetEmergencyProfile(CGXDLMSEmergencyProfile value)
{
    m_EmergencyProfile = value;
}

vector<int>& CGXDLMSLimiter::GetEmergencyProfileGroupIDs()
{
    return m_EmergencyProfileGroupIDs;
}
void CGXDLMSLimiter::SetEmergencyProfileGroupIDs(vector<int> value)
{
    m_EmergencyProfileGroupIDs = value;
}

/** 
 Is Emergency Profile active.
*/
bool CGXDLMSLimiter::GetEmergencyProfileActive()
{
    return m_EmergencyProfileActive;
}
void CGXDLMSLimiter::SetEmergencyProfileActive(bool value)
{
    m_EmergencyProfileActive = value;
}

/** 
 Defines the scripts to be executed when the monitored value
 crosses the threshold for minimal duration time.
*/
CGXDLMSActionItem& CGXDLMSLimiter::GetActionOverThreshold()
{
    return m_ActionOverThreshold;
}
void CGXDLMSLimiter::SetActionOverThreshold(CGXDLMSActionItem value)
{
    m_ActionOverThreshold = value;
}

/** 
 Defines the scripts to be executed when the monitored value
 crosses the threshold for minimal duration time.
*/
CGXDLMSActionItem& CGXDLMSLimiter::GetActionUnderThreshold()
{
    return m_ActionUnderThreshold;
}
void CGXDLMSLimiter::SetActionUnderThreshold(CGXDLMSActionItem value)
{
    m_ActionUnderThreshold = value;
}

// Returns amount of attributes.
int CGXDLMSLimiter::GetAttributeCount()
{
	return 11;
}

// Returns amount of methods.
int CGXDLMSLimiter::GetMethodCount()
{
	return 0;
}

void CGXDLMSLimiter::GetValues(vector<string>& values)
{
	values.clear();
	string ln;
	GetLogicalName(ln);
	values.push_back(ln);
	if (m_MonitoredValue != NULL)
	{
		values.push_back(m_MonitoredValue->GetName().ToString());
	}
	else
	{
		values.push_back("");
	}
	values.push_back(m_ThresholdActive.ToString());
	values.push_back(m_ThresholdNormal.ToString());
	values.push_back(m_ThresholdEmergency.ToString());
	values.push_back(CGXDLMSVariant(m_MinOverThresholdDuration).ToString());
	values.push_back(CGXDLMSVariant(m_MinUnderThresholdDuration).ToString());
	values.push_back(m_EmergencyProfile.ToString());
	std::stringstream sb;
	sb << '[';
	bool empty = true;
	for(vector<int>::iterator it = m_EmergencyProfileGroupIDs.begin(); it != m_EmergencyProfileGroupIDs.end(); ++it)
	{
		if (!empty)
		{
			sb << ", ";
		}
		empty = false;
		string str = CGXDLMSVariant(*it).ToString();
		sb.write(str.c_str(), str.size());
	}
	sb << ']';
	values.push_back(sb.str());		
	values.push_back(CGXDLMSVariant(m_EmergencyProfileActive).ToString());
	
	sb.str(std::string());
	sb << m_ActionOverThreshold.ToString().c_str();
	sb << ", ";
	sb << m_ActionUnderThreshold.ToString().c_str();
	values.push_back(sb.str());		
}

void CGXDLMSLimiter::GetAttributeIndexToRead(vector<int>& attributes)
{
	//LN is static and read only once.
	if (CGXOBISTemplate::IsLogicalNameEmpty(m_LN))
    {
        attributes.push_back(1);
    }
	//MonitoredValue
    if (CanRead(2))
    {
        attributes.push_back(2);
    }

    //ThresholdActive
    if (CanRead(3))
    {
        attributes.push_back(3);
    }

    //ThresholdNormal
    if (CanRead(4))
    {
        attributes.push_back(4);
    }

    //ThresholdEmergency
    if (CanRead(5))
    {
        attributes.push_back(5);
    }

    //MinOverThresholdDuration
    if (CanRead(6))
    {
        attributes.push_back(6);
    }

    //MinUnderThresholdDuration
    if (CanRead(7))
    {
        attributes.push_back(7);
    }

    //EmergencyProfile
    if (CanRead(8))
    {
        attributes.push_back(8);
    }
    //EmergencyProfileGroup
    if (CanRead(9))
    {
        attributes.push_back(9);
    }

    //EmergencyProfileActive
    if (CanRead(10))
    {
        attributes.push_back(10);
    }
    //Actions
    if (CanRead(11))
    {
        attributes.push_back(11);
    }
}

int CGXDLMSLimiter::GetDataType(int index, DLMS_DATA_TYPE& type)
{
	if (index == 1)
	{
		type = DLMS_DATA_TYPE_OCTET_STRING;			
	}
	else if (index == 2)
    {
        type = DLMS_DATA_TYPE_STRUCTURE;
    }
    else if (index == 3)
    {
		CGXDLMSObject::GetDataType(index, type);
    }
    else if (index == 4)
    {
        CGXDLMSObject::GetDataType(index, type);
    }
    else if (index == 5)
    {
        CGXDLMSObject::GetDataType(index, type);
    }
    else if (index == 6)
    {
        type = DLMS_DATA_TYPE_UINT32;
    }
    else if (index == 7)
    {
        type = DLMS_DATA_TYPE_UINT32;
    }
    else if (index == 8)
    {
        type = DLMS_DATA_TYPE_STRUCTURE;
    }
    else if (index == 9)
    {
        type = DLMS_DATA_TYPE_ARRAY;
    }
    else if (index == 10)
    {
        type = DLMS_DATA_TYPE_BOOLEAN;
    }
    else if (index == 11)
    {
        type = DLMS_DATA_TYPE_STRUCTURE;
    }
	else
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}
	return ERROR_CODES_OK;
}

// Returns value of given attribute.
int CGXDLMSLimiter::GetValue(int index, int selector, CGXDLMSVariant& parameters, CGXDLMSVariant& value)
{
	if (index == 1)
	{
		GXHelpers::AddRange(value.byteArr, m_LN, 6);
		value.vt = DLMS_DATA_TYPE_OCTET_STRING;
	}
    else if (index == 2)
    {
		int ret;
        vector<unsigned char> data;
        data.push_back(DLMS_DATA_TYPE_STRUCTURE);
        data.push_back(3);
		string ln;
		m_MonitoredValue->GetLogicalName(ln);
        if ((ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_INT16, m_MonitoredValue->GetObjectType())) != 0 ||
			(ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_OCTET_STRING, ln)) != 0)
		{
			return ret;
		}
        //TODO: CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_UINT8, m_MonitoredValue.GetSelectedAttributeIndex());
        value = data;
    }
    else if (index == 3)
    {
        value = m_ThresholdActive;
		return ERROR_CODES_OK;
    }
    else if (index == 4)
    {
        value = m_ThresholdNormal;
    }
    else if (index == 5)
    {
        value = m_ThresholdEmergency;
    }
    else if (index == 6)
    {
        value = m_MinOverThresholdDuration;
    }
    else if (index == 7)
    {
        value = m_MinUnderThresholdDuration;
    }
    else if (index == 8)
    {
		int ret;
        vector<unsigned char> data;
        data.push_back(DLMS_DATA_TYPE_STRUCTURE);
        data.push_back(3);  
        if ((ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_UINT16, m_EmergencyProfile.GetID())) != 0 ||
			(ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_DATETIME, m_EmergencyProfile.GetActivationTime())) != 0 ||
			(ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_UINT32, m_EmergencyProfile.GetDuration())) != 0)
		{
			return ret;
		}
        value = data;
    }
    else if (index == 9)
    {
		int ret;
        vector<unsigned char> data;
        data.push_back(DLMS_DATA_TYPE_ARRAY);
        data.push_back(m_EmergencyProfileGroupIDs.size());
		for(vector<int>::iterator it = m_EmergencyProfileGroupIDs.begin(); it != m_EmergencyProfileGroupIDs.end(); ++it)
        {
            if ((ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_UINT16, *it)) != 0)
			{
				return ret;
			}
        }
		value = data;
    }
    else if (index == 10)
    {
        value = m_EmergencyProfileActive;
    }
    else if (index == 11)
    {
        vector<unsigned char> data;
        data.push_back(DLMS_DATA_TYPE_STRUCTURE);
        data.push_back(2);
        data.push_back(DLMS_DATA_TYPE_STRUCTURE);
        data.push_back(2);
		int ret;
        if ((ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_OCTET_STRING, m_ActionOverThreshold.GetLogicalName())) != 0 ||
			(ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_UINT16, m_ActionOverThreshold.GetScriptSelector())) != 0)
		{
			return ret;
		}
        data.push_back(DLMS_DATA_TYPE_STRUCTURE);
        data.push_back(2);
		if ((ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_OCTET_STRING, m_ActionUnderThreshold.GetLogicalName())) != 0 ||
			(ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_UINT16, m_ActionUnderThreshold.GetScriptSelector())) != 0)
		{
			return ret;
		}
        value = data;
    }
	else
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}
	return ERROR_CODES_OK;
}

// Set value of given attribute.
int CGXDLMSLimiter::SetValue(int index, CGXDLMSVariant& value)
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
		OBJECT_TYPE ot = (OBJECT_TYPE) value.Arr[0].ToInteger();
		CGXDLMSVariant tmp;
		CGXDLMSClient::ChangeType(value.Arr[1].byteArr, DLMS_DATA_TYPE_OCTET_STRING, tmp);
		string ln = tmp.ToString();
		m_SetSelectedAttributeIndex = value.Arr[2].ToInteger();
		m_MonitoredValue = GetParent()->FindByLN(ot, ln);
    }
    else if (index == 3)
    {
        m_ThresholdActive = value;
    }
    else if (index == 4)
    {
        m_ThresholdNormal = value;
    }
    else if (index == 5)
    {
        m_ThresholdEmergency = value;
    }
    else if (index == 6)
    {
        m_MinOverThresholdDuration = value.ToInteger();
    }
    else if (index == 7)
    {
        m_MinUnderThresholdDuration = value.ToInteger();
    }
    else if (index == 8)
    {
        m_EmergencyProfile.SetID(value.Arr[0].ToInteger());
		CGXDLMSVariant tmp;
		CGXDLMSClient::ChangeType(value.Arr[1].byteArr, DLMS_DATA_TYPE_DATETIME, tmp);
		m_EmergencyProfile.SetActivationTime(tmp.dateTime);
        m_EmergencyProfile.SetDuration(value.Arr[2].ToInteger());
    }
    else if (index == 9)
    {     
		m_EmergencyProfileGroupIDs.clear();
		for (vector<CGXDLMSVariant>::iterator it = value.Arr.begin(); it != value.Arr.end(); ++it)
        {
            m_EmergencyProfileGroupIDs.push_back((*it).ToInteger());
        }
    }
    else if (index == 10)
    {
        m_EmergencyProfileActive = value.boolVal;
    }
    else if (index == 11)
    {
		CGXDLMSVariant tmp;
		CGXDLMSClient::ChangeType(value.Arr[0].Arr[0].byteArr, DLMS_DATA_TYPE_OCTET_STRING, tmp);
		m_ActionOverThreshold.SetLogicalName(tmp.ToString());
        m_ActionOverThreshold.SetScriptSelector(value.Arr[0].Arr[1].ToInteger());
		CGXDLMSClient::ChangeType(value.Arr[1].Arr[0].byteArr, DLMS_DATA_TYPE_OCTET_STRING, tmp);
		m_ActionUnderThreshold.SetLogicalName(tmp.ToString());
        m_ActionUnderThreshold.SetScriptSelector(value.Arr[1].Arr[1].ToInteger());
    }	
	else
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}
	return ERROR_CODES_OK;
}
