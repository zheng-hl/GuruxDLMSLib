//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL:  $
//
// Version:         $Revision:  $,
//                  $Date:  $
//                  $Author: $
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------

#pragma once

const unsigned char LogicalNameObjectID[7] = {0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01};
const unsigned char ShortNameObjectID[7] = {0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x02};

class CGXApplicationContextName
{
public:
	//Constructor
	CGXApplicationContextName() : UseLN(false)
	{		

	}
	int CodeData(std::vector<unsigned char>& buff)
	{
		buff.push_back(0xA1); //Application context name tag
		buff.push_back(0x09); //Len
		buff.push_back(0x6);
		buff.push_back(7); //Len
		if (UseLN)
		{
			GXHelpers::AddRange(buff, LogicalNameObjectID, 7);
		}
		else
		{
			GXHelpers::AddRange(buff, ShortNameObjectID, 7);
		}
		return ERROR_CODES_OK;
	}
	
	//Encode data.
	int EncodeData(unsigned char*& pBuff, int& size)
	{
		unsigned char* pStart = pBuff;
		if (pBuff == NULL)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}
		int tag = (pBuff[0] & 0xFF);
		if (tag != 0xA1)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}
		++pBuff;
		//Get length.
		int len = *pBuff++ & 0xFF;
		if (size < len)
		{
			return ERROR_CODES_ENCODING_FAILED;
		}
		pBuff++;
		len = *pBuff++ & 0xFF;
		UseLN = memcmp(pBuff, LogicalNameObjectID, 7) == 0;
		pBuff += 7;
		size -= (pBuff - pStart);
		return ERROR_CODES_OK;
	}

	bool UseLN;
};
