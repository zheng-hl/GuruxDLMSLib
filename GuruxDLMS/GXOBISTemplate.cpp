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
#include <assert.h>
#include "GXOBISTemplate.h"
#include "GXHelpers.h"

int CGXOBISTemplate::GetObjectCount(unsigned char* pBuff, int &pos)
{
	int cnt = pBuff[pos++] & 0xFF;
	if (cnt > 0x80)
	{
		long tmp = cnt;
		cnt = 0;
		for(int pos1 = tmp - 0x81; pos1 > -1; --pos1)
		{
			int val = pBuff[pos++] & 0xFF;
			val = val << (8 * pos1);
			cnt += val;
		}
	}
	return cnt;
}
// Set count of items.
void CGXOBISTemplate::SetObjectCount(unsigned int Count, std::vector<unsigned char>& buff)
{
    if (Count > 0x7F)
    {
        unsigned int cnt = 0;
        while ((unsigned char) (Count >> (8 * ++cnt)) > 0);                
		buff.push_back((0x80 + cnt));
		unsigned char* p = (unsigned char*) &cnt;
		GXHelpers::AddRange(buff, p + 4 - cnt, cnt);		
    }
    else
    {
        buff.push_back(Count);
    }
}

int CGXOBISTemplate::GetDataType(unsigned char*& pBuff, int& size, DLMS_DATA_TYPE& Type)
{
	if (size == 0)
	{
		return ERROR_CODES_OUTOFMEMORY;
	}
	Type = (DLMS_DATA_TYPE) *pBuff++;
	--size;
	if (Type == DLMS_DATA_TYPE_NONE)
	{
		//Do nothing.
		return ERROR_CODES_OK;
	}
	if (size == 0)
	{
		return ERROR_CODES_OUTOFMEMORY;
	}
	return ERROR_CODES_OK;
}

char CGXOBISTemplate::GetInt8(unsigned char* pBuff)
{
	char value;
	GXHelpers::ChangeByteOrder(&value, pBuff, 1);		
	return value;
}

short CGXOBISTemplate::GetInt16(unsigned char* pBuff)
{
	short value;
	GXHelpers::ChangeByteOrder(&value, pBuff, 2);
	return value;
}

int CGXOBISTemplate::GetInt32(unsigned char* pBuff)
{
	int value;
	GXHelpers::ChangeByteOrder(&value, pBuff, 4);
	return value;
}

long long CGXOBISTemplate::GetInt64(unsigned char* pBuff)
{
	long long value;
	GXHelpers::ChangeByteOrder(&value, pBuff, 8);
	return value;
}

unsigned char CGXOBISTemplate::GetUInt8(unsigned char* pBuff)
{
	unsigned char value;
	GXHelpers::ChangeByteOrder(&value, pBuff, 1);
	return value;
}

unsigned short CGXOBISTemplate::GetUInt16(unsigned char* pBuff)
{
	unsigned short value;
	GXHelpers::ChangeByteOrder(&value, pBuff, 2);
	return value;
}

unsigned int CGXOBISTemplate::GetUInt32(unsigned char* pBuff)
{
	unsigned int value;
	GXHelpers::ChangeByteOrder(&value, pBuff, 4);
	return value;
}

unsigned long long CGXOBISTemplate::GetUInt64(unsigned char* pBuff)
{
	unsigned long long value;
	GXHelpers::ChangeByteOrder(&value, pBuff, 8);
	return value;
}

void CGXOBISTemplate::GetOctetString(unsigned char* pBuff, unsigned char* value, int len)
{	
	memcpy(value, pBuff, len);
	pBuff += len;
}

int CGXOBISTemplate::GetDateTime(unsigned char* pBuff, int buffLen, char* pValue)
{	
	if (buffLen != 12) //If there is not enough data available.
	{		
		return ERROR_CODES_OUTOFMEMORY;
	}	
	int month, day, hour, minute, second, year = 0;
	//Get year.
	GXHelpers::ChangeByteOrder(&year, pBuff, 2);
	pBuff += 2;
	//Get month
	month = *pBuff++;
	//Get day
	day = *pBuff++;
	//Skip week day
	pBuff++;
	//Get time.				
	hour = *pBuff++;
	minute = *pBuff++;
	second = *pBuff++;
	pBuff++;
#if _MSC_VER > 1000
	sprintf_s(pValue, buffLen, "%d/%d/%d %d:%d:%d", month, day, year, hour, minute, second);
#else
	sprintf(pValue, "%d/%d/%d %d:%d:%d", month, day, year, hour, minute, second);
#endif	
	return ERROR_CODES_OK;
}

int CGXOBISTemplate::SetData(std::vector<unsigned char>& buff, DLMS_DATA_TYPE type, CGXDLMSVariant value)
{
	if (value.vt == DLMS_DATA_TYPE_ARRAY && value.byteArr.size() != 0)
	{
		buff.insert(buff.end(), reinterpret_cast<const unsigned char*>(&value.byteArr[0]), 
			        reinterpret_cast<const unsigned char*>(&value.byteArr[0] + value.byteArr.size()));
		return ERROR_CODES_OK;
	}
	if (type == DLMS_DATA_TYPE_OCTET_STRING && value.vt == DLMS_DATA_TYPE_DATETIME)
    {
        type = DLMS_DATA_TYPE_DATETIME;
    }
    if (type == DLMS_DATA_TYPE_DATETIME)
    {
        buff.push_back(DLMS_DATA_TYPE_OCTET_STRING);
    }
	else
	{
		buff.push_back(type);
	}
	//Do not change byte order.
	if (type == DLMS_DATA_TYPE_STRING)
	{
		buff.push_back(value.strVal.size());
		buff.insert(buff.end(), reinterpret_cast<const unsigned char*>(&value.strVal[0]), 
			        reinterpret_cast<const unsigned char*>(&value.strVal[0] + value.strVal.size()));
	}	
	else if (type == DLMS_DATA_TYPE_OCTET_STRING)
	{
		if (value.vt == DLMS_DATA_TYPE_STRING)
        {			
            std::vector< basic_string<char> > items = GXHelpers::Split(value.strVal, '.');
			if (items.size() == 1 && strcmp(items[0].c_str(), value.strVal.c_str()) == 0)
			{
				SetObjectCount(value.strVal.size(), buff);
				buff.insert(buff.end(), reinterpret_cast<const unsigned char*>(&value.strVal[0]), 
			        reinterpret_cast<const unsigned char*>(&value.strVal[0] + value.strVal.size()));
			}
			else
			{
				SetObjectCount(items.size(), buff);
				for(std::vector< basic_string<char> >::iterator it = items.begin(); it != items.end(); ++it)
				{
					buff.push_back((const char) atol((*it).c_str()));
				}
			}
        }
		else if (value.byteArr.size() != 0)
        {
            SetObjectCount(value.byteArr.size(), buff);
			buff.insert(buff.end(), reinterpret_cast<const unsigned char*>(&value.byteArr[0]), 
			        reinterpret_cast<const unsigned char*>(&value.byteArr[0] + value.byteArr.size()));
        }
        else if (value.vt == DLMS_DATA_TYPE_NONE)
        {
            SetObjectCount(0, buff);
        }
        else
        {
             return ERROR_CODES_INVALID_PARAMETER;
        }
	}	
	else if (type == DLMS_DATA_TYPE_STRUCTURE || type == DLMS_DATA_TYPE_ARRAY)
	{
		buff.push_back(value.Arr.size());
		for(vector<CGXDLMSVariant>::iterator it = value.Arr.begin(); it != value.Arr.end(); ++it)
		{
			buff.push_back(it->vt);
			it->GetBytes(buff);
		}
	}
	else
	{
		int ret = value.ChangeType(type);
		if (ret != ERROR_CODES_OK)
		{
			return ret;
		}
		value.GetBytes(buff);
	}	
	return ERROR_CODES_OK;
}

void CGXOBISTemplate::GetLogicalName(vector<unsigned char> data, string& ln)
{
	if (data.size() == 0)
	{
		ln.clear();
	}
	else
	{
		GetLogicalName(&data[0], ln);

	}
}

void CGXOBISTemplate::GetLogicalName(unsigned char* buff, string& ln)
{
	int dataSize;
	char tmp[25];
#if _MSC_VER > 1000
	dataSize = sprintf_s(tmp, 25, "%d.%d.%d.%d.%d.%d", buff[0], buff[1], buff[2], buff[3], buff[4], buff[5]) + 1;
#else
	dataSize = sprintf(tmp, "%d.%d.%d.%d.%d.%d", buff[0], buff[1], buff[2], buff[3], buff[4], buff[5]) + 1;
#endif		
	if (dataSize > 25)
	{	
		assert(0);			
	}
	ln.clear();
	ln.append(tmp, dataSize);	
}


/*
int CGXOBISTemplate::SetData(std::vector<unsigned char>& buff, DLMS_DATA_TYPE Type, void* Data, int DataLen)
{
	buff.push_back(Type);
	//Do not change byte order.
	if (Type == DLMS_DATA_TYPE_OCTET_STRING || Type == DLMS_DATA_TYPE_STRING)
	{
		buff.push_back(DataLen);
		for(int pos = 0; pos != DataLen; ++pos)
		{
			buff.push_back(((unsigned char*) Data)[pos]);
		}		
	}	
	else
	{
		GXHelpers::ChangeByteOrder(buff, Data, DataLen);
	}	
	return 0;
}
*/
int CGXOBISTemplate::GetData(unsigned char*& pBuff, int& size, DLMS_DATA_TYPE Type, CGXDLMSVariant& value, int* pTotalCnt, int* pReadCnt, int* CachePosition)
{
	int dataSize = 0; 
	value.Clear();
	if (size == 0)
	{
		return ERROR_CODES_OUTOFMEMORY;
	}
	bool knownType = Type != DLMS_DATA_TYPE_NONE;
    if (!knownType)
    {
		Type = (DLMS_DATA_TYPE) *pBuff++;
		--size;
	}
	if (Type == DLMS_DATA_TYPE_NONE)
	{
		//Do nothing.
		return ERROR_CODES_OK;
	}
	if (size == 0)
	{
		return ERROR_CODES_OUTOFMEMORY;
	}		
	if (Type == DLMS_DATA_TYPE_ARRAY ||
			Type == DLMS_DATA_TYPE_STRUCTURE)
	{
		int pos = 0;
		int cnt = CGXOBISTemplate::GetObjectCount(pBuff, pos);		
		pBuff += pos;
		size -= pos;
		int ret;
		value.vt = Type;
		for(int a = 0; a != cnt; ++a)
		{			
			CGXDLMSVariant tmp;
			if ((ret = GetData(pBuff, size, DLMS_DATA_TYPE_NONE, tmp, pTotalCnt, pReadCnt, CachePosition)) != ERROR_CODES_OK)
			{
				value.Clear();
				return ret;
			}
			value.Arr.push_back(tmp);
		}		
		return ERROR_CODES_OK;
	}	
	else if (Type == DLMS_DATA_TYPE_BOOLEAN)
	{		
		dataSize = sizeof(unsigned char);
	}
	else if (Type == DLMS_DATA_TYPE_BIT_STRING)
	{	
		int cnt = *pBuff++;
		--size;
		double tmp = cnt;
		tmp /= 8;
		if (cnt % 8 != 0)
		{
			++tmp;
		}
		dataSize = (int) tmp;
		return ERROR_CODES_OK;		
	}
	else if (Type == DLMS_DATA_TYPE_INT32)
	{
		dataSize = 4;
	}
	else if (Type == DLMS_DATA_TYPE_UINT32)
	{
		dataSize = 4;
	}
	else if (Type == DLMS_DATA_TYPE_STRING)
	{
		int pos = 0;
		if (knownType)
		{
			dataSize = size;
		}
		else
		{		
			dataSize = CGXOBISTemplate::GetObjectCount(pBuff, pos);
		}
		pBuff += pos;
		size -= pos;
		if (size < dataSize) //If there is not enough data available.
		{
			size = 0;
			return ERROR_CODES_OUTOFMEMORY;
		}
	}
	//Excample Logical name is octet string, so do not change to string...
	else if (Type == DLMS_DATA_TYPE_OCTET_STRING)
	{
		int pos = 0;
		if (knownType)
		{
			dataSize = size;
		}
		else
		{	
			dataSize = CGXOBISTemplate::GetObjectCount(pBuff, pos);
		}
		pBuff += pos;
		size -= pos;
		if (size < dataSize) //If there is not enough data available.
		{
			size = 0;
			return ERROR_CODES_OUTOFMEMORY;
		}
	}
	else if (Type == DLMS_DATA_TYPE_BINARY_CODED_DESIMAL)
	{
		assert(0);
	}
	else if (Type == DLMS_DATA_TYPE_INT8)
	{
		dataSize = sizeof(unsigned char);
	}
	else if (Type == DLMS_DATA_TYPE_INT16)
	{
		dataSize = 2;
	}
	else if (Type == DLMS_DATA_TYPE_UINT8)
	{
		dataSize = 1;
	}
	else if (Type == DLMS_DATA_TYPE_UINT16)
	{
		dataSize = 2;
	}
	else if (Type == DLMS_DATA_TYPE_COMPACT_ARRAY)
	{
		assert(0);
	}
	else if (Type == DLMS_DATA_TYPE_INT64)
	{
		dataSize = 8;
	}
	else if (Type == DLMS_DATA_TYPE_UINT64)
	{
		dataSize = 8;
	}
	else if (Type == DLMS_DATA_TYPE_ENUM)
	{
		dataSize = 1;
	}
	else if (Type == DLMS_DATA_TYPE_FLOAT32)
	{
		dataSize = 4;
	}
	else if (Type == DLMS_DATA_TYPE_FLOAT64)
	{
		dataSize = 8;
	}
	else if (Type == DLMS_DATA_TYPE_DATETIME)
	{
		dataSize = 12;
	}
	else if (Type == DLMS_DATA_TYPE_DATE)
	{
		dataSize = 5;
	}
	else if (Type == DLMS_DATA_TYPE_TIME)
	{
		dataSize = 7;
	}
	else
	{
		assert(0);
	}
	if (size < dataSize) //If there is not enough data available.
	{
		size = 0;
		return ERROR_CODES_OUTOFMEMORY;
	}	
	
	if (Type == DLMS_DATA_TYPE_STRING)
	{
		value.strVal.append((const char*) pBuff, dataSize);
	}
	//Excample Logical name is octet string, so do not change to string...
	else if (Type == DLMS_DATA_TYPE_OCTET_STRING)
	{
		GXHelpers::AddRange(value.byteArr, pBuff, dataSize);
	}
	else
	{
		GXHelpers::ChangeByteOrder(&value.bVal, pBuff, dataSize);
	}
	value.vt = Type;
	pBuff += dataSize;	
	size -= dataSize;
	return ERROR_CODES_OK;
}

bool CGXOBISTemplate::IsLogicalNameEmpty(unsigned char* pLN)
{
	const unsigned char EmptyLN[] = {0, 0, 0, 0, 0, 0};
	return memcmp(pLN, EmptyLN, 6) == 0;
}

const char* CGXOBISTemplate::GetUnitAsString(int unit)
{
	switch(unit)
	{
		case 1:
			return "Year";
		break;
		case 2:
			return "Month";
		break;
		case 3:
			return "Week";
		break;
		case 4:
			return "Day";
		break;
		case 5:
			return "Hour";
		break;
		case 6:
			return "Minute";
		break;
		case 7:
			return "Second";
		break;
		case 8:
			return "Phase angle degree rad*180/p";
		break;
		case 9:
			return "Temperature T degree centigrade";
		break;
		case 10:		
			return "Local currency";
		break;
		case 11:
			return "Length l meter m";		
		break;
		case 12:
			return "Speed v m/s";		
		break;
		case 13:
			return "Volume V m3";		
		break;
		case 14:
			return "Corrected volume m3";		
		break;
		case 15:
			return "Volume flux m3/60*60s";		
		break;
		case 16:
			return "Corrected volume flux m3/60*60s";		
		break;
		case 17:
			return "Volume flux m3/24*60*60s";		
		break;
		case 18:
			return "Corrected volume flux m3/24*60*60s";		
		break;
		case 19:
			return "Volume 10-3 m3";		
		break;
		case 20:
			return "Mass m kilogram kg";		
		break;
		case 21:
			return "Force F newton N";		
		break;
		case 22:
			return "Energy newtonmeter J = Nm = Ws";		
		break;
		case 23:
			return "Pressure p pascal N/m2";		
		break;
		case 24:
			return "Pressure p bar 10-5 N/m2";		
		break;
		case 25:
			return "Energy joule J = Nm = Ws";		
		break;
		case 26:
			return "Thermal power J/60*60s";		
		break;
		case 27:
			return "Active power P watt W = J/s";		
		break;
		case 28:
			return "Apparent power S";		
		break;
		case 29:
			return "Reactive power Q";		
		break;
		case 30:
			return "Active energy W*60*60s";		
		break;
		case 31:
			return "Apparent energy VA*60*60s";		
		break;
		case 32:
			return "Reactive energy var*60*60s";			
		break;
		case 33:
			return "Current I ampere A";			
		break;
		case 34:
			return "Electrical charge Q coulomb C = As";			
		break;
		case 35:
			return "Voltage";			
		break;
		case 36:
			return "Electrical field strength E V/m";
		break;
		case 37:
			return "Capacity C farad C/V = As/V";
		break;
		case 38:
			return "Resistance R ohm = V/A";			
		break;
		case 39:
			return "Resistivity";			
		break;
		case 40:
			return "Magnetic flux F weber Wb = Vs";
		break;
		case 41:
			return "Induction T tesla Wb/m2";			
		break;
		case 42:
			return "Magnetic field strength H A/m";			
		break;
		case 43:
			return "Inductivity L henry H = Wb/A";			
		break;
		case 44:
			return "Frequency f";			
		break;
		case 45:
			return "Active energy meter constant 1/Wh";			
		break;
		case 46:
			return "Reactive energy meter constant";			
		break;
		case 47:
			return "Apparent energy meter constant";			
		break;
		case 48:
			return "V260*60s";			
		break;
		case 49:
			return "A260*60s";			
		break;
		case 50:
			return "Mass flux kg/s";			
		break;
		case 51:
			return "Conductance siemens 1/ohm";			
		break;
		case 254 :
			return "Other Unit";		
		break;
		case 255:
			return "No Unit";		
		break;
	}
	return "";
}
