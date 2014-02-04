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
#include <stdio.h>
#include <string>
using namespace std;
#include "Enums.h"
#include "ErrorCodes.h"
#include "GXHelpers.h"
#include "GXDLMSVariant.h"
#include "GXOBISTemplate.h"

int CGXDLMSVariant::Convert(CGXDLMSVariant* item, DLMS_DATA_TYPE type)
{
	if (item->vt == type)
	{
		return ERROR_CODES_OK;
	}
	CGXDLMSVariant tmp(item);
	if (type == DLMS_DATA_TYPE_STRING)
	{
		item->Clear();
		if (tmp.vt == DLMS_DATA_TYPE_ARRAY || tmp.vt == DLMS_DATA_TYPE_STRUCTURE)
		{
			basic_string<char> str = "{";
			for(vector<CGXDLMSVariant>::iterator it = tmp.Arr.begin(); it != tmp.Arr.end(); ++it)
			{
				str += it->ToString();
				if (it != tmp.Arr.end())
				{
					str += ", ";
				}
			}
			str += "}";
			item->strVal = str;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_BOOLEAN)
		{
			if (tmp.boolVal == 0)
			{
				item->strVal = "False";
			}
			else
			{
				item->strVal = "True";
			}
			item->vt = type;
			return ERROR_CODES_OK;
		}
		char buff[250];
		if (tmp.vt == DLMS_DATA_TYPE_INT32)
		{				
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%ld", tmp.lVal);
#else
			sprintf(buff, "%ld", tmp.lVal);
#endif
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_UINT32)
		{				
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%lu", tmp.ulVal);
#else
			sprintf(buff, "%lu", tmp.ulVal);
#endif		
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_INT8)
		{				
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%d", tmp.cVal);
#else
			sprintf(buff, "%d", tmp.cVal);
#endif				
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_INT16)
		{				
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%d", tmp.iVal);
#else
			sprintf(buff, "%d", tmp.iVal);
#endif				
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_UINT8)
		{				
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%d", tmp.bVal);
#else
			sprintf(buff, "%d", tmp.bVal);
#endif				
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_UINT16)
		{				
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%d", tmp.uiVal);
#else
			sprintf(buff, "%d", tmp.uiVal);
#endif				
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_INT64)
		{				
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%lld", tmp.llVal);
#else
			//sprintf(buff, "%lld", tmp.llVal);
			sprintf(buff, "%I64d", tmp.llVal);
#endif				
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_UINT64)
		{				
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%llu", tmp.ullVal);
#else
			//sprintf(buff, "%llu", tmp.ullVal);
			sprintf(buff, "%I64u", tmp.ullVal);
#endif				
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_ENUM)
		{
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%d", tmp.bVal);
#else
			sprintf(buff, "%d", tmp.bVal);
#endif				
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_FLOAT32)
		{			
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%f", tmp.fltVal);
#else
			sprintf(buff, "%f", tmp.fltVal);
#endif
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_FLOAT64)
		{				
#if _MSC_VER > 1000
			sprintf_s(buff, 250, "%lf", tmp.dblVal);
#else
			sprintf(buff, "%lf", tmp.dblVal);
#endif				
			item->strVal = buff;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (tmp.vt == DLMS_DATA_TYPE_BIT_STRING)
		{
			return ERROR_CODES_NOT_IMPLEMENTED;
		}
		if (tmp.vt == DLMS_DATA_TYPE_OCTET_STRING)
		{
			char buff[4];
			for(vector<unsigned char>::iterator it = tmp.byteArr.begin(); it != tmp.byteArr.end(); ++it)
			{
				if (it != tmp.byteArr.begin())
				{
					item->strVal.append(".");
				}
#if _MSC_VER > 1000
				sprintf_s(buff, 4, "%d", *it);
#else
				sprintf(buff, "%d", *it);
#endif		
				item->strVal.append(buff);
			}
			item->vt = type;
			return ERROR_CODES_OK;
		}
		return ERROR_CODES_NOT_IMPLEMENTED;
	}
	if (item->vt == DLMS_DATA_TYPE_STRING)
	{
		item->Clear();
		if (type == DLMS_DATA_TYPE_BOOLEAN)
		{
			item->boolVal = tmp.strVal.compare("False") == 0 ?  0: 1;
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_INT32)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%ld", &item->lVal);
#else
			sscanf(tmp.strVal.c_str(), "%ld", &item->lVal);
#endif				
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_UINT32)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%lu", &item->ulVal);
#else
			sscanf(tmp.strVal.c_str(), "%lu", &item->ulVal);
#endif				
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_INT8)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%c", &item->cVal);
#else
			sscanf(tmp.strVal.c_str(), "%c", &item->cVal);
#endif
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_INT16)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%hd", &item->iVal);
#else
			sscanf(tmp.strVal.c_str(), "%hd", &item->iVal);
#endif				
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_UINT8)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%hhu", &item->bVal);
#else
			int value;
			sscanf(tmp.strVal.c_str(), "%d", &value);
			item->bVal = value & 0xFF;
#endif				
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_UINT16)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%hu", &item->uiVal);
#else
			int value;
			sscanf(tmp.strVal.c_str(), "%d", &value);
			item->uiVal = value & 0xFFFF;
#endif				
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_INT64)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%lld", &item->llVal);
#else
			//sscanf(tmp.strVal.c_str(), "%lld", &item->llVal);
			sscanf(tmp.strVal.c_str(), "%I64d", &item->llVal);
#endif				
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_UINT64)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%llu", &item->ullVal);
#else
			//sscanf(tmp.strVal.c_str(), "%llu", &item->ullVal);
			sscanf(tmp.strVal.c_str(), "%I64u", &item->ullVal);
#endif				
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_ENUM)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%hhu", &item->bVal);
#else
			int value;
			sscanf(tmp.strVal.c_str(), "%d", &value);
			item->bVal = value & 0xFF;
#endif				
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_FLOAT32)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%f", &item->fltVal);
#else
			sscanf(tmp.strVal.c_str(), "%f", &item->fltVal);
#endif				
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_FLOAT64)
		{
#if _MSC_VER > 1000
			sscanf_s(tmp.strVal.c_str(), "%lf", &item->dblVal);
#else
			sscanf(tmp.strVal.c_str(), "%lf", &item->dblVal);
#endif				
			item->vt = type;
			return ERROR_CODES_OK;
		}
		if (type == DLMS_DATA_TYPE_OCTET_STRING)
		{
			char* pBuff = (char*) tmp.strVal.c_str();
			char* ch;
			int val = 0;
			while((ch = strchr(pBuff, '.')) != NULL)
			{
				*ch = '\0';
#if _MSC_VER > 1000
				if (sscanf_s(pBuff, "%d", &val) == 1)
#else
				if (sscanf(pBuff, "%d", &val) == 1)
#endif					
				{
					item->byteArr.push_back(val);
					pBuff = ++ch;
				}
			}
			if (item->byteArr.size() == 0)
			{
				for(unsigned int pos = 0; pos != tmp.strVal.length(); ++pos)
				{
					item->byteArr.push_back(tmp.strVal[pos]);
				}
			}
#if _MSC_VER > 1000
			else if (sscanf_s(pBuff, "%d", &val) == 1)
#else
			else if (sscanf(pBuff, "%d", &val) == 1)
#endif				
			{
				item->byteArr.push_back(val);
			}
			item->vt = type;
			return ERROR_CODES_OK;
		}
		return ERROR_CODES_NOT_IMPLEMENTED;
	}
	int fromSize = tmp.GetSize();
	int toSize = GetSize(type);
	//If we try to change bigger valut to smaller check that value is not too big.
	//Example Int16 to Int8.
	if (fromSize > toSize)
	{
		unsigned char* pValue = &tmp.bVal;
		for(int pos = toSize; pos != fromSize; ++pos)
		{			
			if (pValue[pos] != 0)
			{
				return ERROR_CODES_INVALID_PARAMETER;
			}
		}
	}
	item->Clear();
	if (fromSize > toSize)
	{
		memcpy(&item->bVal, &tmp.bVal, toSize);
	}
	else
	{
		memset(&item->bVal, 0, toSize);
		memcpy(&item->bVal, &tmp.bVal, fromSize);
	}
	item->vt = type;
	return ERROR_CODES_OK;
}

void CGXDLMSVariant::Clear()
{
	Arr.clear();
	strVal.clear();
	byteArr.clear();
	vt = DLMS_DATA_TYPE_NONE;
}

CGXDLMSVariant::CGXDLMSVariant()
{
	Clear();
}

CGXDLMSVariant::CGXDLMSVariant(CGXDLMSVariant* value)
{
	vt = value->vt;
	if (vt == DLMS_DATA_TYPE_NONE)
	{
		return;
	}
	else if (vt == DLMS_DATA_TYPE_STRING ||
		vt == DLMS_DATA_TYPE_BIT_STRING)
	{
		this->strVal.append(value->strVal);
	}
	else if (vt == DLMS_DATA_TYPE_OCTET_STRING)
	{
		GXHelpers::AddRange(this->byteArr, value->byteArr);
	}
	else if (vt == DLMS_DATA_TYPE_ARRAY || vt == DLMS_DATA_TYPE_STRUCTURE)
	{
		for(vector<CGXDLMSVariant>::iterator it = value->Arr.begin(); it != value->Arr.end(); ++it)
		{
			Arr.push_back(CGXDLMSVariant(*it));
		}
	}
	else
	{
		int size = value->GetSize();
		if (size > 0)
		{
			memcpy(&bVal, &value->bVal, size);
		}
		else
		{
			assert(0);
		}
	}
}

CGXDLMSVariant::CGXDLMSVariant(float value)
{
	vt = DLMS_DATA_TYPE_FLOAT32;
	fltVal = value;
}

CGXDLMSVariant::CGXDLMSVariant(double value)
{
	vt = DLMS_DATA_TYPE_FLOAT64;
	dblVal = value;
}


CGXDLMSVariant::CGXDLMSVariant(unsigned long long value)
{
	vt = DLMS_DATA_TYPE_UINT64;
	ullVal = value;
}

CGXDLMSVariant::CGXDLMSVariant(long long value)
{
	vt = DLMS_DATA_TYPE_INT64;
	llVal = value;
}

CGXDLMSVariant::CGXDLMSVariant(bool value)
{
	vt = DLMS_DATA_TYPE_BOOLEAN;
	boolVal = value;
}


CGXDLMSVariant::CGXDLMSVariant(char value)
{
	vt = DLMS_DATA_TYPE_INT8;
	cVal = value;
}

CGXDLMSVariant::CGXDLMSVariant(short value)
{
	vt = DLMS_DATA_TYPE_INT16;
	iVal = value;
}


CGXDLMSVariant::CGXDLMSVariant(int value)
{
	vt = DLMS_DATA_TYPE_INT32;
	lVal = value;
}

CGXDLMSVariant::CGXDLMSVariant(long value)
{
	vt = DLMS_DATA_TYPE_INT32;
	lVal = value;
}

CGXDLMSVariant::CGXDLMSVariant(struct tm value)
{
	vt = DLMS_DATA_TYPE_DATETIME;
	dateTime.SetValue(value);
}

CGXDLMSVariant::CGXDLMSVariant(vector<unsigned char> value)
{
	vt = DLMS_DATA_TYPE_ARRAY;
	byteArr = value;
}

CGXDLMSVariant::CGXDLMSVariant(CGXDateTime value)
{
	vt = DLMS_DATA_TYPE_DATETIME;
	dateTime = value;
}

CGXDLMSVariant::CGXDLMSVariant(unsigned char value)
{
	vt = DLMS_DATA_TYPE_UINT8;
	bVal = value;
}

CGXDLMSVariant::CGXDLMSVariant(unsigned short value)
{
	vt = DLMS_DATA_TYPE_UINT16;
	uiVal = value;
}

CGXDLMSVariant::CGXDLMSVariant(unsigned long value)
{
	vt = DLMS_DATA_TYPE_UINT32;
	ulVal = value;
}

CGXDLMSVariant::CGXDLMSVariant(unsigned int value)
{
	vt = DLMS_DATA_TYPE_UINT32;
	ulVal = value;
}

CGXDLMSVariant::CGXDLMSVariant(basic_string<char> value)
{
	vt = DLMS_DATA_TYPE_STRING;
	strVal = value;
}	

CGXDLMSVariant::CGXDLMSVariant(const char* value)
{
	vt = DLMS_DATA_TYPE_STRING;
	strVal.append(value);
}

CGXDLMSVariant::CGXDLMSVariant(unsigned char* pValue, int size, DLMS_DATA_TYPE type)
{
	assert(size > -1);
	for(int pos = 0; pos != size; ++pos)
	{
		byteArr.push_back(pValue[pos]);
	}	
	vt = type;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(basic_string<char> value)
{
	Clear();
	vt = DLMS_DATA_TYPE_STRING;
	strVal.append(value);
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(const char* value)
{
	Clear();
	vt = DLMS_DATA_TYPE_STRING;
	strVal.append(value);
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(float value)
{
	Clear();
	vt = DLMS_DATA_TYPE_FLOAT32;
	fltVal = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(double value)
{
	Clear();
	vt = DLMS_DATA_TYPE_FLOAT64;
	dblVal = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(unsigned long long value)
{
	Clear();
	vt = DLMS_DATA_TYPE_UINT64;
	ullVal = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(long long value)
{
	Clear();
	vt = DLMS_DATA_TYPE_INT64;
	llVal = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(bool value)
{
	Clear();
	vt = DLMS_DATA_TYPE_BOOLEAN;
	boolVal = value;
	return *this;
}


CGXDLMSVariant& CGXDLMSVariant::operator=(char value)
{
	Clear();
	vt = DLMS_DATA_TYPE_INT8;
	cVal = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(short value)
{
	Clear();
	vt = DLMS_DATA_TYPE_INT16;
	iVal = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(int value)
{
	Clear();
	vt = DLMS_DATA_TYPE_INT32;
	lVal = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(long value)
{
	Clear();
	vt = DLMS_DATA_TYPE_INT32;
	lVal = value;
	return *this;
}


CGXDLMSVariant& CGXDLMSVariant::operator=(unsigned char value)
{
	Clear();
	vt = DLMS_DATA_TYPE_UINT8;
	bVal = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(unsigned short value)
{
	Clear();
	vt = DLMS_DATA_TYPE_UINT16;
	uiVal = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(unsigned long value)
{
	Clear();
	vt = DLMS_DATA_TYPE_UINT32;
	ulVal = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(struct tm value)
{
	Clear();
	vt = DLMS_DATA_TYPE_DATETIME;
	dateTime.SetValue(value);
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(vector<unsigned char> value)
{
	Clear();
	vt = DLMS_DATA_TYPE_ARRAY;
	byteArr = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(CGXDateTime value)
{
	Clear();
	vt = DLMS_DATA_TYPE_DATETIME;
	dateTime = value;
	return *this;
}

CGXDLMSVariant& CGXDLMSVariant::operator=(unsigned int value)
{
	Clear();
	vt = DLMS_DATA_TYPE_UINT32;
	ulVal = value;
	return *this;
}

void CGXDLMSVariant::Add(const unsigned char* pValue, int count)
{
	vt = DLMS_DATA_TYPE_OCTET_STRING;
	GXHelpers::AddRange(this->byteArr, pValue, count);	
}

void CGXDLMSVariant::Add(const char* pValue, int count)
{
	vt = DLMS_DATA_TYPE_OCTET_STRING;
	GXHelpers::AddRange(this->byteArr, pValue, count);
}

void CGXDLMSVariant::Add(basic_string<char> value)
{
	vt = DLMS_DATA_TYPE_OCTET_STRING;
	const unsigned char* pData = reinterpret_cast<const unsigned char*>(&value[0]);
	byteArr.insert(byteArr.end(), pData, 
        reinterpret_cast<const unsigned char*>(pData) + value.size());
}

bool CGXDLMSVariant::Equals(CGXDLMSVariant& item)
{
	if (vt != item.vt)
	{
		return false;
	}
	int size = GetSize();
	if (size == -1 || size != item.GetSize())
	{
		return false;
	}
	if (size != 0)
	{
		return memcmp(&this->bVal, &item.bVal, size) == 0;	
	}
	return true;
}

int CGXDLMSVariant::ChangeType(DLMS_DATA_TYPE newType)
{
	if (newType == vt)
	{
		return ERROR_CODES_OK;
	}

	if (newType == DLMS_DATA_TYPE_NONE)
	{
		Clear();
		return ERROR_CODES_OK;
	}
	if (vt == DLMS_DATA_TYPE_ARRAY && newType == DLMS_DATA_TYPE_OCTET_STRING)
	{
		return ERROR_CODES_OK;
	}
	if (vt == DLMS_DATA_TYPE_STRING)
	{
		return Convert(this, newType);
	}
	switch(newType)
	{
		case DLMS_DATA_TYPE_STRING:
		case DLMS_DATA_TYPE_BOOLEAN:
		case DLMS_DATA_TYPE_INT32:
		case DLMS_DATA_TYPE_UINT32:
		case DLMS_DATA_TYPE_INT8:
		case DLMS_DATA_TYPE_INT16:
		case DLMS_DATA_TYPE_UINT8:
		case DLMS_DATA_TYPE_UINT16:
		case DLMS_DATA_TYPE_INT64:
		case DLMS_DATA_TYPE_UINT64:
		case DLMS_DATA_TYPE_ENUM:
		case DLMS_DATA_TYPE_FLOAT32:
		case DLMS_DATA_TYPE_FLOAT64: 
			return Convert(this, newType);
		break;
		default:
			//Handled later.
		break;
	}
	CGXDLMSVariant tmp(this);
	switch(vt)
	{
	case DLMS_DATA_TYPE_BOOLEAN:
		return ERROR_CODES_INVALID_PARAMETER;
	case DLMS_DATA_TYPE_BIT_STRING:
		return ERROR_CODES_INVALID_PARAMETER;
	case DLMS_DATA_TYPE_INT32:
		switch(newType)
		{
			case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
				return ERROR_CODES_NOT_IMPLEMENTED;
				break;
			default:
				return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_UINT32:
		switch(newType)
		{
		case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_OCTET_STRING:
		switch(newType)
		{
		case DLMS_DATA_TYPE_DATETIME:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		case DLMS_DATA_TYPE_DATE:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		case DLMS_DATA_TYPE_TIME:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
		switch(newType)
		{
		case DLMS_DATA_TYPE_INT32:
			break;
		case DLMS_DATA_TYPE_UINT32:
			break;
		case DLMS_DATA_TYPE_STRING:
			break;
		case DLMS_DATA_TYPE_INT8:
			break;
		case DLMS_DATA_TYPE_INT16:
			break;
		case DLMS_DATA_TYPE_UINT8:
			break;
		case DLMS_DATA_TYPE_UINT16:
			break;
		case DLMS_DATA_TYPE_INT64:
			break;
		case DLMS_DATA_TYPE_UINT64:
			break;
		case DLMS_DATA_TYPE_ENUM:
			break;
		case DLMS_DATA_TYPE_FLOAT32:
			break;
		case DLMS_DATA_TYPE_FLOAT64: 
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_INT8:
		switch(newType)
		{
		case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_INT16:
		switch(newType)
		{
		case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_UINT8:
		switch(newType)
		{
		case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_UINT16:
		switch(newType)
		{
		case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_INT64:
		switch(newType)
		{
		case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_UINT64:
		switch(newType)
		{
		case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_ENUM:
		switch(newType)
		{
		case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_FLOAT32:
		switch(newType)
		{
		case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_FLOAT64: 
		switch(newType)
		{
		case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
			return ERROR_CODES_NOT_IMPLEMENTED;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_DATETIME:
		switch(newType)
		{
		case DLMS_DATA_TYPE_OCTET_STRING:
			break;
		case DLMS_DATA_TYPE_STRING:
			break;
		case DLMS_DATA_TYPE_DATE:
			break;
		case DLMS_DATA_TYPE_TIME:
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_DATE:
		switch(newType)
		{
		case DLMS_DATA_TYPE_OCTET_STRING:
			break;
		case DLMS_DATA_TYPE_STRING:
			break;
		case DLMS_DATA_TYPE_DATETIME:
			break;
		case DLMS_DATA_TYPE_DATE:
			break;
		case DLMS_DATA_TYPE_TIME:
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_TIME:
		switch(newType)
		{
		case DLMS_DATA_TYPE_OCTET_STRING:
			break;
		case DLMS_DATA_TYPE_STRING:
			break;
		case DLMS_DATA_TYPE_DATETIME:
			break;
		case DLMS_DATA_TYPE_DATE:
			break;
		case DLMS_DATA_TYPE_TIME:
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		break;
	case DLMS_DATA_TYPE_ARRAY:
	case DLMS_DATA_TYPE_STRUCTURE:
	case DLMS_DATA_TYPE_COMPACT_ARRAY:
		default:
			return ERROR_CODES_INVALID_PARAMETER;
	}
	return ERROR_CODES_OK;
}

//Returns bytes as Big Endian byteorder.
void CGXDLMSVariant::GetBytes(std::vector<unsigned char>& buff)
{	
	if (this->vt == DLMS_DATA_TYPE_STRUCTURE ||
		this->vt == DLMS_DATA_TYPE_ARRAY)
	{
		for(vector<CGXDLMSVariant>::iterator it = Arr.begin(); it != Arr.end(); ++it)
		{
			it->GetBytes(buff);
		}
		return;
	}

	int size = GetSize();
	if (size > 0)
	{
		if (this->vt == DLMS_DATA_TYPE_STRING)
		{
			for(int pos = 0; pos != size; ++pos)
			{
				buff.push_back(strVal[pos]);
			}		
		}
		else if (this->vt == DLMS_DATA_TYPE_OCTET_STRING)
		{				
			for(int pos = 0; pos != size; ++pos)
			{
				buff.push_back(byteArr[pos]);
			}		
		}
		else if (this->vt == DLMS_DATA_TYPE_DATETIME)
		{
			buff[buff.size() - 1] = DLMS_DATA_TYPE_OCTET_STRING;
			buff.push_back(12);
			//Add year.
			unsigned short year = 0xFFFF;
			struct tm dt = this->dateTime.GetValue();
			DATETIME_SKIPS skip = this->dateTime.GetSkip();
			if (dt.tm_year != -1 && (skip & DATETIME_SKIPS_YEAR) == 0)
			{
				year = 1900 + dt.tm_year;
			}
			GXHelpers::ChangeByteOrder(buff, &year, 2);
			//Add month
			if (dt.tm_mon != -1 && (skip & DATETIME_SKIPS_MONTH) == 0)
			{
				buff.push_back(dt.tm_mon + 1);
			}
			else
			{
				buff.push_back(0xFF);
			}
			//Add day
			if (dt.tm_mday != -1 && (skip & DATETIME_SKIPS_DAY) == 0)
			{
				buff.push_back(dt.tm_mday);
			}
			else
			{
				buff.push_back(0xFF);
			}
			//Add week day
			buff.push_back(0xFF);
			//Add Hours
			if (dt.tm_hour != -1 && (skip & DATETIME_SKIPS_HOUR) == 0)
			{
				buff.push_back(dt.tm_hour);
			}
			else
			{
				buff.push_back(0xFF);
			}
			//Add Minutes
			if (dt.tm_min != -1 && (skip & DATETIME_SKIPS_MINUTE) == 0)
			{
				buff.push_back(dt.tm_min);
			}
			else
			{
				buff.push_back(0xFF);
			}
			//Add seconds.
			if (dt.tm_sec != -1 && (skip & DATETIME_SKIPS_SECOND) == 0)
			{
				buff.push_back(dt.tm_sec);
			}
			else
			{
				buff.push_back(0xFF);
			}
			//Add ms.
			buff.push_back(0xFF);
			//Add Deviation
			unsigned short dev = 0x8000; //(not specified)
			GXHelpers::ChangeByteOrder(buff, &dev, 2);			 
			//Add clock status
			buff.push_back(0x0);
		}
		else
		{			
			GXHelpers::ChangeByteOrder(buff, &bVal, size);
		}
	}
}

//Get size in bytes.
int CGXDLMSVariant::GetSize()
{
	if (this->vt == DLMS_DATA_TYPE_STRING ||
		this->vt == DLMS_DATA_TYPE_BIT_STRING)
	{
		return strVal.size();
	}
	if (this->vt == DLMS_DATA_TYPE_OCTET_STRING)
	{
		return byteArr.size();
	}
	return GetSize(this->vt);
}

//Get size in bytes.
int CGXDLMSVariant::GetSize(DLMS_DATA_TYPE vt)
{
	int nSize = -1;
	switch (vt)
	{
	case DLMS_DATA_TYPE_NONE:
		nSize = 0;
		break;
	case DLMS_DATA_TYPE_BOOLEAN:
	case DLMS_DATA_TYPE_INT8:
	case DLMS_DATA_TYPE_UINT8:
	case DLMS_DATA_TYPE_ENUM:
		nSize = 1;
		break;
	case DLMS_DATA_TYPE_INT16:
	case DLMS_DATA_TYPE_UINT16:	
		nSize = 2;
		break;		
	case DLMS_DATA_TYPE_INT32:
	case DLMS_DATA_TYPE_UINT32:
	case DLMS_DATA_TYPE_FLOAT32:
		nSize = 4;
		break;
	case DLMS_DATA_TYPE_INT64:
	case DLMS_DATA_TYPE_UINT64:
	case DLMS_DATA_TYPE_FLOAT64:
		nSize = 8;
		break;	
	case DLMS_DATA_TYPE_BIT_STRING:
		break;
	case DLMS_DATA_TYPE_OCTET_STRING:
		break;
	case DLMS_DATA_TYPE_STRING:
		nSize = -1;
		break;
	case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
		break;
	case DLMS_DATA_TYPE_DATETIME:
		nSize = 12;
		break;
	case DLMS_DATA_TYPE_DATE:
		break;
	case DLMS_DATA_TYPE_TIME:
		break;
	case DLMS_DATA_TYPE_ARRAY:
		break;
	case DLMS_DATA_TYPE_STRUCTURE:
		break;
	case DLMS_DATA_TYPE_COMPACT_ARRAY:
		break;
	}
	return nSize;
}

basic_string<char> CGXDLMSVariant::ToString()
{
	CGXDLMSVariant tmp(this);
	tmp.ChangeType(DLMS_DATA_TYPE_STRING);
	if (tmp.strVal.length() == 0)
	{
		return "";
	}
	return tmp.strVal.c_str();
}

basic_string<char> CGXDLMSVariant::ToDateTime()
{
	basic_string<char> data;
	char buff[20];
	if (this->vt == DLMS_DATA_TYPE_DATETIME ||
		(this->vt == DLMS_DATA_TYPE_OCTET_STRING && this->byteArr.size() == 12))
	{
		if (this->byteArr.size() != 12) //If there is not enough data available.
		{		
			return "Invalid";
		}
		int month, day, hour, minute, second, year;
		//Get year.
		year = CGXOBISTemplate::GetUInt16(&this->byteArr[0]);		
		if (year != 0xFFFF)
		{
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", year);
#else
			sprintf(buff, "%d", year);
#endif
			data.append(buff);
		}
		//Get month
		month = this->byteArr[2];
		if (month != 0xFF)
		{
			if (data.size() != 0)
			{
				data.append("/");
			}
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", month);
#else
			sprintf(buff, "%d", month);
#endif
			data.append(buff);
		}
		//Get day
		day = this->byteArr[3];
		if (day != 0xFF)
		{
			if (data.size() != 0)
			{
				data.append("/");
			}
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", day);
#else
			sprintf(buff, "%d", day);
#endif
			data.append(buff);
		}
		//Skip week day
		//Get time.				
		hour = this->byteArr[4];
		minute = this->byteArr[5];
		second = this->byteArr[6];		
#if _MSC_VER > 1000
		sprintf_s(buff, 20, " %d:%d:%d", hour, minute, second);
#else
		sprintf(buff, " %d:%d:%d", hour, minute, second);
#endif
		data.append(buff);
		return data;
	}
	if (this->vt == DLMS_DATA_TYPE_DATE)
	{
		if (this->byteArr.size() != 5) //If there is not enough data available.
		{		
			return "Invalid";
		}	
		int month, day, year;
		//Get year.
		year = CGXOBISTemplate::GetUInt16(&this->byteArr[0]);		
		if (year != 0xFFFF)
		{
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", year);
#else
			sprintf(buff, "%d", year);
#endif
			data.append(buff);
		}
		//Get month
		month = this->byteArr[2];
		if (month != 0xFF)
		{
			if (data.size() != 0)
			{
				data.append("/");
			}
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", month);
#else
			sprintf(buff, "%d", month);
#endif
			data.append(buff);
		}
		//Get day
		day = this->byteArr[3];
		if (day != 0xFF)
		{
			if (data.size() != 0)
			{
				data.append("/");
			}
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", day);
#else
			sprintf(buff, "%d", day);
#endif
			data.append(buff);
		}
#if _MSC_VER > 1000
		sprintf_s(buff, 20, "%d/%d/%d", month, day, year);
#else
		sprintf(buff, "%d/%d/%d", month, day, year);
#endif		
		data.append(buff);
		return data;
	}
	if (this->vt == DLMS_DATA_TYPE_TIME)
	{
		if (this->byteArr.size() != 7) //If there is not enough data available.
		{		
			return "Invalid";
		}	
		int hour, minute, second;
		//Get time.				
		hour = this->byteArr[0];
		minute = this->byteArr[1];
		second = this->byteArr[2];
#if _MSC_VER > 1000
		sprintf_s(buff, 20, "%d:%d:%d", hour, minute, second);
#else
		sprintf(buff, "%d:%d:%d", hour, minute, second);
#endif		
		return buff;
	}
	return "Invalid";
}

basic_string<char> CGXDLMSVariant::ToDate()
{
	basic_string<char> data;
	char buff[20];
	if (this->vt == DLMS_DATA_TYPE_DATETIME ||
		(this->vt == DLMS_DATA_TYPE_OCTET_STRING && this->byteArr.size() == 12))
	{
		if (this->byteArr.size() != 12) //If there is not enough data available.
		{		
			return "Invalid";
		}	
		int month, day, year;
		//Get year.
		year = CGXOBISTemplate::GetUInt16(&this->byteArr[0]);		
		if (year != 0xFFFF)
		{
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", year);
#else
			sprintf(buff, "%d", year);
#endif
			data.append(buff);
		}
		//Get month
		month = this->byteArr[2];
		if (month != 0xFF)
		{
			if (data.size() != 0)
			{
				data.append("/");
			}
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", month);
#else
			sprintf(buff, "%d", month);
#endif
			data.append(buff);
		}
		//Get day
		day = this->byteArr[3];
		if (day != 0xFF)
		{
			if (data.size() != 0)
			{
				data.append("/");
			}
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", day);
#else
			sprintf(buff, "%d", day);
#endif
			data.append(buff);
		}
		return data;
	}
	if (this->vt == DLMS_DATA_TYPE_DATE)
	{
		if (this->byteArr.size() != 5) //If there is not enough data available.
		{		
			return "Invalid";
		}	
		int month, day, year;
		//Get year.
		year = CGXOBISTemplate::GetUInt16(&this->byteArr[0]);		
		if (year != 0xFFFF)
		{
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", year);
#else
			sprintf(buff, "%d", year);
#endif
			data.append(buff);
		}
		//Get month
		month = this->byteArr[2];
		if (month != 0xFF)
		{
			if (data.size() != 0)
			{
				data.append("/");
			}
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", month);
#else
			sprintf(buff, "%d", month);
#endif
			data.append(buff);
		}
		//Get day
		day = this->byteArr[3];
		if (day != 0xFF)
		{
			if (data.size() != 0)
			{
				data.append("/");
			}
#if _MSC_VER > 1000
			sprintf_s(buff, 20, "%d", day);
#else
			sprintf(buff, "%d", day);
#endif
			data.append(buff);
		}
		return data;
	}
	return "Invalid";
}

basic_string<char> CGXDLMSVariant::ToTime()
{
	if (this->vt == DLMS_DATA_TYPE_DATETIME ||
		(this->vt == DLMS_DATA_TYPE_OCTET_STRING && this->byteArr.size() == 12))
	{
		if (this->byteArr.size() != 12) //If there is not enough data available.
		{		
			return "Invalid";
		}	
		int hour, minute, second;
		//Get time.				
		hour = this->byteArr[4];
		minute = this->byteArr[5];
		second = this->byteArr[6];
		char buff[25];
#if _MSC_VER > 1000
		sprintf_s(buff, 25, "%d:%d:%d", hour, minute, second);
#else
		sprintf(buff, "%d:%d:%d", hour, minute, second);
#endif		
		return buff;
	}	
	if (this->vt == DLMS_DATA_TYPE_TIME)
	{
		if (this->byteArr.size() != 7) //If there is not enough data available.
		{		
			return "Invalid";
		}	
		int hour, minute, second;
		//Get time.				
		hour = this->byteArr[0];
		minute = this->byteArr[1];
		second = this->byteArr[2];
		char buff[25];
#if _MSC_VER > 1000
		sprintf_s(buff, 25, "%d:%d:%d", hour, minute, second);
#else
		sprintf(buff, "%d:%d:%d", hour, minute, second);
#endif		
		return buff;
	}
	return "Invalid";
}

int CGXDLMSVariant::ToInteger()
{
	if (vt == DLMS_DATA_TYPE_NONE)
	{
		return 0;
	}

	if (vt == DLMS_DATA_TYPE_BOOLEAN)
	{
		return boolVal ? 1 : 0;
	}
	if (vt == DLMS_DATA_TYPE_INT32)
	{
		return lVal;
	}
	if (vt == DLMS_DATA_TYPE_UINT32)
	{
		return ulVal;
	}
	if (vt == DLMS_DATA_TYPE_BINARY_CODED_DESIMAL)
	{
		assert(0);
	}
	if (vt == DLMS_DATA_TYPE_STRING_UTF8)
	{
		//TODO:
		assert(0);
	}
	if (vt == DLMS_DATA_TYPE_INT8)
	{
		return cVal;
	}

	if (vt == DLMS_DATA_TYPE_INT16)
	{
		return iVal;
	}
	if (vt == DLMS_DATA_TYPE_UINT8)
	{
		return bVal;
	}
	if (vt == DLMS_DATA_TYPE_UINT16)
	{
		return uiVal;
	}
	if (vt == DLMS_DATA_TYPE_INT64)
	{
		//TODO:
		assert(0);
	}
	if (vt == DLMS_DATA_TYPE_UINT64)
	{
		//TODO:
		assert(0);
	}
	if (vt == DLMS_DATA_TYPE_ENUM)
	{
		return bVal;
	}
	if (vt == DLMS_DATA_TYPE_FLOAT32)
	{
		//TODO:
		assert(0);
	}
	if (vt == DLMS_DATA_TYPE_FLOAT64)
	{
		//TODO:
		assert(0);
	}
	assert(0);
	return 0;

}
