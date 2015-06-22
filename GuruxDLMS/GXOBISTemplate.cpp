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
#include "GXDateTime.h"

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
			if (value.strVal.length() != 0)
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
        }
		else if (value.byteArr.size() != 0)
        {
            SetObjectCount(value.byteArr.size(), buff);
			buff.insert(buff.end(), reinterpret_cast<const unsigned char*>(&value.byteArr[0]), 
			        reinterpret_cast<const unsigned char*>(&value.byteArr[0] + value.byteArr.size()));
        }
        else if (value.vt == DLMS_DATA_TYPE_NONE || 
			(value.byteArr.size() == 0 && 
			(value.vt == DLMS_DATA_TYPE_ARRAY || value.vt == DLMS_DATA_TYPE_OCTET_STRING)))
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
	else if (type == DLMS_DATA_TYPE_BIT_STRING)
	{
		int index = 0;
		unsigned char val = 0;
		SetObjectCount(value.strVal.size(), buff);
		for(string::iterator it = value.strVal.begin(); it != value.strVal.end(); ++it)
		{
			if (*it == '1')
			{
				val |= (unsigned char)(1 << index++);
			}
			else if (*it == '0')
			{
				index++;
			}
			else 
			{
				return ERROR_CODES_INVALID_PARAMETER;
			}
			if (index == 8)
			{
				index = 0;
				buff.push_back(val);
				val = 0;
			}
		}
		if (index != 0)
		{
			buff.push_back(val);
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
	ln.append(tmp, dataSize - 1);	
}

void CGXOBISTemplate::ToBitString(unsigned char value, int count, string& data)
{
    if (count > 8)
    {
        count = 8;
    }
    for (int pos = count - 1; pos != -1; --pos)
    {
        if ((value & (1 << pos)) != 0)
        {
			data.append("1");
        }
        else
        {
			data.append("0");
        }
    }
}

//Return UTC offset in minutes.
int GetUTCOffset()
{	
    time_t currtime;
    struct tm timeinfo;
    time (&currtime);
#if _MSC_VER > 1000
	gmtime_s(&timeinfo, &currtime);
#else
	timeinfo = *gmtime(&currtime);
#endif    
    time_t utc = mktime(&timeinfo);
#if _MSC_VER > 1000
	localtime_s(&timeinfo, &currtime);
#else
	timeinfo = *localtime(&currtime);
#endif    

    time_t local = mktime(&timeinfo);
    //Offset from UTC in minutes.
    double offset = difftime(local, utc) / 60;
    // Adjust for DST
    if (timeinfo.tm_isdst)
    {
        offset += 60;
    }
    return (int) offset;
}

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
		int byteCnt = (int)tmp;
        if (size < byteCnt) //If there is not enought data available.
        {
			size = 0;
			return ERROR_CODES_OUTOFMEMORY;
        }         
        string str = "";
        while (cnt > 0)
        {
            ToBitString(*(pBuff++), cnt, str);
            cnt -= 8;
			--size;
        }                               
        value = str;
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
	//Example Logical name is octet string, so do not change to string...
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
        //If there is not enought data available.
        if (size < 12)
        {
            size = 0;
			return ERROR_CODES_OUTOFMEMORY;
        }
        //Get year.
        int year = GetUInt16(pBuff);
		pBuff += 2;
        //Get month
        int month = *pBuff++;
        //Get day
        int day = *pBuff++;
        //Skip week day
        pBuff++;
        //Get time.
        int hour = *pBuff++;
        int minute = *pBuff++;
        int second = *pBuff++;
        int ms = *pBuff++;
        if (ms != 0xFF && ms != 0)
        {
            ms *= 10;
        }        
        int devitation = GetInt16(pBuff);                            
		pBuff += 2;
		GXDLMS_CLOCK_STATUS status = (GXDLMS_CLOCK_STATUS) *pBuff++;
		size -= 12;
        CGXDateTime dt(year, month, day, hour, minute, second, ms);
        dt.SetStatus(status);
		//Add deviation if used.
		if ((devitation & 0xFFFF) != 0x8000)
		{
			long offset = GetUTCOffset();
			struct tm val = dt.GetValue();
			val.tm_isdst = (status & GXDLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE) != 0;
			if (val.tm_isdst)
			{
				devitation += 60;
			}
			offset -= devitation;
			val.tm_min += offset;			
			if (mktime(&val) == -1)
			{
				assert(0);
			}		
			dt.SetValue(val);
		}
		else //If deviation is not defined.
		{
			devitation = 0;
			struct tm val = dt.GetValue();
			val.tm_isdst = (status & GXDLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE) != 0;
			if (val.tm_isdst)
			{
				devitation += 60;
			}
			val.tm_min -= devitation;
			if (mktime(&val) == -1)
			{
				assert(0);
			}		
			dt.SetValue(val);			
		}		
        value = dt;
		return ERROR_CODES_OK;
	}
	else if (Type == DLMS_DATA_TYPE_DATE)
	{
		if (knownType)
        {
             //If there is not enought data available.
            if (size < 5)
            {
                size = 0;
				return ERROR_CODES_OUTOFMEMORY;
            }
        }
        else
        {
             //If there is not enough data available.
            ++pBuff; //Get count.
			--size;
            if (size < 5)
            {
                size = 0;
				return ERROR_CODES_OUTOFMEMORY;
            }
        }
        //Get year.
        int year = GetUInt16(pBuff);
		pBuff += 2;
        //Get month
        int month = *pBuff++;
        //Get day
        int day = *pBuff++;
        //Skip week day
        pBuff++;
		CGXDateTime dt(year, month, day, -1, -1, -1, -1);
        dt.SetStatus((GXDLMS_CLOCK_STATUS) *pBuff++);
        value = dt;
	}
	else if (Type == DLMS_DATA_TYPE_TIME)
	{
		if (knownType)
        {
             //If there is not enought data available.
            if (size < 4)
            {
                size = 0;
				return ERROR_CODES_OUTOFMEMORY;
            }
        }
        else
        {
             //If there is not enough data available.
            ++pBuff; //Get count.
			--size;
            if (size < 4)
            {
                size = 0;
				return ERROR_CODES_OUTOFMEMORY;
            }
        }
        //Get time.
        int hour = *pBuff++;
        int minute = *pBuff++;
        int second = *pBuff++;
        int ms = *pBuff++;
        if (ms != 0xFF && ms != 0)
        {
            ms *= 10;
        }        
        CGXDateTime dt(-1, -1, -1, hour, minute, second, ms);
        dt.SetStatus((GXDLMS_CLOCK_STATUS) *pBuff++);
        value = dt;
		return ERROR_CODES_OK;
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
		bool octetString = false;
        if (knownType)
        {
            //Check that this is not octet string.
            for(int a = 0; a != dataSize; ++a)
            {                            
                if (pBuff[a] != 0 && pBuff[a] < 0x20)
                {
                    octetString = true;
                    break;
                }
            }
        }
        if (octetString)
        {
			char tmp[4];			
			for(int a = 0; a != dataSize; ++a)
            {			
				if (a != 0)
				{
					value.strVal.append(".", 1);
				}
				int val = pBuff[a];
#if _MSC_VER > 1000
				int ret = sprintf_s(tmp, 4, "%d", val);				
#else
				int ret = sprintf(tmp, "%d", val);				
#endif				
				if (ret == -1 || ret > 3)
				{
					return ERROR_CODES_INVALID_PARAMETER;
				}
				value.strVal.append(tmp, ret);
			}
        }
        else
        {
			//Remove '\0' from string if used.
            while (dataSize > 0 && pBuff[dataSize - 1] == 0)
            {
                --dataSize;
            }
            value.strVal.append((const char*) pBuff, dataSize);
        }		
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