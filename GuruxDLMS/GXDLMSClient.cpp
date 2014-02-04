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

#include "Enums.h"
#include "GXDLMSClient.h"
#include "GXHelpers.h"
#include "GXAPDU.h"
#include "GXOBISTemplate.h"
#include "Objects/GXDLMSObject.h"
#include "OBiscodes.h"
#include "Objects/GXDLMSObject.h"

CGXDLMSClient::CGXDLMSClient(bool UseLogicalNameReferencing, 
				CGXDLMSVariant ClientID,
				CGXDLMSVariant ServerID,
				//Authentication type.
				GXDLMS_AUTHENTICATION Authentication, 
				//Password if authentication is used.
				const char* pPassword,
				GXDLMS_INTERFACETYPE IntefaceType) : m_base(false)
{
	m_base.m_UseLogicalNameReferencing = UseLogicalNameReferencing;
	m_base.ClientID = ClientID;
	m_base.ServerID = ServerID;
	m_base.m_InterfaceType = IntefaceType;
	m_base.m_Authentication = Authentication;
	m_base.m_pPassword = pPassword;
}

CGXDLMSClient::~CGXDLMSClient()
{
	if (m_base.m_pLNSettings != NULL)
	{
		delete m_base.m_pLNSettings;
		m_base.m_pLNSettings = NULL;
	}
	if (m_base.m_pSNSettings != NULL)
	{
		delete m_base.m_pSNSettings;
		m_base.m_pSNSettings = NULL;
	}	
}

int CGXDLMSClient::SNRMRequest(vector< vector<unsigned char> >& Packets)
{
	Packets.clear();
	m_base.m_pLNSettings = NULL;
	m_base.m_pSNSettings = NULL;
	m_base.m_ExpectedFrame = m_base.m_FrameSequence = 0;
	m_base.m_MaxReceivePDUSize = 0xFFFF;
	m_base.ClearProgress();
	//SNRM reguest is not used in network connections.
	if (m_base.m_InterfaceType == GXDLMS_INTERFACETYPE_NET)
	{
		return ERROR_CODES_OK;
	}	
	std::vector<unsigned char> items;
	int ret = m_base.AddFrame(DLMS_COMMAND_SNRM, false, items, 0, 0, Packets);
	return ret;
}

int CGXDLMSClient::GetDataFromPacket(unsigned char* Packet, int PacketSize, std::vector<unsigned char>& reply, GXDLMS_DATA_REQUEST_TYPES& moreData)
{
	unsigned char* pData = NULL;
	int DataSize = 0;
	int ret = GetDataFromPacket(Packet, PacketSize, pData, DataSize, moreData);
	if (ret == 0)
	{
		reply.insert(reply.end(), reinterpret_cast<const unsigned char*>(pData), 
        reinterpret_cast<const unsigned char*>(pData) + DataSize);
	}
	if (pData != NULL)
	{
		delete pData;
	}
	return ret;
}

int CGXDLMSClient::GetDataFromPacket(unsigned char* Packet, int PacketSize, unsigned char*& Data, int& DataSize, GXDLMS_DATA_REQUEST_TYPES& moreData)
{
	DLMS_COMMAND cmd;
	return m_base.GetDataFromPacket(Packet, PacketSize, Data, DataSize, moreData, cmd);
}

int CGXDLMSClient::ParseUAResponse(vector<unsigned char>& data)
{	
	if (m_base.GetInterfaceType() == GXDLMS_INTERFACETYPE_NET && data.size() == 0)
	{
		return ERROR_CODES_OK;
	}
	return ParseUAResponse(&data[0], data.size());
}

int CGXDLMSClient::ParseUAResponse(unsigned char* pBuff, int buffSize)
{	
	if (pBuff == NULL || buffSize == 0)
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}
	int pos = 0;
	++pos; // Skip FromatID
	++pos; // Skip GroupID
	++pos; // Skip GroupLen
	int ret = ERROR_CODES_OK;
	while(pos < buffSize)
	{
		unsigned char id = pBuff[pos++];
		unsigned char len = pBuff[pos++];
		CGXDLMSVariant value;
		GXHelpers::ChangeByteOrder(&value.bVal, pBuff + pos, len);
		switch(len)
		{
		case 1:
			value.vt = DLMS_DATA_TYPE_INT8;
			break;
		case 2:
			value.vt = DLMS_DATA_TYPE_INT16;
			break;
		case 4:
			value.vt = DLMS_DATA_TYPE_INT32;
			break;
		default:
			return ERROR_CODES_INVALID_PARAMETER;
		}
		pos += len;
		switch(id)
		{
			case HDLC_INFO_MAX_INFO_TX:	
				m_base.m_Limits.SetMaxInfoTX(value);
			break;
			case HDLC_INFO_MAX_INFO_RX:
				m_base.m_Limits.SetMaxInfoRX(value);
				value.ChangeType(DLMS_DATA_TYPE_UINT32);
				m_base.m_MaxInfoRX = value.ulVal;
			break;
			case HDLC_INFO_WINDOW_SIZE_TX:
				m_base.m_Limits.SetWindowSizeTX(value);
			break;
			case HDLC_INFO_WINDOW_SIZE_RX:
				m_base.m_Limits.SetWindowSizeRX(value);
			break;
			default:	
				ret = ERROR_CODES_INVALID_PARAMETER;
			break;
		}
	}	
	return ret;
}

/** 
 Changes byte array received from the meter to given type.

 @param value Byte array received from the meter.
 @param type Wanted type.
 @return Value changed by type.
*/
int CGXDLMSClient::ChangeType(vector<unsigned char> value, DLMS_DATA_TYPE type, CGXDLMSVariant& newValue)
{	
	newValue.Clear();
	if (value.size() == 0)
    {
		if (type == DLMS_DATA_TYPE_STRING || type == DLMS_DATA_TYPE_OCTET_STRING ||
			type == DLMS_DATA_TYPE_STRING_UTF8)
		{
			newValue = "";
		}
		return ERROR_CODES_OK;
    }
    if (type == DLMS_DATA_TYPE_NONE)
    {
		newValue = GXHelpers::bytesToHex(&value[0], value.size());
        return ERROR_CODES_OK;
    }
    int ret;
	unsigned char* pBuff = &value[0];
	int size = value.size();
	if ((ret = CGXOBISTemplate::GetData(pBuff, size, type, newValue)) != 0)
	{
		return ret;
	}	
    return ret;
}

int CGXDLMSClient::AARQRequest(vector< vector<unsigned char> >& Packets)
{
	Packets.clear();
	int ret = m_base.CheckInit();
	if (ret != 0)
	{
		return ret;
	}	
	m_base.m_FrameSequence = m_base.m_ExpectedFrame = -1;
	GXAPDU aarq;
	aarq.UseLN(m_base.m_UseLogicalNameReferencing);	
	if (m_base.m_UseLogicalNameReferencing)
	{	
		if (m_base.m_pLNSettings == NULL)
		{
			m_base.m_pLNSettings = new CGXDLMSLNSettings();
		}
		aarq.UserInformation.SetConformanceBlock(m_base.m_pLNSettings->m_ConformanceBlock);		
	}
	else
	{		
		if (m_base.m_pSNSettings == NULL)
		{
			m_base.m_pSNSettings = new CGXDLMSSNSettings();
		}
		aarq.UserInformation.SetConformanceBlock(m_base.m_pSNSettings->m_ConformanceBlock);
	}		
	aarq.SetAuthentication(m_base.m_Authentication, m_base.m_pPassword);
	aarq.UserInformation.DLMSVersioNumber = m_base.m_DLMSVersion;
	aarq.UserInformation.MaxReceivePDUSize = (m_base.m_MaxReceivePDUSize);
	std::vector<unsigned char> buff;
	if ((ret = aarq.CodeData(buff, m_base.m_InterfaceType)) != 0)
	{
		return ret;
	}
	return m_base.SplitToBlocks(buff, DLMS_COMMAND_NONE, Packets);
}

int CGXDLMSClient::GetKeepAlive(vector< vector<unsigned char> >& Packets)
{
	m_base.ClearProgress();
	vector<unsigned char> data;
	return m_base.AddFrame(m_base.GenerateAliveFrame(), false, data, 0, 0, Packets);
}

int CGXDLMSClient::ParseAAREResponse(vector<unsigned char>& data)
{
	return ParseAAREResponse(&data[0], data.size());
}

int CGXDLMSClient::ParseAAREResponse(unsigned char* pBuff, int DataLen)
{
	int ret = ERROR_CODES_OK;
	//Parse AARE data.
	GXAPDU pdu;
	ret = pdu.EncodeData(pBuff, DataLen);
	if (ret != 0)
	{		
		return ret;
	}
	m_base.m_UseLogicalNameReferencing = pdu.UseLN();	 
	if (m_base.m_UseLogicalNameReferencing)
	{
		TRACE1("--- Logical Name settings are---\r\n");
	}
	else
	{
		TRACE1("--- Short Name settings are---\r\n");
	}
	GXDLMS_ASSOCIATION_RESULT result = pdu.GetResultComponent();
	if (result == GXDLMS_ASSOCIATION_RESULT_ACCEPTED)
	{
		TRACE1("- Client has accepted connection.\r\n");
		if (m_base.m_UseLogicalNameReferencing)
		{
			m_base.m_pSNSettings = NULL;
			if (m_base.m_pLNSettings == NULL)
			{
				m_base.m_pLNSettings = new CGXDLMSLNSettings();
			}
			m_base.m_pLNSettings->SetConformanceBlock(pdu.UserInformation.GetConformanceBlock());
		}
		else
		{
			m_base.m_pLNSettings = NULL;			
			if (m_base.m_pSNSettings == NULL)
			{
				m_base.m_pSNSettings = new CGXDLMSSNSettings();
			}
			m_base.m_pSNSettings->SetConformanceBlock(pdu.UserInformation.GetConformanceBlock());
		}
		m_base.m_MaxReceivePDUSize = pdu.UserInformation.MaxReceivePDUSize;
		m_base.m_DLMSVersion = pdu.UserInformation.DLMSVersioNumber;
		TRACE("- Server max PDU size is %d.\n", m_base.m_MaxReceivePDUSize);
		TRACE("- Server DLMS version number is %d\n", m_base.m_DLMSVersion);
	}
	else
	{
		GXDLMS_SOURCE_DIAGNOSTIC diag = pdu.GetResultDiagnosticValue();
		TRACE("Result diagnostic value is %d\n", diag);
		if (result == GXDLMS_ASSOCIATION_RESULT_REJECTED_PERMAMENT)
		{
			ret = ERROR_CODES_REJECTED_PERMAMENT;
		}
		else if (result == GXDLMS_ASSOCIATION_RESULT_REJECTED_TRANSIENT)
		{
			ret = ERROR_CODES_REJECTED_TRANSIENT;
		}
		if (diag == GXDLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN)
		{
			ret = ERROR_CODES_NO_REASON_GIVEN;
		}
		else if (diag == GXDLMS_SOURCE_DIAGNOSTIC_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED)
		{
			ret = ERROR_CODES_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED;
		}
		else if (diag == GXDLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED)
		{
			ret = ERROR_CODES_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED;
		}
		else if (diag == GXDLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED)
		{
			ret = ERROR_CODES_AUTHENTICATION_MECHANISM_NAME_REQUIRED;
		}
		else if (diag == GXDLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE)
		{
			ret = ERROR_CODES_AUTHENTICATION_FAILURE;
		}
		else if (diag == GXDLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_REQUIRED)
		{
			ret = ERROR_CODES_AUTHENTICATION_REQUIRED;
		}
		return ret;		
	}
	assert(m_base.m_DLMSVersion == 6);
	return ret;
}

int CGXDLMSClient::DisconnectedModeRequest(vector< vector<unsigned char> >& Packets)
{
	m_base.ClearProgress();
	//In corrent behavior disconnect is not generated for network connection.
	if (m_base.m_InterfaceType == GXDLMS_INTERFACETYPE_NET)
	{
		return ERROR_CODES_OK;
	}
	//If connection is not established there is no need to send DisconnectRequest.
	if (m_base.m_pSNSettings == NULL && m_base.m_pLNSettings == NULL)
	{
		return ERROR_CODES_OK;
	}	
	vector<unsigned char> data;
	return m_base.AddFrame(DLMS_FRAME_TYPE_DISCONNECT_MODE, false, data, 0, 0, Packets);
}

int CGXDLMSClient::DisconnectRequest(vector< vector<unsigned char> >& Packets)
{
	m_base.ClearProgress();
	//If connection is not established there is no need to send DisconnectRequest.
	if (m_base.m_pSNSettings == NULL && m_base.m_pLNSettings == NULL)
	{
		return ERROR_CODES_OK;
	}
	if (m_base.m_InterfaceType == GXDLMS_INTERFACETYPE_NET)
	{	
		vector<unsigned char> data;
		data.push_back(0x62);
		data.push_back(0x0);
		return m_base.AddFrame(DLMS_FRAME_TYPE_DISCONNECT, false, data, 0, 2, Packets);
	}	
	vector<unsigned char> data;
	return m_base.AddFrame(DLMS_FRAME_TYPE_DISCONNECT, false, data, 0, 0, Packets);
}

int CGXDLMSClient::GetObjectsRequest(vector< vector<unsigned char> >& Packets)
{	
	if (m_base.m_UseLogicalNameReferencing)
	{
		unsigned char ln[] = {0, 0, 40, 0, 0, 0xFF};
		CGXDLMSVariant name(ln, 6, DLMS_DATA_TYPE_OCTET_STRING);		
		return Read(name, OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, 2, Packets);
	}
	CGXDLMSVariant name((unsigned short) 0xFA00);
	return Read(name, OBJECT_TYPE_ASSOCIATION_SHORT_NAME, 2, Packets);
}

int CGXDLMSClient::Read(CGXDLMSVariant& name, OBJECT_TYPE InterfaceClass, int AttributeOrdinal, vector< vector<unsigned char> >& Packets)
{
	Packets.clear();
	if (InterfaceClass == OBJECT_TYPE_NONE || AttributeOrdinal < 0)
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}	
	DLMS_COMMAND cmd = m_base.m_UseLogicalNameReferencing ? DLMS_COMMAND_GET_REQUEST : DLMS_COMMAND_READ_REQUEST;
	m_base.ClearProgress();
	vector<unsigned char> data;
	return m_base.GenerateMessage(name, 2, data, InterfaceClass, AttributeOrdinal, cmd, Packets);
}

int CGXDLMSClient::Method(CGXDLMSObject* item, int AttributeOrdinal, CGXDLMSVariant Data, vector< vector<unsigned char> >& Packets)
{
	CGXDLMSVariant name = item->GetName();
	return Method(name, item->GetObjectType(), AttributeOrdinal, Data, Packets);         
}

int CGXDLMSClient::Method(CGXDLMSVariant& name, OBJECT_TYPE InterfaceClass, int AttributeOrdinal, CGXDLMSVariant Value, vector< vector<unsigned char> >& Packets)
{
	Packets.clear();
	if (InterfaceClass == OBJECT_TYPE_NONE || AttributeOrdinal < 0)
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}	
	m_base.ClearProgress();
	vector<unsigned char> data;
	CGXOBISTemplate::SetData(data, Value.vt, Value);
	if (!m_base.m_UseLogicalNameReferencing)
    {
        int value, count;
		CGXDLMS::GetActionInfo(InterfaceClass, value, count);
        if (AttributeOrdinal > count)
        {
            return ERROR_CODES_INVALID_PARAMETER;
        }
        AttributeOrdinal = (value + (AttributeOrdinal - 1) * 0x8);
    }
	return m_base.GenerateMessage(name, 2, data, InterfaceClass, AttributeOrdinal, DLMS_COMMAND_METHOD_REQUEST, Packets);    
}

int CGXDLMSClient::Write(CGXDLMSVariant& name, OBJECT_TYPE InterfaceClass, int AttributeOrdinal, CGXDLMSVariant Value, vector< vector<unsigned char> >& Packets)
{
	Packets.clear();
	if (InterfaceClass == OBJECT_TYPE_NONE || AttributeOrdinal < 0)
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}	
	DLMS_COMMAND cmd = m_base.m_UseLogicalNameReferencing ? DLMS_COMMAND_SET_REQUEST : DLMS_COMMAND_WRITE_REQUEST;
	m_base.ClearProgress();
	vector<unsigned char> data;
	CGXOBISTemplate::SetData(data, Value.vt, Value);
	return m_base.GenerateMessage(name, 2, data, InterfaceClass, AttributeOrdinal, cmd, Packets);
}

int CGXDLMSClient::ParseObjects(vector<unsigned char>& data, CGXDLMSObjectCollection& objects, bool findDescriptions)
{		
	int ret;
	if (m_base.m_UseLogicalNameReferencing) 
	{
		ret = m_base.ParseLNObjects(&data[0], data.size(), objects);
	}
	else
	{
		ret = m_base.ParseSNObjects(&data[0], data.size(), objects);	
	}
	if (ret != ERROR_CODES_OK)
	{
		return ret;
	}
	if (findDescriptions)
	{
		UpdateOBISCodes(objects);
	}
	return ERROR_CODES_OK;
}

int CGXDLMSClient::ReadRowsByRange(CGXDLMSVariant& Name, CGXDLMSObject* pSortObject, struct tm* start, struct tm* end, vector< vector<unsigned char> >& Packets)
{		
	if (pSortObject == NULL)
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}
	int ret;
	Packets.clear();
	m_base.ClearProgress();
	std::vector<unsigned char> data;	
	data.push_back(0x01);  //Add AccessSelector
    data.push_back(DLMS_DATA_TYPE_STRUCTURE); //Add enum tag.
    data.push_back(0x04); //Add item count   
	data.push_back(DLMS_DATA_TYPE_STRUCTURE); //Add enum tag.
	data.push_back(0x04); //Add item count           
	// Add class_id
	CGXDLMSVariant value = (unsigned short) pSortObject->GetObjectType();
	if((ret = CGXOBISTemplate::SetData(data, value.vt, value)) != 0)
	{
		return ret;
	}
	// Add parameter Logical name
	CGXDLMSVariant ln(&pSortObject->GetName().byteArr[0], 6, DLMS_DATA_TYPE_OCTET_STRING);	
	if((ret = CGXOBISTemplate::SetData(data, ln.vt, ln)) != 0)
	{
		return ret;
	}
	//Add attribute index.
	value = (char) 2;
	if((ret = CGXOBISTemplate::SetData(data, value.vt, value)) != 0)
	{
		return ret;
	}
	//Add version.
	value = pSortObject->GetVersion();
	if((ret = CGXOBISTemplate::SetData(data, value.vt, value)) != 0)
	{
		return ret;
	}
	//Add start time		
	CGXDLMSVariant tmp = CGXDateTime(*start);
	if((ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_OCTET_STRING, tmp)) != 0)
	{		
		return ret;
	}
	//Add end time
	tmp = CGXDateTime(*end);
	if((ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_OCTET_STRING, tmp)) != 0)
	{	
		return ret;
	}
    //Add array of read columns. Read All...
    data.push_back(0x01); //Add array type   
    data.push_back(0x00); //Add item count
	DLMS_COMMAND cmd = m_base.m_UseLogicalNameReferencing ? DLMS_COMMAND_GET_REQUEST : DLMS_COMMAND_READ_REQUEST;
	return m_base.GenerateMessage(Name, 4, data, OBJECT_TYPE_PROFILE_GENERIC, 2, cmd, Packets);	
}

int CGXDLMSClient::ReadRowsByEntry(CGXDLMSVariant& Name, unsigned int Index, unsigned int Count, vector< vector<unsigned char> >& Packets)
{
	int ret;
	Packets.clear();
	m_base.ClearProgress();
	std::vector<unsigned char> data;	
	data.push_back(2);  //Add AccessSelector
    data.push_back(DLMS_DATA_TYPE_STRUCTURE); //Add enum tag.
    data.push_back(0x04); //Add item count
	//Add start index
    CGXDLMSVariant value = CGXDLMSVariant((unsigned long) Index);
	if((ret = CGXOBISTemplate::SetData(data, value.vt, value)) != 0)
	{		
		return ret;
	}
	//Add Count
	value = (unsigned long) Count;
	if((ret = CGXOBISTemplate::SetData(data, value.vt, value)) != 0)
	{		
		return ret;
	}
	//Read all columns.
	value = (unsigned short) 0;
	if((ret = CGXOBISTemplate::SetData(data, value.vt, value)) != 0 ||
		(ret = CGXOBISTemplate::SetData(data, value.vt, value)) != 0)
	{		
		return ret;
	}
	DLMS_COMMAND cmd = m_base.m_UseLogicalNameReferencing ? DLMS_COMMAND_GET_REQUEST : DLMS_COMMAND_READ_REQUEST;
	return m_base.GenerateMessage(Name, 4, data, OBJECT_TYPE_PROFILE_GENERIC, 2, cmd, Packets);	
}

int CGXDLMSClient::ReceiverReady(GXDLMS_DATA_REQUEST_TYPES Type, vector< vector<unsigned char> >& Packets)
{	
	if (Type < GXDLMS_DATA_REQUEST_TYPES_FRAME || Type > GXDLMS_DATA_REQUEST_TYPES_BLOCK)
	{
		return ERROR_CODES_INVALID_PARAMETER;
	}
	vector<unsigned char> data;
	if (!m_base.m_UseLogicalNameReferencing || Type == GXDLMS_DATA_REQUEST_TYPES_FRAME)
	{
		unsigned char id = m_base.GenerateSupervisoryFrame(0);
		return m_base.AddFrame(id, false, data, 0, 0, Packets);
	} 
	if (m_base.m_InterfaceType != GXDLMS_INTERFACETYPE_NET)
	{
		if (m_base.m_Server)
		{
			GXHelpers::AddRange(data, LLCReplyBytes, 3);
		}
		else
		{
			GXHelpers::AddRange(data, LLCSendBytes, 3);
		}		
	}	
	//Get request normal
	data.push_back(0xC0);
	data.push_back(0x02);
	//Invoke ID and priority.
	data.push_back(0x81);	
	GXHelpers::ChangeByteOrder(data, &m_base.m_PacketIndex, 4);
	return m_base.AddFrame(m_base.GenerateIFrame(), false, data, 0, data.size(), Packets);	
}

bool CGXDLMSClient::IsDLMSPacketComplete(unsigned char* Data, int buffSize)
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
	int ret = m_base.GetDataFromHDLCFrame(Data, buffSize, FrameType, pBuff, size, cmd, MoreData);
	if (pBuff != NULL)
	{
		delete pBuff;
		pBuff = NULL;
	}	
	return ret == ERROR_CODES_OK;
}

int CGXDLMSClient::CheckReplyErrors(unsigned char* SendData, int SendbuffSize, unsigned char* ReceivedData, int ReceivedbuffSize)
{
	int ret;
	if (SendData != NULL)
	{
		if ((ret = IsReplyPacket(SendData, SendbuffSize, ReceivedData, ReceivedbuffSize)) != 0)
		{
			//If packet is not reply for send packet...
			if (ret == ERROR_CODES_FALSE)
			{
				return ERROR_CODES_NOT_REPLY;
			}
			return ret;
		}		
	}
	//If we are checking UA or AARE messages.
	if (m_base.m_pLNSettings == NULL && m_base.m_pSNSettings == NULL)
	{		
		return ERROR_CODES_OK;
	}

	unsigned char* pBuff = NULL;
	unsigned char FrameType = -1;
	int DataLen = -1;	
	DLMS_COMMAND cmd;
	GXDLMS_DATA_REQUEST_TYPES bMoreData = GXDLMS_DATA_REQUEST_TYPES_NONE;
	if (SendData != NULL)
	{
		ret = m_base.GetDataFromHDLCFrame(SendData, SendbuffSize, FrameType, pBuff, DataLen, cmd, bMoreData, false);
		if (pBuff != NULL)
		{
			delete pBuff;
			pBuff = NULL;
		}
		if (ret != 0)
		{
			return ret;
		}
		if (m_base.IsReceiverReadyRequest(FrameType) || FrameType == DLMS_FRAME_TYPE_DISCONNECT)
		{			
			return ERROR_CODES_OK;
		}
	}
	int errCode = 0;
	ret = m_base.GetDataFromHDLCFrame(ReceivedData, ReceivedbuffSize, FrameType, pBuff, DataLen, cmd, bMoreData, true, NULL, false, true, &errCode);
	if (pBuff != NULL)
	{
		delete pBuff;
		pBuff = NULL;
	}
	if (ret != ERROR_CODES_OK)
	{
		return ERROR_CODES_OK;
	}
	if (cmd == DLMS_COMMAND_REJECTED)
	{
		return ERROR_CODES_FRAME_REJECTED;
	}

	if (errCode != 0x00)
	{		
		switch(errCode) 
		{
		case 1: //Access Error : Device reports a hardware fault
			return ERROR_CODES_HW_FAULT;
		break;
		case 2: //Access Error : Device reports a temporary failure
			return ERROR_CODES_TEMP_FAIL;
		break;
		case 3: // Access Error : Device reports Read-Write denied
			return ERROR_CODES_WRITE_DENIED;
		break;
		case 4: // Access Error : Device reports a undefined object
			return ERROR_CODES_UNDEFINED_OBJ;
		break;
		case 5: // Access Error : Device reports a inconsistent Class or Object
			return ERROR_CODES_INCONSISTENT_OBJ;
		break;
		case 6: // Access Error : Device reports a unavailable object
			return ERROR_CODES_UNAVAILABLE_OBJ;
		break;
		case 7: // Access Error : Device reports a unmatched type
			return ERROR_CODES_UNMATCH_TYPE;
		break;
		case 8: // Access Error : Device reports scope of access violated
			return ERROR_CODES_VIOLATED;
		break;
		default:			
			//Unknown error.
			return ERROR_CODES_UNKNOWN;
		break;
		}
	}
	return ERROR_CODES_OK;
}

int CGXDLMSClient::IsReplyPacket(unsigned char* SendData, int SendbuffSize, unsigned char* ReceivedData, int ReceivedbuffSize)
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
	if ((ret = m_base.GetDataFromHDLCFrame(SendData, SendbuffSize, SendID, pSendBuff, size, cmd, MoreData, false)) == 0 &&
		(ret = m_base.GetDataFromHDLCFrame(ReceivedData, ReceivedbuffSize, ReceivedID, pReceivedBuff, size, cmd, MoreData, true)) == 0)
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
		if (ReceivedID == DLMS_FRAME_TYPE_REJECTED || (SendID == DLMS_FRAME_TYPE_DISCONNECT && ReceivedID == DLMS_FRAME_TYPE_UA) || m_base.IsExpectedFrame(SendID, ReceivedID))
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

int CGXDLMSClient::GetValue(vector<unsigned char>& data, CGXDLMSVariant& value)
{	
	return GetValue(&data[0], data.size(), value);
}

int CGXDLMSClient::GetValue(unsigned char* pBuff, int Size, CGXDLMSVariant& value)
{	
	return CGXOBISTemplate::GetData(pBuff, Size, DLMS_DATA_TYPE_NONE, value);
}

int CGXDLMSClient::GetValue(vector<unsigned char>& data, CGXDLMSObject* pObject, int index, CGXDLMSVariant& value)
{	
	int ret;	
	if ((ret = GetValue(&data[0], data.size(), value)) != 0)
	{
		return ret;
	}		
	if (value.vt == DLMS_DATA_TYPE_ARRAY)
	{
		DLMS_DATA_TYPE type;
		if ((ret = pObject->GetUIDataType(index, type)) != 0)
		{
			return ret;
		}
		if (type != DLMS_DATA_TYPE_NONE)
		{
			CGXDLMSVariant tmp = value;
			value.Clear();
			return ChangeType(tmp.byteArr, type, value);
		}
	}
	return ret;
}

int CGXDLMSClient::GetValue(unsigned char* pBuff, int Size, CGXDLMSObject* pObject, int index, CGXDLMSVariant& value)
{	
	int ret;
	if ((ret = GetValue(pBuff, Size, value)) != 0)
	{
		return ret;
	}
	if (value.vt == DLMS_DATA_TYPE_ARRAY)
	{
		DLMS_DATA_TYPE type;
		if ((ret = pObject->GetUIDataType(index, type)) != 0)
		{
			return ret;
		}
		if (type != DLMS_DATA_TYPE_NONE)
		{
			CGXDLMSVariant tmp = value;
			value.Clear();
			return ChangeType(tmp.byteArr, type, value);
		}
	}
	return ret;
}

int CGXDLMSClient::UpdateValue(unsigned char* pBuff, int Size, CGXDLMSObject* pObject, int index, CGXDLMSVariant& value)
{
	value.Clear();
	int ret;
	if ((ret = GetValue(pBuff, Size, pObject, index, value)) != 0)
	{
		return ret;
	}
	return pObject->SetValue(index, value);
}

int CGXDLMSClient::UpdateValue(vector<unsigned char>& data, CGXDLMSObject* pObject, int index, CGXDLMSVariant& value)
{
	return UpdateValue(&data[0], data.size(), pObject, index, value);
}


int CGXDLMSClient::GetDataType(vector<unsigned char>& data, DLMS_DATA_TYPE& Type)
{	
	return GetDataType(&data[0], data.size(), Type);
}

int CGXDLMSClient::GetDataType(unsigned char* pBuff, int Size, DLMS_DATA_TYPE& Type)
{	
	return CGXOBISTemplate::GetDataType(pBuff, Size, Type);
}


void CGXDLMSClient::UpdateOBISCodes(CGXDLMSObjectCollection& objects)
{                    
    basic_string<char> str = OBIS_CODES1;        
	str.append(OBIS_CODES2);
	str.append(OBIS_CODES3);
	str.append(OBIS_CODES4);
	str.append(OBIS_CODES5);
	str.append(OBIS_CODES6);
	str.append(OBIS_CODES7);
	str.append(OBIS_CODES8);
	str.append(OBIS_CODES9);
	str.append(OBIS_CODES10);
	str.append(OBIS_CODES11);
    CGXStandardObisCodeCollection codes;
	vector< basic_string<char> > rows = GXHelpers::Split(str, "\r\n", true);
	int row = 0;
	basic_string<char> last;
	for(vector< basic_string<char> >::iterator it = rows.begin(); it != rows.end(); ++it)
	{
        vector< basic_string<char> > items = GXHelpers::Split(*it, ";\r\n", false);
		if (items.size() != 8)
		{
			items = GXHelpers::Split(*it, ";\r\n", false);
		}
		assert(items.size() == 8);	
        vector< basic_string<char> > obis = GXHelpers::Split(items[0], ".\r\n", false);
		if (obis.size() != 6)
		{
			obis = GXHelpers::Split(items[0], ".\r\n", false);
		}
        basic_string<char> str = items[3] + "; " + items[4] + "; " + items[5] + "; " + items[6] + "; " + items[7];
        codes.push_back(CGXStandardObisCode(obis, str, items[1], items[2]));
		++row;
		last = *it;
    }
	for(vector<CGXDLMSObject*>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
        string ln;
		(*it)->GetLogicalName(ln);
        CGXStandardObisCode code;		
        if (codes.Find(ln, (*it)->GetObjectType(), code))
        {
            (*it)->SetDescription(code.GetDescription());
            //If string is used
			if (code.GetDataType().find("10") != std::string::npos)
            {
                code.SetDataType("10");
            }
            //If date time is used.
            else if (code.GetDataType().find("25") != std::string::npos ||
				code.GetDataType().find("26") != std::string::npos)
            {
                code.SetDataType("25");
            }                
            else if (code.GetDataType().find("9"))
            {
                //Time stamps of the billing periods objects (first scheme if there are two)
                if ((CGXStandardObisCodeCollection::EqualsMask("0.0-64.96.7.10-14.255", ln) ||
                    //Time stamps of the billing periods objects (second scheme)
                    CGXStandardObisCodeCollection::EqualsMask("0.0-64.0.1.5.0-99,255", ln) ||
                    //Time of power failure
                    CGXStandardObisCodeCollection::EqualsMask("0.0-64.0.1.2.0-99,255", ln) ||
                    //Time stamps of the billing periods objects (first scheme if there are two)                        
                    CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.1.2.0-99,255", ln) ||
                    //Time stamps of the billing periods objects (second scheme)
                    CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.1.5.0-99,255", ln) ||
                    //Time expired since last end of billing period
                    CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.0.255", ln) ||                        
                    //Time of last reset
                    CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.6.255", ln) ||
                    //Date of last reset
                    CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.7.255", ln) ||
                    //Time expired since last end of billing period (Second billing period scheme)
                    CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.13.255", ln) ||                        
                    //Time of last reset (Second billing period scheme)
                    CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.14.255", ln) ||
                    //Date of last reset (Second billing period scheme)
                    CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.15.255", ln)))
                {
                    code.SetDataType("25");
                }
                //Local time
                else if (CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.1.255", ln))
                {
                    code.SetDataType("27");
                }
                //Local date
                else if (CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.2.255", ln))
                {
                    code.SetDataType("26");
                }
            }
            if (code.GetDataType() != "*" && 
                    code.GetDataType() != "" && 
                    code.GetDataType().find(",") == std::string::npos)
            {
				int value;
#if _MSC_VER > 1000
				sscanf_s(code.GetDataType().c_str(), "%d", &value);
#else
				sscanf(code.GetDataType().c_str(), "%d", &value);
#endif												
				DLMS_DATA_TYPE type = (DLMS_DATA_TYPE) value;
				switch ((*it)->GetObjectType())
				{
					case OBJECT_TYPE_DATA:
					case OBJECT_TYPE_REGISTER:
					case OBJECT_TYPE_REGISTER_ACTIVATION:
					case OBJECT_TYPE_EXTENDED_REGISTER:
						(*it)->SetUIDataType(2, type);
						break;
					default:
						break;
				}
			}
		}
		else
		{
			/*
			System.out.println("Unknown OBIS Code: " + it.getLogicalName() + 
					" Type: " + it.getObjectType());                
					*/
		}
    }
}
