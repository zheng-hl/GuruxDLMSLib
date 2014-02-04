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

#include "GXAPDU.h"
#include "GXDLMSServerBase.h"
#include "GXOBISTemplate.h"
#include "Objects/GXDLMSAssociationLogicalName.h"
#include "Objects/GXDLMSAssociationShortName.h"
#include "ManufacturerSettings/GXAttributeCollection.h"
#include "GXDLMS.h"

CGXDLMSServerBase::CGXDLMSServerBase(bool UseLogicalNameReferencing,
				GXDLMS_INTERFACETYPE IntefaceType, unsigned short MaxReceivePDUSize) : m_Base(true), m_Initialized(false), m_FrameIndex(0)
{
	m_Base.m_UseLogicalNameReferencing = UseLogicalNameReferencing;
	m_Base.m_InterfaceType = IntefaceType;
	m_Base.m_MaxReceivePDUSize = MaxReceivePDUSize;
    Reset();            
    m_Base.m_pLNSettings = new CGXDLMSLNSettings();
    m_Base.m_pSNSettings = new CGXDLMSSNSettings();
	if (IntefaceType == GXDLMS_INTERFACETYPE_GENERAL)
	{
		m_Authentications.push_back(new GXAuthentication(GXDLMS_AUTHENTICATION_NONE, "", (unsigned char) 0x10));
		m_Authentications.push_back(new GXAuthentication(GXDLMS_AUTHENTICATION_LOW, "GuruxLow", (unsigned char) 0x20));
		m_Authentications.push_back(new GXAuthentication(GXDLMS_AUTHENTICATION_HIGH, "GuruxHigh", (unsigned char) 0x40));
		CGXDLMSVariant value;
		CountServerID((unsigned char)1, 0, value);
		m_ServerIDs.push_back(value);
	}
	else
	{
		m_Authentications.push_back(new GXAuthentication(GXDLMS_AUTHENTICATION_NONE, "", (unsigned short) 0x10));
		m_Authentications.push_back(new GXAuthentication(GXDLMS_AUTHENTICATION_LOW, "GuruxLow", (unsigned short) 0x20));
		m_Authentications.push_back(new GXAuthentication(GXDLMS_AUTHENTICATION_HIGH, "GuruxHigh", (unsigned short) 0x40));
		CGXDLMSVariant value = (unsigned short) 1;
		m_ServerIDs.push_back(value);
	}
}

//Is Logican Name referencing used.
bool CGXDLMSServerBase::GetUseLogicalNameReferencing()
{
	return m_Base.m_UseLogicalNameReferencing;
}

//Get Interface type.
GXDLMS_INTERFACETYPE CGXDLMSServerBase::GetInterfaceType()
{
	return m_Base.m_InterfaceType;
}

//Get Max Receive PDU Size.
unsigned short CGXDLMSServerBase::GetMaxReceivePDUSize()
{
	return m_Base.m_MaxReceivePDUSize;
}

//Get Server ID.
CGXDLMSVariant CGXDLMSServerBase::GetServerID()
{
	return m_Base.ServerID;
}

//Set Server ID.
void CGXDLMSServerBase::SetServerID(CGXDLMSVariant& value)
{
	m_Base.ServerID = value;
}

void CGXDLMSServerBase::Reset()
{
	m_Base.ServerID.Clear();
	m_Base.ClientID.Clear();
}

CGXDLMSServerBase::~CGXDLMSServerBase(void)
{
}

int CGXDLMSServerBase::CountServerID(CGXDLMSVariant physicalAddress, int LogicalAddress, CGXDLMSVariant& value)
{
	value.Clear();
    if (m_Base.m_InterfaceType == GXDLMS_INTERFACETYPE_NET)
    {
		value = (unsigned short) (LogicalAddress << 9 | physicalAddress.uiVal);
		return ERROR_CODES_OK;
    }	
	if (physicalAddress.vt == DLMS_DATA_TYPE_UINT8)
    {
        value.bVal = ((LogicalAddress & 0x7) << 5) | ((physicalAddress.bVal & 0x7) << 1) | 0x1;
        value.vt = DLMS_DATA_TYPE_UINT8;
    }
    else if (physicalAddress.vt == DLMS_DATA_TYPE_UINT16)
    {
        int physicalID = physicalAddress.uiVal;
        int logicalID = LogicalAddress;
        int total = (physicalID & 0x7F) << 1 | 1;
        value.uiVal = total | (logicalID << 9);
        value.vt = DLMS_DATA_TYPE_UINT16;
    }
    else if (physicalAddress.vt == DLMS_DATA_TYPE_UINT32)
    {
        int physicalID = physicalAddress.ulVal;
        int logicalID = LogicalAddress;
        int total = (((physicalID >> 7) & 0x7F) << 8) | (physicalID & 0x7F);
        value.ulVal = (total << 1) | 1 | (logicalID << 17);
		value.vt = DLMS_DATA_TYPE_UINT32;
    }
    else
    {
        return ERROR_CODES_INVALID_PARAMETER;
    }
    return ERROR_CODES_OK;
}

int CGXDLMSServerBase::Initialize()
{
    if (m_Authentications.size() == 0)
    {
		//Authentication is not set.
        return ERROR_CODES_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED;
    }    
    m_Initialized = true;
	bool association = false;
    if (m_SortedItems.size() != m_Items.size())
    {
		string ln;
        for (unsigned long pos = 0; pos != m_Items.size(); ++pos)
        {                    
			CGXDLMSObject* it = m_Items.at(pos);
			it->GetLogicalName(ln);
			if (strcmp(ln.c_str(), "0.0.0.0.0.0") == 0)
            {
                return ERROR_CODES_INVALID_LOGICAL_NAME;
            }                    					
			OBJECT_TYPE type = it->GetObjectType();
			if ((type == OBJECT_TYPE_ASSOCIATION_SHORT_NAME && !m_Base.GetUseLogicalNameReferencing()) ||
                (type == OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME && m_Base.GetUseLogicalNameReferencing()))
            {
                association = true;
            }            			
        }		
        if (!association)
        {
            if (m_Base.GetUseLogicalNameReferencing())
            {                
				CGXDLMSAssociationLogicalName* pLn = new CGXDLMSAssociationLogicalName();
				for (CGXDLMSObjectCollection::iterator it = m_Items.begin(); it != m_Items.end(); ++it)
				{
					pLn->GetObjectList().push_back(*it);
				}				
				m_Items.push_back(pLn);
            }
            else
            {
				CGXDLMSAssociationShortName* pSn = new CGXDLMSAssociationShortName();
				for (CGXDLMSObjectCollection::iterator it = m_Items.begin(); it != m_Items.end(); ++it)
				{
					pSn->GetObjectList().push_back(*it);
				}				
                m_Items.push_back(pSn);
            }
        }
        //Arrange items by Short Name.
        unsigned short sn = 0xA0;
        if (!m_Base.GetUseLogicalNameReferencing())
        {			
            m_SortedItems.clear();
			for (CGXDLMSObjectCollection::iterator it = m_Items.begin(); it != m_Items.end(); ++it)
			{
				unsigned short itemSN = (*it)->GetShortName();				
                //Generate Short Name if not given.
                if (itemSN == 0)
                {
                    do
                    {
						(*it)->SetShortName(sn);
						itemSN = sn;
                        sn += 0xA0;
                    }
					while (m_SortedItems[itemSN] != NULL);
                }
                m_SortedItems[itemSN] = *it;				
			}
        }
    }
	return ERROR_CODES_OK;
}

// Reserved for internal use.
int CGXDLMSServerBase::GetAddress(unsigned char* pBuff, int dataSize, CGXDLMSVariant& clientId, CGXDLMSVariant& serverId)
{
	clientId.Clear();
	int serverSize = serverId.GetSize();
	if (pBuff == NULL || dataSize < 1 || serverSize < 1)
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}
    int PacketStartID = 0;
    int FrameLen = 0;
	int pos1 = 0;
	unsigned char FrameType;
	GXDLMS_DATA_REQUEST_TYPES MoreData = GXDLMS_DATA_REQUEST_TYPES_NONE;
    //If DLMS frame is generated.
    if (m_Base.m_InterfaceType != GXDLMS_INTERFACETYPE_NET)
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
		//If not enough data.
		FrameLen += pBuff[pos1++];
		if (dataSize - pos1 + 2 < FrameLen)
		{
			// Not enought data to parse %d(%d)", len, FrameLen + 2);
			return ERROR_CODES_OUTOFMEMORY;
		}
		if (MoreData == GXDLMS_DATA_REQUEST_TYPES_NONE && 
			pBuff[FrameLen + PacketStartID + 1] != HDLCFrameStartEnd)
		{
			return ERROR_CODES_INVALID_DATA_FORMAT;
		}		
		GXHelpers::ChangeByteOrder(&serverId.bVal, pBuff + pos1, serverSize);
		pos1 += serverSize;
		GXHelpers::ChangeByteOrder(&clientId.bVal, pBuff + pos1, 1);//Client address is always one byte.
		clientId.vt = DLMS_DATA_TYPE_UINT8;
		pos1 += 1;
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
		GXHelpers::ChangeByteOrder(&clientId.uiVal, pBuff + pos1, 2);
		clientId.vt = DLMS_DATA_TYPE_UINT16;
		pos1 += 2;
		serverId.Clear();
		GXHelpers::ChangeByteOrder(&serverId.uiVal, pBuff + pos1, 2);
		serverId.vt = DLMS_DATA_TYPE_UINT16;
        pos1 += 2;
    }
    return ERROR_CODES_OK;
}

int CGXDLMSServerBase::HandleRequest(vector<unsigned char>& data, unsigned char*& pReply, int& ReplySize)
{
	return HandleRequest(&data[0], data.size(), pReply, ReplySize);
}

int CGXDLMSServerBase::HandleRequest(unsigned char* pData, int size, unsigned char*& pReply, int& ReplySize)
{
	printf("CGXDLMSServerBase::HandleRequest\r\n");
	if (pData == NULL || size < 1)
	{
		printf("CGXDLMSServerBase::HandleRequest failed. Invalid parameter.\r\n");
		return ERROR_CODES_INVALID_PARAMETER;
	}
	pReply = NULL;
	ReplySize = 0;
    if (m_ReceivedData.size() != 0)
    {
		GXHelpers::AddRange(m_ReceivedData, pData, size);
		pData = &(m_ReceivedData[0]);
		size += m_ReceivedData.size();
    }

	if (m_Base.ServerID.vt == DLMS_DATA_TYPE_NONE)
    {
		CGXDLMSVariant sid, cid;
		for(vector<CGXDLMSVariant>::iterator it = m_ServerIDs.begin(); it != m_ServerIDs.end(); ++it)
        {
			sid = *it;
            if (GetAddress(pData, size, cid, sid) == ERROR_CODES_OK)
            {
                if (sid.Equals(*it))
                {
                	m_Base.ServerID = sid;
                    m_Base.ClientID = cid;                    
                    Initialize();
                    break;
                }
            }
        }
        //We do not communicate if server ID not found.
        if (m_Base.ServerID.vt == DLMS_DATA_TYPE_NONE)
        {
        	printf("CGXDLMSServerBase::HandleRequest failed. Invalid connection.\r\n");
        	return OnInvalidConnection();
        }
    }
	if (!m_Initialized)
	{
		printf("CGXDLMSServerBase::HandleRequest failed. Not initialize.\r\n");
		return ERROR_CODES_NOT_INITIALIZED;
	}
	if (!m_Base.IsDLMSPacketComplete(pData, size))
    {		
        if (m_ReceivedData.size() == 0)
        {
			GXHelpers::AddRange(m_ReceivedData, pData, size);
        }
        printf("CGXDLMSServerBase::HandleRequest failed. Wait more data.\r\n");
        return ERROR_CODES_FALSE; //Wait more data.
    }
    CGXDLMSObject* pItem = NULL;
    unsigned char* allData = NULL;
	int DataSize = 0;
    //byte frame;
	DLMS_COMMAND cmd;
	GXDLMS_DATA_REQUEST_TYPES moreData;
    int ret = m_Base.GetDataFromPacket(pData, size, allData, DataSize, moreData, cmd);
	if (ret != ERROR_CODES_OK)
	{
		printf("CGXDLMSServerBase::HandleRequest failed. GetDataFromPacket failed %d.\r\n", ret);
		return ret;
	}	
    m_ReceivedData.clear();	
	//Ask next block.
    if (moreData == GXDLMS_DATA_REQUEST_TYPES_BLOCK)
    {
		++m_FrameIndex;
		unsigned long BlockIndex = 0;
		m_SendData.clear();
		GXHelpers::ChangeByteOrder(&BlockIndex, &allData, 4);			
		return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
    }
	//Ask next frame.
    else if ((moreData & GXDLMS_DATA_REQUEST_TYPES_FRAME) != 0)
    {            
		++m_FrameIndex;
        //Keep alive...
        if (m_FrameIndex >= m_SendData.size())
        {
            m_FrameIndex = 0;
			m_SendData.clear();
			vector<unsigned char> Data;
			m_Base.AddFrame(m_Base.GenerateAliveFrame(), false, Data, 0, 0, m_SendData);
            return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
        }
		return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);			
    }
	m_FrameIndex = 0;
	m_SendData.clear();	
	CGXDLMSVariant name;
	int index;
    if (cmd == DLMS_COMMAND_SNRM)
    {
    	printf("CGXDLMSServerBase::HandleRequest::SNRM.\r\n");
		HandleSNRMRequest(m_SendData);        
		return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
    }
	else if (cmd == DLMS_COMMAND_AARQ)
    {
		printf("CGXDLMSServerBase::HandleRequest::AARQ.\r\n");
        HandleAARQRequest(allData, DataSize, m_SendData);
		return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
    }
	else if (cmd == DLMS_COMMAND_DISCONNECT_REQUEST)
    {
		printf("CGXDLMSServerBase::HandleRequest::Disconnect.\r\n");
        GenerateDisconnectRequest(m_SendData);
        return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
    }
	else if (cmd == DLMS_COMMAND_SET_REQUEST ||
				cmd == DLMS_COMMAND_WRITE_REQUEST)
    {
		printf("CGXDLMSServerBase::HandleRequest::Write.\r\n");
		OBJECT_TYPE type;
		unsigned char* pParameter = NULL;
		int ParameterSize;
        GetCommand(allData, DataSize, type, name, index, pParameter, ParameterSize);
		if (cmd == DLMS_COMMAND_SET_REQUEST && GetUseLogicalNameReferencing())
		{
			printf("Reading %s, attribute index %d\r\n", name.strVal.c_str(), index);
			pItem = m_Items.FindByLN(type, name.ToString());
		}
		else if (cmd == DLMS_COMMAND_WRITE_REQUEST && !GetUseLogicalNameReferencing())
		{
			name.ChangeType(DLMS_DATA_TYPE_UINT16);
			unsigned short sn = name.uiVal;	
			for(std::map<unsigned short, CGXDLMSObject*>::iterator it = m_SortedItems.begin(); it != m_SortedItems.end(); ++it)
			{
				if (it->first > sn)
				{
					break;
				}
				pItem = it->second;
			}        
			index = ((sn - pItem->GetShortName()) / 8) + 1;
			printf("Reading %x, attribute index %d", pItem->GetShortName(), index);
		}
		else
		{
			//Return HW error.
			ServerReportError(1, 5, 3, m_SendData);
			return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
		}
        if (pItem != NULL)
        {
			CGXDLMSVariant value;
			DLMS_DATA_TYPE dt = DLMS_DATA_TYPE_NONE;
			if (ParameterSize != 0)
			{
				if (CGXOBISTemplate::GetData(pParameter, ParameterSize, dt, value) != 0)
				{
					//Return HW error.
					ServerReportError(1, 5, 3, m_SendData);
					return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
				}
			}
            if (value.vt == DLMS_DATA_TYPE_OCTET_STRING)
            {
                pItem->GetUIDataType(index, dt);
                if (dt != DLMS_DATA_TYPE_NONE)
                {
					if (value.ChangeType(dt) != 0)
					{
						//Return HW error.
						ServerReportError(1, 5, 3, m_SendData);
						return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
					}
                }
            }
			ret = OnWrite(pItem, index, value);
            if (ret == ERROR_CODES_OK)
			{
				if (Acknowledge(DLMS_COMMAND_METHOD_RESPONSE, 0, m_SendData) == ERROR_CODES_OK)
				{
					return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
				}
			}
			if (ret == ERROR_CODES_FALSE)
			{
				/* TODO:
				if (pItem->SetValue(index, value) == ERROR_CODES_OK)
				{
					if (Acknowledge(GetUseLogicalNameReferencing() ? DLMS_COMMAND_SET_RESPONSE : DLMS_COMMAND_WRITE_RESPONSE, 0, m_SendData) == ERROR_CODES_OK)
					{
						return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
					}
				}
				*/
			}
			//Error is handled later.
        }
	}
	else if (cmd == DLMS_COMMAND_GET_REQUEST || cmd == DLMS_COMMAND_READ_REQUEST)
	{
		OBJECT_TYPE type;
		unsigned char* pParameter = NULL;
		int ParameterSize;		
		GetCommand(allData, DataSize, type, name, index, pParameter, ParameterSize);	
		if (cmd == DLMS_COMMAND_GET_REQUEST && GetUseLogicalNameReferencing())
		{
			printf("CGXDLMSServerBase::HandleRequest::Get\r\n");
			printf("Reading %s, attribute index %d\r\n", name.strVal.c_str(), index);
			pItem = m_Items.FindByLN(type, name.ToString());
		}
		else if (cmd == DLMS_COMMAND_READ_REQUEST && !GetUseLogicalNameReferencing())
		{
			printf("CGXDLMSServerBase::HandleRequest::Read\r\n");
			name.ChangeType(DLMS_DATA_TYPE_UINT16);
			unsigned short sn = name.uiVal;	
			for(std::map<unsigned short, CGXDLMSObject*>::iterator it = m_SortedItems.begin(); it != m_SortedItems.end(); ++it)
			{
				if (it->first > sn)
				{
					break;
				}
				pItem = it->second;
			}        
			index = ((sn - pItem->GetShortName()) / 8) + 1;
			printf("Reading %x, attribute index %d", pItem->GetShortName(), index);	
		}
		if (pItem != NULL)
		{
			CGXDLMSVariant value;
			DLMS_DATA_TYPE tp = DLMS_DATA_TYPE_NONE;
			if ((ret = OnRead(pItem, index, value, tp)) == ERROR_CODES_OK)
			{
				if (tp == DLMS_DATA_TYPE_NONE)
				{
					tp = value.vt;
				}
				if ((ret = ReadReply(name, type, index, value, tp, m_SendData)) == 0)
				{
					return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
				}
			}
			if (ret != ERROR_CODES_FALSE)
			{
				printf("OnRead failed.\r\n");
				//Return HW error.
				ServerReportError(1, 5, 3, m_SendData);
				return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
			}
			if (GetValue(pItem, index, pParameter, ParameterSize, m_SendData) == 0)
			{
				return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
			}
		}
		else
		{
    		printf("Reading Failed %s, attribute index %d is unknown.\r\n", name.strVal.c_str(), index);
		}		
	}
	else if (cmd == DLMS_COMMAND_METHOD_REQUEST)
	{
		OBJECT_TYPE type;
		unsigned char* pParameter = NULL;
		int ParameterSize;		
        GetCommand(allData, DataSize, type, name, index, pParameter, ParameterSize);
		CGXDLMSVariant value;
		DLMS_DATA_TYPE dt = DLMS_DATA_TYPE_NONE;
		int ret = CGXOBISTemplate::GetData(pParameter, ParameterSize, dt, value);
		if (ret == ERROR_CODES_OK)
		{
			if (GetUseLogicalNameReferencing())
			{
				printf("Action %s, attribute index %d\r\n", name.strVal.c_str(), index);
				pItem = m_Items.FindByLN(type, name.ToString());
			}
			else
			{
				name.ChangeType(DLMS_DATA_TYPE_UINT16);
				unsigned short sn = name.uiVal;	
				for(std::map<unsigned short, CGXDLMSObject*>::iterator it = m_SortedItems.begin(); it != m_SortedItems.end(); ++it)
				{
					if (it->first > sn)
					{
						break;
					}
					pItem = it->second;
				}        				
				int value, count;
				CGXDLMS::GetActionInfo(pItem->GetObjectType(), value, count);
				index = ((sn - pItem->GetShortName()) / value);
				printf("Action %x, attribute index %d", pItem->GetShortName(), index);
			}
			if (pItem != NULL)
			{
				if ((ret = OnAction(pItem, index, value)) == ERROR_CODES_OK)
				{
					if (Acknowledge(DLMS_COMMAND_METHOD_RESPONSE, 0, m_SendData) == ERROR_CODES_OK)
					{
						return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
					}
				}
				if (ret == ERROR_CODES_FALSE)
				{
					IGXDLMSBase* pTmp = (IGXDLMSBase*) pItem;
					if (pTmp->Invoke(index, value) == ERROR_CODES_OK)
					{
						if (Acknowledge(DLMS_COMMAND_METHOD_RESPONSE, 0, m_SendData) == ERROR_CODES_OK)
						{
							return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
						}
					}
				}
				//Error is handled later.
			}
		}
	}
	//Return HW error.
    ServerReportError(1, 5, 3, m_SendData);
    return SendData(m_SendData, m_FrameIndex, pReply, ReplySize);
}

/// <summary>
/// Generates a read message reply.
/// </summary>
/// <param name="name">Short or Logical Name.</param>
/// <param name="objectType">Read Interface.</param>
/// <param name="attributeOrdinal">Read attribute index.</param>
int CGXDLMSServerBase::ReadReply(CGXDLMSVariant name, OBJECT_TYPE objectType, int attributeOrdinal,
							CGXDLMSVariant& value, DLMS_DATA_TYPE type, vector< vector<unsigned char> >& Packets)
{	
	int ret;
	if (type != DLMS_DATA_TYPE_NONE && type != DLMS_DATA_TYPE_ARRAY && (ret = value.ChangeType(type)) != 0)
	{
		return ret;
	}
	vector<unsigned char> buff;
	CGXOBISTemplate::SetData(buff, value.vt, value);
	ret = m_Base.GenerateMessage(name, 0, buff, objectType, attributeOrdinal,
			m_Base.GetUseLogicalNameReferencing() ? DLMS_COMMAND_GET_RESPONSE : DLMS_COMMAND_READ_RESPONSE, Packets);
	if (ret != 0)
	{
		printf("ReadReply failed with error code %d\r\n", ret);
	}
	return ret;
}

int CGXDLMSServerBase::GetValue(CGXDLMSObject* pItem, int index, unsigned char* pParameters, int count, vector< vector<unsigned char> >& Packets)
{
	if (index < 1)
	{
		printf("GetValue failed. Invalid Index.\r\n");
		return ERROR_CODES_INVALID_PARAMETER;
	}
	m_SendData.clear();
	CGXDLMSVariant value;	
	IGXDLMSBase* pTmp = dynamic_cast<IGXDLMSBase*>(pItem);
	int ret = pTmp->GetValue(index, pParameters, count, value);
	if (ret != ERROR_CODES_OK)
	{
		printf("GetValue failed with error code %d\r\n", ret);
		//Return HW error.
		return ServerReportError(1, 5, 3, Packets);    
	}
	DLMS_DATA_TYPE type = DLMS_DATA_TYPE_NONE;	
	if (pItem->GetDataType(index, type) != ERROR_CODES_OK)
	{
		printf("GetDataType failed with error code %d\r\n", ret);
		//Return HW error.
		return ServerReportError(1, 5, 3, Packets);    
	}
	if (type == DLMS_DATA_TYPE_NONE)
	{
		if (type == DLMS_DATA_TYPE_NONE)
		{
			type = value.vt;
		}
	}
	if (type != DLMS_DATA_TYPE_NONE && type != DLMS_DATA_TYPE_ARRAY)
	{
		if ((ret = value.ChangeType(type)) != ERROR_CODES_OK)
		{
			return ret;
		}
	}
	return ReadReply(pItem->GetName(), pItem->GetObjectType(), index, value, type, Packets);
}

/// <summary>
/// Get command, OBIS Code and attribute index.
/// </summary>
/// <param name="data"></param>        
/// <param name="name"></param>
/// <param name="attributeIndex"></param>
int CGXDLMSServerBase::GetCommand(unsigned char* pData, int DataSize, OBJECT_TYPE& type, CGXDLMSVariant& name, int& attributeIndex, unsigned char*& pParameters, int& ParameterSize)
{
	type = OBJECT_TYPE_NONE;
    pParameters = NULL;
    int index = 0;
    if (m_Base.GetUseLogicalNameReferencing())
    {
		type = (OBJECT_TYPE)CGXOBISTemplate::GetUInt16(pData + index);
		index += 2;
        basic_string<char> str;
		basic_string<unsigned char> tmp((unsigned char*) (pData + index), 6);
		index += 6;
		char buff[4];
		for(basic_string<unsigned char>::iterator it = tmp.begin(); it != tmp.end(); ++it)
        {
            if (str.begin() != str.end())
            {
				str.push_back('.');
            }
#if _MSC_VER > 1000
			sprintf_s(buff, 4, "%d", *it);
#else
			sprintf(buff, "%d", *it);
#endif
			str.append(buff);
        }
        name = str;
        attributeIndex = pData[index++];
        //Skip data index
        ++index;
        ParameterSize = DataSize - index;
        if (ParameterSize != 0)
        {
			pParameters = pData + index;
        }
    }
    else
    {
        attributeIndex = 0;
        ++index;//Cnt
        ++index;//Len.
        name = CGXOBISTemplate::GetUInt16(pData + index);
		index += 2;
		ParameterSize = DataSize - index;                        
        if (ParameterSize != 0)
        {
			pParameters = pData + index;
        }
    }
	return 0;
}


int CGXDLMSServerBase::SendData(vector< vector<unsigned char> >& buff, int index, unsigned char*& pReply, int& ReplySize)
{
	vector<unsigned char>& data = buff[index];
	pReply = &(data[0]);
	ReplySize = data.size();
	return ERROR_CODES_OK;
}

/** 
 Generates a acknowledge message.
*/
int CGXDLMSServerBase::Acknowledge(DLMS_COMMAND cmd, unsigned char status, vector< vector<unsigned char> >& Packets)
{
    vector<unsigned char> buff(0, 7);	
    if (GetInterfaceType() == GXDLMS_INTERFACETYPE_GENERAL)
    {
		GXHelpers::AddRange(buff, LLCReplyBytes, 3);
    }           
    //Get request normal
    buff.push_back(cmd);
    buff.push_back(0x01);
    //Invoke ID and priority.
    buff.push_back(0x81);
    buff.push_back(status);
	return m_Base.AddFrame(m_Base.GenerateIFrame(), false, buff, 0, buff.size(), Packets);
}

/// <summary>
/// Generates a acknowledge message.
/// </summary>
int CGXDLMSServerBase::ServerReportError(unsigned char serviceErrorCode, unsigned char type, unsigned char code, vector< vector<unsigned char> >& Packets)
{    
	printf("ServerReportError error has occurred.\r\n");
	vector<unsigned char> buff(0, 10);
    if (GetInterfaceType() == GXDLMS_INTERFACETYPE_GENERAL)
    {
		GXHelpers::AddRange(buff, LLCReplyBytes, 3);
    } 
    int cmd;
    if (GetUseLogicalNameReferencing())
    {
		cmd = DLMS_COMMAND_GET_RESPONSE;
    }
    else
    {
		cmd = DLMS_COMMAND_READ_RESPONSE;
    }
    //Get request normal
    buff.push_back(cmd);
    if (GetUseLogicalNameReferencing())
    {
        buff.push_back(0x01);                
    }
    buff.push_back(serviceErrorCode);
    //Invoke ID and priority.
    buff.push_back(type);
    buff.push_back(code);
	return m_Base.AddFrame(m_Base.GenerateIFrame(), false, buff, 0, buff.size(), Packets);
}  

/// <summary>
/// Generate disconnect request.
/// </summary>
/// <returns></returns>
int CGXDLMSServerBase::GenerateDisconnectRequest(vector< vector<unsigned char> >& Packets)
{
	vector<unsigned char> buff;
	if (m_Base.m_InterfaceType == GXDLMS_INTERFACETYPE_GENERAL)
	{
		buff.push_back(HDLC_INFO_MAX_INFO_TX);
		buff.push_back(m_Base.m_Limits.GetMaxInfoTX().GetSize());
		m_Base.m_Limits.GetMaxInfoTX().GetBytes(buff);
		buff.push_back(HDLC_INFO_MAX_INFO_RX);
		buff.push_back(m_Base.m_Limits.GetMaxInfoRX().GetSize());
		m_Base.m_Limits.GetMaxInfoRX().GetBytes(buff);
		buff.push_back(HDLC_INFO_WINDOW_SIZE_TX);
		buff.push_back(m_Base.m_Limits.GetWindowSizeTX().GetSize());
		m_Base.m_Limits.GetWindowSizeTX().GetBytes(buff);
		buff.push_back(HDLC_INFO_WINDOW_SIZE_RX);
		buff.push_back(m_Base.m_Limits.GetWindowSizeRX().GetSize());
		m_Base.m_Limits.GetWindowSizeRX().GetBytes(buff);
		unsigned char len = buff.size();
		buff.insert(buff.begin(), 0x81); //FromatID
		buff.insert(buff.begin() + 1, 0x80); //GroupID
		buff.insert(buff.begin() + 2, len); //len		
	}
	else
	{
		buff.push_back(0x63);
		buff.push_back(0x03);
		buff.push_back(0x80);
		buff.push_back(0x01);
		buff.push_back(0x00);
	}
	return m_Base.AddFrame(DLMS_FRAME_TYPE_UA, false, buff, 0, buff.size(), Packets);
}

// Parse AARQ request that cliend send and returns AARE request.
int CGXDLMSServerBase::HandleAARQRequest(unsigned char* pData, int DataSize, vector< vector<unsigned char> >& Packets)
{
    GXAPDU aarq;
	aarq.UseLN(m_Base.GetUseLogicalNameReferencing());
    int ret = aarq.EncodeData(pData, DataSize);
	if (ret != 0)
	{
		return ret;
	}
    GXDLMS_ASSOCIATION_RESULT result = GXDLMS_ASSOCIATION_RESULT_ACCEPTED;
    GXDLMS_SOURCE_DIAGNOSTIC diagnostic = GXDLMS_SOURCE_DIAGNOSTIC_NULL;
    if (m_Base.GetUseLogicalNameReferencing() != aarq.UseLN())
    {
        result = GXDLMS_ASSOCIATION_RESULT_REJECTED_PERMAMENT;
        diagnostic = GXDLMS_SOURCE_DIAGNOSTIC_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED;
    }
    else
    {
        GXAuthentication* pAuth = NULL;
		for(vector<GXAuthentication*>::iterator it = m_Authentications.begin(); it != m_Authentications.end(); ++it)
        {
			if ((*it)->GetType() == aarq.GetAuthentication())
            {
                pAuth = *it;
                break;
            }
        }
        if (pAuth == NULL)
        {
            result = GXDLMS_ASSOCIATION_RESULT_REJECTED_PERMAMENT;
            //If authentication is required.
            if (aarq.GetAuthentication() == GXDLMS_AUTHENTICATION_NONE)
            {
                diagnostic = GXDLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_REQUIRED;
            }
            else
            {
                diagnostic = GXDLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED;
            }
        }
        //If authentication is used check pw.
		else if (aarq.GetAuthentication() != GXDLMS_AUTHENTICATION_NONE && GXHelpers::StringCompare(pAuth->GetPassword().c_str(), aarq.GetPassword().c_str()))
        {
            result = GXDLMS_ASSOCIATION_RESULT_REJECTED_PERMAMENT;
            diagnostic = GXDLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
        }
    }
    //Generate AARE packet.
    vector<unsigned char> buff;
    unsigned char* pConformanceBlock;
    if (m_Base.GetUseLogicalNameReferencing())
    {
        pConformanceBlock = m_Base.m_pLNSettings->m_ConformanceBlock;
    }
    else
    {
        pConformanceBlock = m_Base.m_pSNSettings->m_ConformanceBlock;
    }
    aarq.GenerateAARE(buff, m_Base.m_MaxReceivePDUSize, pConformanceBlock, result, diagnostic);
    m_Base.m_ExpectedFrame = 0;
    m_Base.m_FrameSequence = -1;
	return m_Base.SplitToBlocks(buff, DLMS_COMMAND_NONE, Packets);
}

// Parse SNRM Request.
// If server do not accept client empty byte array is returned.
int CGXDLMSServerBase::HandleSNRMRequest(vector< vector<unsigned char> >& Packets)
{
	vector<unsigned char> buff;
	buff.push_back(HDLC_INFO_MAX_INFO_TX);
	buff.push_back(m_Base.m_Limits.GetMaxInfoTX().GetSize());
	m_Base.m_Limits.GetMaxInfoTX().GetBytes(buff);
    buff.push_back(HDLC_INFO_MAX_INFO_RX);
	buff.push_back(m_Base.m_Limits.GetMaxInfoRX().GetSize());
	m_Base.m_Limits.GetMaxInfoRX().GetBytes(buff);
    buff.push_back(HDLC_INFO_WINDOW_SIZE_TX);
	buff.push_back(m_Base.m_Limits.GetWindowSizeTX().GetSize());
	m_Base.m_Limits.GetWindowSizeTX().GetBytes(buff);
    buff.push_back(HDLC_INFO_WINDOW_SIZE_RX);
	buff.push_back(m_Base.m_Limits.GetWindowSizeRX().GetSize());
	m_Base.m_Limits.GetWindowSizeRX().GetBytes(buff);
    unsigned char len = buff.size();
	buff.insert(buff.begin(), 0x81); //FromatID
    buff.insert(buff.begin() + 1, 0x80); //GroupID
    buff.insert(buff.begin() + 2, len); //len
    return m_Base.AddFrame(DLMS_FRAME_TYPE_UA, false, buff, 0, buff.size(), Packets);
}
