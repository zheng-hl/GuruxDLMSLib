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

#include "GXDLMS.h"
#include "GXAPDU.h"
#include "GXOBISTemplate.h"
#include "GXDLMSClient.h"
#include "GXDLMSObjectFactory.h"

void CGXDLMS::GenerateFCS16Table()
{
	#define P       0x8408
	unsigned int b, v;
	int i;
	for (b = 0; ; )
	{
		v = b;
		for (i = 8; i--;)
		{
			v = v & 1 ? (v >> 1) ^ P : v >> 1;
		}
		m_FCS16Table[b] = v & 0xFFFF;
		if (++b == 256)
		{
			break;
		}
	}
}

unsigned char CGXDLMS::GenerateIFrame()
{
	//Expected frame number is increased only when first keep alive msg is send...
	if (!m_bIsLastMsgKeepAliveMsg)
	{
		++m_ExpectedFrame;
	}
	return GenerateNextFrame();
}

unsigned char CGXDLMS::GenerateNextFrame()
{
	++m_FrameSequence;
	unsigned char val = ((m_FrameSequence & 0x7) << 1) & 0xF;
	val |= ((((m_ExpectedFrame & 0x7) << 1) | 0x1) & 0xF) << 4;
	m_bIsLastMsgKeepAliveMsg = false;
	return val;
}

unsigned char CGXDLMS::GenerateAliveFrame()
{
	//Expected frame number is increased only when first keep alive msg is send...
	if (!m_bIsLastMsgKeepAliveMsg)
	{
		++m_ExpectedFrame;
		m_bIsLastMsgKeepAliveMsg = true;
	}
	unsigned char val = 1;
	val |= (((((m_ExpectedFrame) & 0x7) << 1) | 0x1) & 0xF) << 4;
	return val;
}

//Return true if frame sequences are same.
bool CGXDLMS::IsExpectedFrame(unsigned char send, unsigned char received)
{
	//In keep alive msg send ID might be same as receiver ID.
	return send == received ||
		((send >> 5) & 0x7) == ((received >> 1) & 0x7) ||
		(send == 0x93 && received == 0x73);
}

// Generate I-frame: Information frame
//
// 0 Receive Ready (denoted RR(R)) is a positive acknowledge ACK of all frames
// up to and including frame number R-1.
// 1 Reject (denoted RE.J(R)) is a negative acknowledge NAK
// of a Go-back-N mechanism. ie start retransmitting from frame number R.
// 2 Receive Not Ready (denoted RNR(R)) is a positive acknowledge of all
// frames up to and including R-1 but the sender must pause until a
// Receive Ready arrives. This can be used to pause the sender because of
// temporary problems at the receiver.
// 3 Selective Reject (denoted SREJ(R)) is a negative acknowledge in a
// Selective Repeat mechanism. ie resend only frame R. It is not
// supported in several implementations.
unsigned char CGXDLMS::GenerateSupervisoryFrame(unsigned char type)
{
	++m_ExpectedFrame;
	unsigned char val = (((type & 0x3) << 2) | 0x1) & 0xF;
	val |= ((((m_ExpectedFrame & 0x7) << 1) | 0x1) & 0xF) << 4;
	m_bIsLastMsgKeepAliveMsg = false;
	return val;
}

bool CGXDLMS::IsReceiverReadyRequest(unsigned char val)
{
	bool b =  (val & 0xF) == 1 && (val >> 4) == (((m_ExpectedFrame & 0x7) << 1) | 0x1);
	return b;
}

unsigned short CGXDLMS::CountCRC(unsigned char* pBuff, int Index, int Count)
{
	unsigned short FCS16 = 0xFFFF;
	for(short pos = 0; pos < Count; ++pos)
	{
		FCS16 = (FCS16 >> 8) ^ m_FCS16Table[(FCS16 ^ pBuff[Index + pos]) & 0xFF];
	}
	FCS16 = ~FCS16;
	return FCS16;
}

unsigned short CGXDLMS::CountCRC(std::vector<unsigned char>& Buff, int Index, int Count)
{
	unsigned short FCS16 = 0xFFFF;
	for(short pos = 0; pos < Count; ++pos)
	{
		FCS16 = (FCS16 >> 8) ^ m_FCS16Table[(FCS16 ^ Buff[Index + pos]) & 0xFF];
	}
	FCS16 = ~FCS16;
	return FCS16;
}

bool CGXDLMS::IsDLMSPacketComplete(std::vector<unsigned char>& Data)
{
	unsigned char* pData = &(*Data.begin());
	return IsDLMSPacketComplete(pData, Data.size());
}

bool CGXDLMS::IsDLMSPacketComplete(unsigned char* Data, int buffSize)
{
	if (Data == NULL)
	{
		return false;
	}
	unsigned char FrameType = -1;
	unsigned char* pBuff = NULL;
	int size = 0;
	DLMS_COMMAND cmd;
	GXDLMS_DATA_REQUEST_TYPES MoreData = GXDLMS_DATA_REQUEST_TYPES_NONE;	
	int ret = GetDataFromHDLCFrame(Data, buffSize, FrameType, pBuff, size, cmd, MoreData);
	if (pBuff != NULL)
	{
		delete pBuff;
		pBuff = NULL;
	}	
	return ret == 0;
}

int CGXDLMS::GetDataFromPacket(unsigned char* Packet, int PacketSize, unsigned char*& Data, int& DataSize, GXDLMS_DATA_REQUEST_TYPES& moreData, DLMS_COMMAND& cmd)
{	
	if (Packet == NULL || PacketSize < 0)
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}
	cmd = DLMS_COMMAND_NONE;
	int ret;
	unsigned char* pBuff = NULL;
	unsigned char FrameType = -1;
	int DataLen = -1;		
	if ((ret = GetDataFromHDLCFrame(Packet, PacketSize, FrameType, pBuff, DataLen, cmd, moreData, true, NULL, DataSize == 0, true, NULL)) != 0)
	{
		if (pBuff != NULL)
		{
			delete pBuff;
		}
		return ret;
	}	
	if (cmd == DLMS_COMMAND_REJECTED)
	{
		if (pBuff != NULL)
		{
			delete pBuff;
		}
		return ERROR_CODES_FRAME_REJECTED;
	}
	//Return empty if HDLC frame not found.
	if (ret == ERROR_CODES_FALSE)
	{
		if (pBuff != NULL)
		{
			delete pBuff;
		}			
		return ERROR_CODES_OK;
	}
	//If there is no data.
	if (DataSize == 0)
	{
		Data = pBuff;
		DataSize = DataLen;
	}
	//Append data to the exist data.
	else
	{
		unsigned char* pExists = Data;
		Data = new unsigned char[DataSize + DataLen];
		memcpy(Data, pExists, DataSize);
		memcpy(Data + DataSize, pBuff, DataLen);
		DataSize += DataLen;
		delete pBuff;
		delete pExists;
	}
	return ERROR_CODES_OK;
}

//Return ERROR_CODES_FALSE if LLC bytes are not included.
int CGXDLMS::CheckLLCBytes(unsigned char* pBuff, int& pos, int length)
{
	if (m_InterfaceType == GXDLMS_INTERFACETYPE_NET)
	{
		assert(0);
	}
	if (m_Server)
	{
		//Check LLC bytes.
		if (memcmp(pBuff + pos, LLCSendBytes, 3) != 0)
		{
			return ERROR_CODES_FALSE;
		}
	}
	else
	{
		//Check LLC bytes.
		if (memcmp(pBuff + pos, LLCReplyBytes, 3) != 0)
		{
			return ERROR_CODES_FALSE;
		}
	}
	pos += 3;
	return ERROR_CODES_OK;
}

/// <summary>
/// Reserved for internal use.
/// </summary>
int CGXDLMS::GenerateMessage(CGXDLMSVariant& name, int ParameterCount, vector<unsigned char>& data, OBJECT_TYPE interfaceClass, int AttributeOrdinal, DLMS_COMMAND cmd, vector< vector<unsigned char> >& Packets)
{
    CGXDLMSVariant tmp = m_Limits.GetMaxInfoRX();
	tmp.ChangeType(DLMS_DATA_TYPE_INT32);
    std::vector<unsigned char> buff(0, 20 + data.size());
    if (m_UseLogicalNameReferencing)
    {
        if (cmd == DLMS_COMMAND_GET_REQUEST || cmd == DLMS_COMMAND_SET_REQUEST || cmd == DLMS_COMMAND_METHOD_REQUEST)
        {
            //Interface class.
            GXHelpers::SetUInt16(interfaceClass, buff);
            //Add LN			
			if (name.vt == DLMS_DATA_TYPE_OCTET_STRING)
			{
				GXHelpers::AddRange(buff, name.byteArr, 0, name.byteArr.size());
			}
			else
			{
				unsigned char ln[6];
				int ret = GXHelpers::GetLogicalName(name.strVal, ln);
				if (ret != 0)
				{
					return ret;
				}
				GXHelpers::AddRange(buff, ln, 6);
			}			
            buff.push_back(AttributeOrdinal);
			if (data.size() == 0 || cmd == DLMS_COMMAND_SET_REQUEST)
            {
                buff.push_back(0x0); //Items count
            }
            else
            {
                buff.push_back(0x1); //Items count
            }            
        }
    }
    else
    {
        //Set read or write tag.
        buff.push_back(cmd);
		buff.push_back(1);        
        if (cmd == DLMS_COMMAND_READ_RESPONSE || cmd == DLMS_COMMAND_WRITE_RESPONSE)
        {
            buff.push_back(0x0);
        }
        else
        {
            buff.push_back(ParameterCount);
			name.ChangeType(DLMS_DATA_TYPE_UINT32);
			unsigned short base_address = name.uiVal;
            if (cmd == DLMS_COMMAND_METHOD_REQUEST)
            {
                base_address += (unsigned short)AttributeOrdinal;
            }
            else
            {
                base_address += (unsigned short)((AttributeOrdinal - 1) * 8);
            }
            GXHelpers::SetUInt16(base_address, buff);
        }
    }
    if (data.size() != 0)
    {
		GXHelpers::AddRange(buff, data);
    }            
	return SplitToBlocks(buff, cmd, Packets);
}

int CGXDLMS::GetSNData(unsigned char* pBuff, int dataSize, int& index, int* pError, DLMS_COMMAND& cmd)
{                        
    //Check that this is reply
	if (cmd != DLMS_COMMAND_READ_REQUEST && cmd != DLMS_COMMAND_WRITE_REQUEST &&
		cmd != DLMS_COMMAND_SET_REQUEST && cmd != DLMS_COMMAND_SET_RESPONSE && 
		cmd != DLMS_COMMAND_READ_RESPONSE && cmd != DLMS_COMMAND_WRITE_RESPONSE &&
		cmd != DLMS_COMMAND_GET_REQUEST && cmd != DLMS_COMMAND_GET_RESPONSE &&
		cmd != DLMS_COMMAND_METHOD_REQUEST && cmd != DLMS_COMMAND_METHOD_RESPONSE)
    {
        return ERROR_CODES_INVALID_PARAMETER;
    }
    ++index;
	if (cmd == DLMS_COMMAND_READ_RESPONSE || cmd == DLMS_COMMAND_WRITE_RESPONSE)
    {        
        //Add reply status.
        ++index;
        bool bIsError = (pBuff[index++] != 0);
        if (bIsError && pError != NULL)
        {
            *pError = pBuff[index++];
        }
    }            
	return ERROR_CODES_OK;
}

int CGXDLMS::GetLNData(unsigned char* pBuff, int dataSize, int& index, int* pError, GXDLMS_DATA_REQUEST_TYPES& MoreData, DLMS_COMMAND& cmd)
{
    ++index;
    if (dataSize - 1 < index)
    {
        return ERROR_CODES_OUTOFMEMORY;
    }
    //If meter returns exception.
    if (cmd == 0xD8)
    {
//        StateError StateError = (StateError) buff[index++];
  //      ServiceError ServiceError = (ServiceError) buff[index++];
    //    throw new GXDLMSException(StateError.ToString() + " " + ServiceError.ToString());
    }	    
	if (cmd != DLMS_COMMAND_GET_RESPONSE && cmd != DLMS_COMMAND_SET_RESPONSE &&
		cmd != DLMS_COMMAND_SET_REQUEST && cmd != DLMS_COMMAND_GET_REQUEST && 
		cmd != DLMS_COMMAND_METHOD_REQUEST && cmd != DLMS_COMMAND_METHOD_RESPONSE)
    {
        return ERROR_CODES_INVALID_PARAMETER;
    }
    unsigned char AttributeID = pBuff[index++];
    if (dataSize - 1 < index)
    {
        return ERROR_CODES_OUTOFMEMORY;
    }
    //Skip Invoke ID and priority.
    ++index;
    if (dataSize - 1 < index)
    {
        return ERROR_CODES_OUTOFMEMORY;
    }
    if (m_Server && AttributeID == 0x2)
    {
        MoreData = GXDLMS_DATA_REQUEST_TYPES_BLOCK;
    }
    else
    {
        if (m_Server && AttributeID == 0x01)
        {
			
        }
        else
        {
            unsigned char Priority = pBuff[index++];
            if (dataSize - 1 < index)
            {
                return ERROR_CODES_OUTOFMEMORY;
            }
            if (AttributeID == 0x01 && Priority != 0)
            {
				if (pError != NULL)
				{
					*pError = pBuff[index++];
				}
                if (dataSize - 1 < index)
                {
                    return ERROR_CODES_OUTOFMEMORY;
                }
            }
            else
            {
                if (AttributeID == 0x02)
                {
                    if (Priority == 0)
                    {
                        MoreData = (GXDLMS_DATA_REQUEST_TYPES) (MoreData | GXDLMS_DATA_REQUEST_TYPES_BLOCK);
                    }
					m_PacketIndex = CGXOBISTemplate::GetUInt32(pBuff + index);
					index += 4;
                    ++index;
                    //Get data length.
					CGXOBISTemplate::GetObjectCount(pBuff, index);
                }
            }
        }
    }
    return ERROR_CODES_OK;
}

int CGXDLMS::GetData(unsigned char* pBuff, int& pos1, int dataSize, bool bReply, DLMS_COMMAND& cmd, GXDLMS_DATA_REQUEST_TYPES& MoreData, int* pError)
{
	//Check response.
	unsigned char command = pBuff[pos1];
	if (command == 0x60)
	{
		cmd = DLMS_COMMAND_AARQ;					
	}
	else if (command == 0x62)
	{
		cmd = DLMS_COMMAND_DISCONNECT_REQUEST;
	}
	else if (bReply && command != 0x61 && command != 0x60)
	{
		cmd = (DLMS_COMMAND) command;
		//If LN is used, check is there more data available.
        if (m_UseLogicalNameReferencing)
        {
			int ret = GetLNData(pBuff, dataSize, pos1, pError, MoreData, cmd);
            if (ret != 0)
            {
				return ret;
            }
        }
        else
        {
			int ret = GetSNData(pBuff, dataSize, pos1, pError, cmd);
			if (ret != 0)
            {
				return ret;
            }                        
        }					
	}
	return 0;
}

int CGXDLMS::GetDataFromHDLCFrame(unsigned char* pBuff, int dataSize, unsigned char& FrameType, unsigned char*& pData,
									   int& DataLen, DLMS_COMMAND& cmd, GXDLMS_DATA_REQUEST_TYPES& MoreData, 
									   bool bReply, unsigned int* pNextIndex, bool IncludeHeader, bool bCheckCRC, int* pError)
{
	MoreData = GXDLMS_DATA_REQUEST_TYPES_NONE;
	int ret, PacketStartID = 0, pos1 = 0, FrameLen = 0;
	//If DLMS frame is generated.
	if (m_InterfaceType != GXDLMS_INTERFACETYPE_NET)
	{
		//Find start of HDLC frame.
		for (pos1 = 0; pos1 < dataSize; ++pos1)
		{
			if (pBuff[pos1] == HDLCFrameStartEnd)
			{
				PacketStartID = pos1;
				break;
			}
		}
		if (pos1 == dataSize || pBuff[pos1++] != HDLCFrameStartEnd) //Not a HDLC frame.
		{
			return ERROR_CODES_INVALID_DATA_FORMAT;
		}
		FrameType = pBuff[pos1++];

		//Is there more data available.
        if ((FrameType & 0x8) != 0)
        {
            MoreData = GXDLMS_DATA_REQUEST_TYPES_FRAME;
        }
        //Check frame length.
        if ((FrameType & 0x7) != 0)
        {
            FrameLen = ((FrameType & 0x7) << 8);
        }
		//If not enought data.
		FrameLen += pBuff[pos1++];
		if (bCheckCRC)
		{
			if (dataSize - pos1 + 2 < FrameLen)
			{
				// Not enought data to parse %d(%d)", len, FrameLen + 2);
				return ERROR_CODES_OUTOFMEMORY;
			}
			if (MoreData == GXDLMS_DATA_REQUEST_TYPES_NONE && pBuff[FrameLen + PacketStartID + 1] != HDLCFrameStartEnd)
			{
				return ERROR_CODES_INVALID_DATA_FORMAT;
			}
		}
	}
	else
	{
		//Get version
		unsigned int ver = 0;
		GXHelpers::ChangeByteOrder(&ver, pBuff + pos1, 2);
		pos1 += 2;
		if (ver != 1)
		{
			return ERROR_CODES_INVALID_VERSION_NUMBER;
		}		
	}
	int clientSize = ClientID.GetSize();
	int serverSize = ServerID.GetSize();
	unsigned long clientID, serverID;
	GXHelpers::ChangeByteOrder(&clientID, &ClientID.bVal, clientSize);
	GXHelpers::ChangeByteOrder(&serverID, &ServerID.bVal, serverSize);
	if (((m_Server || !bReply) && m_InterfaceType != GXDLMS_INTERFACETYPE_NET) ||
		((!m_Server && bReply) && m_InterfaceType == GXDLMS_INTERFACETYPE_NET)) //If data is sent packet.
	{
		unsigned long tmp = serverID;
		serverID = clientID;
		clientID = tmp;
		tmp = serverSize;
		serverSize = clientSize;
		clientSize = tmp;
	}
	//Check that client addresses match.
	if (memcmp(pBuff + pos1, &clientID, clientSize) != 0)
	{
		//If echo.
		if (m_InterfaceType != GXDLMS_INTERFACETYPE_NET && FrameLen != 0)
		{
			//Check that client addresses match.
			if (memcmp(pBuff + pos1, &serverID, serverSize) == 0 ||
			//Check that server addresses match.
				memcmp(pBuff + pos1 + serverSize, &clientID, clientSize) == 0)
			{
				dataSize -= FrameLen + pos1 - 1;
				if (dataSize > 5)
				{
					return GetDataFromHDLCFrame(pBuff + FrameLen + pos1 -1, dataSize, FrameType, pData, DataLen, cmd, MoreData, bReply, pNextIndex, IncludeHeader, bCheckCRC, pError);
				}
			}
		}
		return ERROR_CODES_CLIENT_ADDRESS_NO_NOT_MATCH;
	}	
	pos1 += clientSize;
	//Check that server addresses match.
	if (memcmp(pBuff + pos1, &serverID, serverSize) != 0)
	{
		return ERROR_CODES_SERVER_ADDRESS_NO_NOT_MATCH;
	}	
	pos1 += serverSize;
	/////////////////////////////////
	if (m_InterfaceType != GXDLMS_INTERFACETYPE_NET)
	{
		//Get frame type.
		FrameType = pBuff[pos1++];
		//If server has left.
		if (FrameType == DLMS_FRAME_TYPE_DISCONNECT_MODE ||
			FrameType == DLMS_FRAME_TYPE_REJECTED)
		{
			cmd = DLMS_COMMAND_REJECTED;
			return ERROR_CODES_OK;
		}
		 if (FrameType == DLMS_COMMAND_SNRM ||
            FrameType == DLMS_COMMAND_DISCONNECT_REQUEST)
        {
            //Check that CRC match.
            int crc = CountCRC(pBuff, PacketStartID + 1, pos1 - PacketStartID - 1);                                        
            if (memcmp((pBuff + pos1), &crc, 2) != 0)
            {
                return ERROR_CODES_WRONG_CRC;
            }
            if (FrameType == DLMS_COMMAND_SNRM)
            {
                cmd = DLMS_COMMAND_SNRM;
                return ERROR_CODES_OK;
            }
            if (FrameType == DLMS_COMMAND_DISCONNECT_REQUEST)
            {
                cmd = DLMS_COMMAND_DISCONNECT_REQUEST;
                return ERROR_CODES_OK;
            }
        }        
		//Check that header crc is corrent.
		unsigned int crc = CountCRC(pBuff, PacketStartID + 1, pos1 - PacketStartID - 1);	
		if (memcmp((pBuff + pos1), &crc, 2) != 0)
		{
			//Do nothing because Actaris is counting wrong CRC to the header.
		}
		pos1 += 2;
		//Check that CRC match.
		crc = CountCRC(pBuff + PacketStartID, 1, dataSize - PacketStartID - 4);
		if (bCheckCRC && memcmp((pBuff + dataSize - PacketStartID - 3), &crc, 2) != 0)
		{
			return ERROR_CODES_WRONG_CRC;
		}
		if (FrameType != DLMS_FRAME_TYPE_UA && FrameType != DLMS_FRAME_TYPE_SNRM)
		{
			if ((FrameType & 0x1) == 1)//If Keep Alive or get next frame
			{
				MoreData = GXDLMS_DATA_REQUEST_TYPES_FRAME;
				ret = ERROR_CODES_FALSE;
			}
			else
			{
				ret = CheckLLCBytes(pBuff, pos1, DataLen);
				if (ret > ERROR_CODES_OK)
				{
					return ret;
				}
			}
			if (ret == ERROR_CODES_OK) //CheckLLCBytes returns ERROR_CODES_FALSE if LLC bytes are not used.
			{
				if ((ret = GetData(pBuff, pos1, dataSize, bReply, cmd, MoreData, pError)) != 0)
				{
					return ret;
				}				
			}
		}
		//Skip data header and data CRC and EOP.
		DataLen = dataSize - pos1;
		if (bCheckCRC && DataLen > 1) //Remove CRC and EOP if added.
		{
			DataLen -= 3;
		}		
	}
	else
	{
		GXHelpers::ChangeByteOrder(&DataLen, pBuff + pos1, 2);
		pos1 += 2;
		if (DataLen + pos1 > dataSize) //If frame is not read complete.
		{
			return ERROR_CODES_OUTOFMEMORY;
		}
		/*
		// IEC62056-53 Sections 8.3 and 8.6.1
		// If Get.Response.Normal.
		unsigned char res = pBuff[pos1];
		if (bReply && res != 0x61)
		{
			//If meter returns exception.
            if (res == 0xD8)
            {
				printf("Meter returns exception.\r\n");
				return ERROR_CODES_INVALID_RESPONSE;
            }
			if ((ret = GetData(pBuff, pos1, dataSize, bReply, cmd, MoreData, pError)) != 0)
			{
				return ret;
			}			
		}
		DataLen = dataSize - pos1;
		*/
		if (DataLen != 0)
        {
            // IEC62056-53 Sections 8.3 and 8.6.1
            // If Get.Response.Normal.
            cmd = (DLMS_COMMAND) pBuff[pos1];                   
            //If chiphering is used.
            if (cmd == DLMS_COMMAND_GLO_GET_REQUEST ||
                cmd == DLMS_COMMAND_GLO_GET_RESPONSE ||
                cmd == DLMS_COMMAND_GLO_SET_REQUEST ||
                cmd == DLMS_COMMAND_GLO_SET_RESPONSE ||
                cmd == DLMS_COMMAND_GLO_METHOD_REQUEST ||
                cmd == DLMS_COMMAND_GLO_METHOD_RESPONSE ||
                cmd == DLMS_COMMAND_AARQ ||
				cmd == DLMS_COMMAND_DISCONNECT_RESPONSE ||
				cmd == DLMS_COMMAND_DISCONNECT_RESPONSE)
            {
            }
            else if (bReply && cmd != 0x61 && cmd != 0x60)
            {
				//If meter returns exception.
				if (cmd == 0xD8)
				{
					printf("Meter returns exception.\r\n");
					return ERROR_CODES_INVALID_RESPONSE;
				}
                if ((ret = GetData(pBuff, pos1, dataSize, bReply, cmd, MoreData, pError)) != 0)
				{
					return ret;
				}
            }
        }
		DataLen = dataSize - pos1;
	}
	
	//Set data length	
	//Save data.	
	if (DataLen > 0)
	{
		pData = new unsigned char[DataLen];
		memcpy(pData, pBuff + pos1, DataLen);
	}
	else
	{
		DataLen = 0;
		pData = NULL;
	}
	return ERROR_CODES_OK;
}

int CGXDLMS::AddFrame(int Type, bool MoreFrames, vector<unsigned char>& Data, int Index, int Count, vector< vector<unsigned char> >& Packets)
{	
	if (Index < 0 || Count < 0)
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}
	int ret = CheckInit();
	if (ret != 0)
	{
		return ret;
	}
	//Set packet size. BOP + Data size + dest and source size + EOP.
	int clientSize = ClientID.GetSize();
	int serverSize = ServerID.GetSize();
	int len = 7 + clientSize + serverSize;
	//If data is added. CRC is count for HDLC frame.
    if (Count > 0)
    {
        len += Count + 2;
    }
	std::vector<unsigned char> buff(0, len);	
	if (m_InterfaceType == GXDLMS_INTERFACETYPE_NET)
	{
		//Client or server address is not given in right format.
		//Like this:
		// CGXDLMSClient cl(true, (unsigned short) 3, (unsigned short) 23, GXDLMS_AUTHENTICATION_NONE, NULL, GXDLMS_INTERFACETYPE_NET);	
		if (clientSize != 2 || serverSize != 2)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}

		//Add version 0x0001
		unsigned short version = 1;
		GXHelpers::ChangeByteOrder(buff, &version, 2);
		if(m_Server)
		{
			//Add Destination (Server)		
			GXHelpers::ChangeByteOrder(buff, &ServerID.bVal, serverSize);
			//Add Source (Client)
			GXHelpers::ChangeByteOrder(buff, &ClientID.bVal, clientSize);		
		}
		else
		{
			//Add Source (Client)
			GXHelpers::ChangeByteOrder(buff, &ClientID.bVal, clientSize);		
			//Add Destination (Server)		
			GXHelpers::ChangeByteOrder(buff, &ServerID.bVal, serverSize);
		}
		//Add data length. 2 bytes.
		buff.push_back((unsigned char) (Count >> 8) & 0xFF);
		buff.push_back((Count & 0xFF));
		for(int pos = 0; pos != Count; ++pos)
		{
			buff.push_back(Data[Index + pos]);			
		}		
	}
	else
	{
		//HDLC frame opening flag.
		buff.push_back(HDLCFrameStartEnd);
		//Frame type 
		buff.push_back(MoreFrames ? 0xA8 : 0xA0);
		//Length of msg.
		buff.push_back((unsigned char) (len - 2));		
		if(m_Server)
		{
			//Client address		
			GXHelpers::ChangeByteOrder(buff, &ClientID.bVal, clientSize);		
			//Server address
			GXHelpers::ChangeByteOrder(buff, &ServerID.bVal, serverSize);		
		}
		else
		{
			//Server address
			GXHelpers::ChangeByteOrder(buff, &ServerID.bVal, serverSize);		
			//Client address		
			GXHelpers::ChangeByteOrder(buff, &ClientID.bVal, clientSize);		
		}
		//Add DLMS frame type
		buff.push_back(Type & 0xFF);
		//Count CRC for header.
		unsigned int crc = 0;
		if (Count > 0)
		{
			crc = CountCRC(buff, 1, buff.size() - 1);		
			buff.push_back(crc & 0xFF);
			buff.push_back((crc >> 8) & 0xFF);
			for(int pos = 0; pos != Count; ++pos)
			{
				buff.push_back(Data[Index + pos]);
			}
		}
		//Count CRC for HDLC frame.
		crc = CountCRC(buff, 1, buff.size() - 1);		
		buff.push_back(crc & 0xFF);
		buff.push_back((crc >> 8) & 0xFF);
		//EOP
		buff.push_back(HDLCFrameStartEnd);
	}
	Packets.push_back(buff);
	return ERROR_CODES_OK;
}

// SN referencing
int CGXDLMS::ParseSNObjects(unsigned char* pBuff, int len, CGXDLMSObjectCollection& objects)
{	
	objects.clear();
	int ret;
	//Get array tag.
	int pos = 0;
	unsigned char size = pBuff[pos++];
	//Check that data is in the array
	if (size != 0x01)
	{
		return ERROR_CODES_INVALID_RESPONSE;
	}
	//get object count
	int count = CGXOBISTemplate::GetObjectCount(pBuff, pos);
	pBuff += pos;
	len -= pos;	
	for(int objPos = 0; objPos != count; ++objPos)
	{
		CGXDLMSVariant value;
		if ((ret = CGXOBISTemplate::GetData(pBuff, len, DLMS_DATA_TYPE_NONE, value)) != 0)
		{
			return ret;
		}
		if (value.vt != DLMS_DATA_TYPE_STRUCTURE || value.Arr.size() != 4)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}		
		if (value.Arr[0].vt != DLMS_DATA_TYPE_INT16 ||
			value.Arr[1].vt != DLMS_DATA_TYPE_UINT16 ||
			value.Arr[2].vt != DLMS_DATA_TYPE_UINT8 ||
			value.Arr[3].vt != DLMS_DATA_TYPE_OCTET_STRING)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}
		short sn = value.Arr[0].ToInteger();
		unsigned short class_id = (unsigned short) value.Arr[1].ToInteger();
		unsigned char version = (unsigned char) value.Arr[2].ToInteger();
		CGXDLMSVariant ln = value.Arr[3];
		CGXDLMSObject* pObj = CGXDLMSObjectFactory::CreateObject((OBJECT_TYPE) class_id);
		pObj->m_SN = sn;
		pObj->SetVersion(version);
		int cnt = ln.byteArr.size();
		assert(cnt == 6);
		for(int pos = 0; pos != cnt; ++pos)
		{
			pObj->m_LN[pos] = *(ln.byteArr.begin() + pos);
		}

		objects.push_back(pObj);
		/*
		if (class_id == OBJECT_TYPE_PROFILE_GENERIC)
		{
			objects.push_back(new CGXDLMSObject(sn, class_id, version, ln.byteArr));
		}
		else
		{
			objects.push_back(new CGXDLMSObject(sn, class_id, version, ln.byteArr));
		}
		*/
	}
	return 0;
}

// LN referencing
int CGXDLMS::ParseLNObjects(unsigned char* pBuff, int len, CGXDLMSObjectCollection& objects)
{
	objects.clear();
	int ret;
	//Get array tag.
	int pos = 0;
	unsigned char size = pBuff[pos++];
	//Check that data is in the array
	if (size != DLMS_DATA_TYPE_ARRAY)
	{
		return ERROR_CODES_INVALID_RESPONSE;
	}
	//get object count
	int count = CGXOBISTemplate::GetObjectCount(pBuff, pos);	
	pBuff += pos;
	len -= pos;	
	for(int objPos = 0; objPos != count; ++objPos)
	{
		CGXDLMSVariant value;
		if ((ret = CGXOBISTemplate::GetData(pBuff, len, DLMS_DATA_TYPE_NONE, value)) != 0)
		{
			//This fix Iskraemeco (MT-880) bug.
			if (ret == ERROR_CODES_OUTOFMEMORY)
			{
				break;
			}
			return ret;
		}
		if (value.vt != DLMS_DATA_TYPE_STRUCTURE || value.Arr.size() != 4)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}
		if (value.Arr[0].vt != DLMS_DATA_TYPE_UINT16)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}
		unsigned short class_id = value.Arr[0].ToInteger();
		if (value.Arr[1].vt != DLMS_DATA_TYPE_UINT8)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}
		unsigned char version = value.Arr[1].ToInteger();

		if (value.Arr[2].vt != DLMS_DATA_TYPE_OCTET_STRING)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}
		CGXDLMSVariant ln = value.Arr[2];
		//Get Access rights...
		if (value.Arr[3].vt != DLMS_DATA_TYPE_STRUCTURE || value.Arr[3].Arr.size() != 2)
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}
		CGXDLMSObject* pObj = CGXDLMSObjectFactory::CreateObject((OBJECT_TYPE) class_id);
		pObj->SetVersion(version);
		int cnt;
		// attribute_access_descriptor Start
		if (value.Arr[3].Arr[0].vt != DLMS_DATA_TYPE_ARRAY)
		{
			delete pObj;
			return ERROR_CODES_INVALID_PARAMETER;
		}		
		for(unsigned int pos = 0; pos != value.Arr[3].Arr[0].Arr.size(); ++pos)
		{			
			if (value.Arr[3].Arr[0].Arr[pos].vt != DLMS_DATA_TYPE_STRUCTURE || 
				value.Arr[3].Arr[0].Arr[pos].Arr.size() != 3)
			{
				delete pObj;
				return ERROR_CODES_INVALID_PARAMETER;
			}
			int id = value.Arr[3].Arr[0].Arr[pos].Arr[0].ToInteger();
			//Get access_mode			
			if (value.Arr[3].Arr[0].Arr[pos].Arr[1].vt != DLMS_DATA_TYPE_ENUM)
			{
				delete pObj;
				return ERROR_CODES_INVALID_PARAMETER;
			}			
			pObj->SetAccess(id, (ACCESSMODE) value.Arr[3].Arr[0].Arr[pos].Arr[1].ToInteger());
			//Get access_selectors			
			if (value.Arr[3].Arr[0].Arr[pos].Arr[2].vt == DLMS_DATA_TYPE_ARRAY)
			{
				int cnt2 = value.Arr[3].Arr[0].Arr[pos].Arr[2].Arr.size();
				for(int pos2 = 0; pos2 != cnt2; ++pos2)
				{
					//Get access_mode
				}
			}
			else if (value.Arr[3].Arr[0].Arr[pos].Arr[2].vt != DLMS_DATA_TYPE_NONE)
			{
				delete pObj;
				return ERROR_CODES_INVALID_PARAMETER;
			}
		}	
		// attribute_access_descriptor End
		// method_access_item Start				
		if (value.Arr[3].Arr[1].vt != DLMS_DATA_TYPE_ARRAY)
		{
			delete pObj;
			return ERROR_CODES_INVALID_PARAMETER;
		}
		for(unsigned int pos = 0; pos != value.Arr[3].Arr[1].Arr.size(); ++pos)
		{
			CGXDLMSVariant tmp = value.Arr[3].Arr[1].Arr[pos];
			if (tmp.vt != DLMS_DATA_TYPE_STRUCTURE || 
				tmp.Arr.size() != 2)
			{
				delete pObj;
				return ERROR_CODES_INVALID_PARAMETER;
			}
			int id = tmp.Arr[0].ToInteger();
			//Get access_mode			
			//In version 0 data type is boolean.
			if (tmp.Arr[1].vt != DLMS_DATA_TYPE_ENUM && tmp.Arr[1].vt != DLMS_DATA_TYPE_BOOLEAN)
			{
				delete pObj;
				return ERROR_CODES_INVALID_PARAMETER;
			}			
			pObj->SetMethodAccess(id, (METHOD_ACCESSMODE) tmp.Arr[1].ToInteger());			
		}	
		// method_access_item End
		cnt = ln.byteArr.size();
		assert(cnt == 6);
		for(int pos = 0; pos != cnt; ++pos)
		{
			pObj->m_LN[pos] = *(ln.byteArr.begin() + pos);
		}
		objects.push_back(pObj);
	}	
	return 0;
}

int CGXDLMS::SplitToFrames(vector<unsigned char>& Data, unsigned int blockIndex, unsigned int& index, unsigned int count, DLMS_COMMAND Cmd, vector< vector<unsigned char> >& Packets)
{    
	vector<unsigned char> tmp(0, count + 13);
    if (m_InterfaceType == GXDLMS_INTERFACETYPE_GENERAL)
    {
        if (m_Server)
        {
			GXHelpers::AddRange(tmp, LLCReplyBytes, 3);
        }
        else
        {
			GXHelpers::AddRange(tmp, LLCSendBytes, 3);
        }
    }
    if (Cmd != DLMS_COMMAND_NONE && m_UseLogicalNameReferencing)
    {
		bool moreBlocks = Data.size() > m_MaxReceivePDUSize && Data.size() > index + count;
        //Command, multible blocks and Invoke ID and priority.
		tmp.push_back(Cmd);
		tmp.push_back(moreBlocks ? 2 : 1);
		tmp.push_back(0x81);
        if (m_Server)
        {
			tmp.push_back(0x0); // Get-Data-Result choice data
        }
        if (moreBlocks)
        {
			GXHelpers::SetUInt32(blockIndex, tmp);
            tmp.push_back(0);
			CGXOBISTemplate::SetObjectCount(count, tmp);
        }
    }    
    unsigned int dataSize;
    if (m_InterfaceType == GXDLMS_INTERFACETYPE_NET)
    {
        dataSize = m_MaxReceivePDUSize;
    }
    else
    {
		CGXDLMSVariant value;
        if (Cmd == DLMS_COMMAND_GET_REQUEST || Cmd == DLMS_COMMAND_METHOD_REQUEST || Cmd == DLMS_COMMAND_READ_REQUEST || 
            Cmd == DLMS_COMMAND_SET_REQUEST || Cmd == DLMS_COMMAND_WRITE_REQUEST)
        {
			value = m_Limits.GetMaxInfoTX();
			value.ChangeType(DLMS_DATA_TYPE_INT32);
            dataSize = value.lVal;
        }
        else
        {
			value = m_Limits.GetMaxInfoRX();
			value.ChangeType(DLMS_DATA_TYPE_INT32);
            dataSize = value.lVal;
        }
    }
	if (count + index > Data.size())
    {
        count = Data.size() - index;
    }		
	GXHelpers::AddRange(tmp, Data, index, count);
	index += count;
    count = tmp.size();
    if (count < dataSize)
    {
        dataSize = count;
    }            
    unsigned int cnt = (unsigned int)(count / dataSize);
	if (count % dataSize != 0)
	{
		++cnt;
	}
	int start = 0;
    for (unsigned int pos = 0; pos < cnt; ++pos)
    {
        unsigned char id = 0;
		if (m_InterfaceType == GXDLMS_INTERFACETYPE_GENERAL)
		{
			if (pos == 0)
			{
				id = GenerateIFrame();
			}
			else
			{
				id = GenerateNextFrame();
			}
		}
        if (start + dataSize > tmp.size())
        {
            dataSize = tmp.size() - start;
        } 
        AddFrame(id, cnt != 1 && pos < cnt - 1, tmp, start, dataSize, Packets);
        start += dataSize;
    } 
	return 0;
}

// Split the send packet to a size that the device can handle.
int CGXDLMS::SplitToBlocks(vector<unsigned char>& Data, DLMS_COMMAND Cmd, vector< vector<unsigned char> >& Packets)
{
    unsigned int index = 0;
    if (!m_UseLogicalNameReferencing)//SN
    {                
        return SplitToFrames(Data, 0, index, Data.size(), Cmd, Packets);
    }     
    //If LN           
    //Split to Blocks.
    unsigned int blockIndex = 0;
    do
    {
		int packCount = Packets.size();
        SplitToFrames(Data, ++blockIndex, index, m_MaxReceivePDUSize, Cmd, Packets);
        if (m_InterfaceType == GXDLMS_INTERFACETYPE_GENERAL && (Packets.size() - packCount) != 1)
        {
            m_ExpectedFrame += 3;
        }
    }
    while (index < (Data.size() - 1));
	return ERROR_CODES_OK;
}

int CGXDLMS::IsReplyPacket(unsigned char* SendData, int SendbuffSize, unsigned char* ReceivedData, int ReceivedbuffSize)
{
	int ret = 0;
	//Check if Echo
	if (SendData != NULL && ReceivedData != NULL && SendbuffSize == ReceivedbuffSize)
	{
		//If echo.
		if (memcmp(SendData, ReceivedData, SendbuffSize) == 0)
		{
			return ERROR_CODES_FALSE;		
		}
	}	
	int size = 0;
	unsigned char* pSendBuff = NULL;
	unsigned char* pReceivedBuff = NULL;
	unsigned char SendID = 0, ReceivedID = 0;
	DLMS_COMMAND cmd;
	GXDLMS_DATA_REQUEST_TYPES MoreData = GXDLMS_DATA_REQUEST_TYPES_NONE;
	if ((ret = GetDataFromHDLCFrame(SendData, SendbuffSize, SendID, pSendBuff, size, cmd, MoreData, false)) == 0 &&
		(ret = GetDataFromHDLCFrame(ReceivedData, ReceivedbuffSize, ReceivedID, pReceivedBuff, size, cmd, MoreData, true)) == 0)
	{
		if (pSendBuff != NULL)
		{
			delete pSendBuff;
			pSendBuff = NULL;
		}
		if (pReceivedBuff != NULL)
		{
			delete pReceivedBuff;
			pReceivedBuff = NULL;
		}

		if (cmd == DLMS_COMMAND_REJECTED)
		{
			return ERROR_CODES_FRAME_REJECTED;
		}
		if (ReceivedID == DLMS_FRAME_TYPE_REJECTED || (SendID == DLMS_FRAME_TYPE_DISCONNECT && ReceivedID == DLMS_FRAME_TYPE_UA) || IsExpectedFrame(SendID, ReceivedID))
		{
			ret = ERROR_CODES_OK;
		}
		else
		{
			ret = ERROR_CODES_FALSE;
		}
	}	
	if (pSendBuff != NULL)
	{
		delete pSendBuff;
	}
	if (pReceivedBuff != NULL)
	{
		delete pReceivedBuff;
	}
	return ret;
}

int CGXDLMS::GetActionInfo(OBJECT_TYPE objectType, int& value, int& count)
{        
    switch (objectType)
    {
        case OBJECT_TYPE_DATA:
        case OBJECT_TYPE_ACTION_SCHEDULE:
        case OBJECT_TYPE_ALL:
        case OBJECT_TYPE_AUTO_ANSWER:
        case OBJECT_TYPE_AUTO_CONNECT:
        case OBJECT_TYPE_MAC_ADDRESS_SETUP:
        case OBJECT_TYPE_EVENT:
        case OBJECT_TYPE_GPRS_SETUP:
        case OBJECT_TYPE_IEC_HDLC_SETUP:
        case OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        case OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        case OBJECT_TYPE_MODEM_CONFIGURATION:
        case OBJECT_TYPE_PPP_SETUP:
        case OBJECT_TYPE_REGISTER_MONITOR:
        case OBJECT_TYPE_REMOTE_ANALOGUE_CONTROL:
        case OBJECT_TYPE_REMOTE_DIGITAL_CONTROL:
        case OBJECT_TYPE_SCHEDULE:
        case OBJECT_TYPE_SMTP_SETUP:
        case OBJECT_TYPE_STATUS_MAPPING:
        case OBJECT_TYPE_TCP_UDP_SETUP:
        case OBJECT_TYPE_TUNNEL:
        case OBJECT_TYPE_UTILITY_TABLES:
            return ERROR_CODES_INVALID_PARAMETER;
        case OBJECT_TYPE_ACTIVITY_CALENDAR:
            value = 0x50;
            count = 1;
            break;
        case OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
            value = 0x60;
            count = 4;
            break;
        case OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
            value = 0x20;
            count = 8;
            break;
        case OBJECT_TYPE_CLOCK:
            value = 0x60;
            count = 6;
            break;
        case OBJECT_TYPE_DEMAND_REGISTER:
            value = 0x48;
            count = 2;
            break;
        case OBJECT_TYPE_EXTENDED_REGISTER:
            value = 0x38;
            count = 1;
            break;
        case OBJECT_TYPE_IP4_SETUP:
            value = 0x60;
            count = 3;
            break;
        case OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
            value = 0x60;
            count = 8;
            break;
        case OBJECT_TYPE_PROFILE_GENERIC:
            value = 0x58;
            count = 4;
            break;
        case OBJECT_TYPE_REGISTER:
            value = 0x28;
            count = 1;
            break;
        case OBJECT_TYPE_REGISTER_ACTIVATION:
            value = 0x30;
            count = 3;
            break;
        case OBJECT_TYPE_REGISTER_TABLE:
            value = 0x28;
            count = 2;
            break;
        case OBJECT_TYPE_SAP_ASSIGNMENT:
        case OBJECT_TYPE_SCRIPT_TABLE:
            value = 0x20;
            count = 1;
            break;
        case OBJECT_TYPE_SPECIAL_DAYS_TABLE:
            value = 0x10;
            count = 2;
            break;
        default:     
            count = value = 0;
            break;
    }
	return ERROR_CODES_OK;
}