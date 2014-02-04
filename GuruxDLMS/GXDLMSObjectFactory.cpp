#include "GXDLMSObjectFactory.h"
#include "GXDLMSClient.h"
#include "Objects/GXDLMSActionSchedule.h"
#include "Objects/GXDLMSActivityCalendar.h"
#include "Objects/GXDLMSAssociationLogicalName.h"
#include "Objects/GXDLMSAssociationShortName.h"
#include "Objects/GXDLMSAutoAnswer.h"
#include "Objects/GXDLMSAutoConnect.h"
#include "Objects/GXDLMSClock.h"
#include "Objects/GXDLMSData.h"
#include "Objects/GXDLMSRegister.h"
#include "Objects/GXDLMSExtendedRegister.h"
#include "Objects/GXDLMSDemandRegister.h"
#include "Objects/GXDLMSRegisterMonitor.h"
#include "Objects/GXDLMSProfileGeneric.h"
#include "Objects/GXDLMSTcpUdpSetup.h"
#include "Objects/GXDLMSGprsSetup.h"
#include "Objects/GXDLMSActionSchedule.h"
#include "Objects/GXDLMSSpecialDaysTable.h"
#include "Objects/GXDLMSImageTransfer.h"
#include "Objects/GXDLMSDisconnectControl.h"
#include "Objects/GXDLMSLimiter.h"
#include "Objects/GXDLMSMBusClient.h"
#include "Objects/GXDLMSScriptTable.h"
#include "Objects/GXDLMSMBusSlavePortSetup.h"
#include "Objects/GXDLMSSecuritySetup.h"
#include "Objects/GXDLMSPppSetup.h"
#include "Objects/GXDLMSMacAddressSetup.h"
#include "Objects/GXDLMSHdlcSetup.h"
#include "Objects/GXDLMSIECOpticalPortSetup.h"
#include "Objects/GXDLMSIp4Setup.h"
#include "Objects/GXDLMSRegisterActivation.h"
#include "Objects/GXDLMSSchedule.h"
#include "Objects/GXDLMSModemConfiguration.h"
#include "Objects/GXDLMSSapAssignment.h"

// Reserved for internal use. 
CGXDLMSObject* CGXDLMSObjectFactory::CreateObject(OBJECT_TYPE type)
{
    if (type == OBJECT_TYPE_ACTION_SCHEDULE)
    {
        return new CGXDLMSActionSchedule();
    }        
    if (type == OBJECT_TYPE_ACTIVITY_CALENDAR)
    {
        return new CGXDLMSActivityCalendar();
    }
    if (type == OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
        return new CGXDLMSAssociationLogicalName();
    }
    if (type == OBJECT_TYPE_ASSOCIATION_SHORT_NAME)
    {
        return new CGXDLMSAssociationShortName();
    }
    if (type == OBJECT_TYPE_AUTO_ANSWER)
    {
        return new CGXDLMSAutoAnswer();
    }
    if (type == OBJECT_TYPE_AUTO_CONNECT)
    {
        return new CGXDLMSAutoConnect();
    }
    if (type == OBJECT_TYPE_CLOCK)
    {
        return new CGXDLMSClock();
    }
    if (type == OBJECT_TYPE_DATA)
    {
        return new CGXDLMSData();
    }
    if (type == OBJECT_TYPE_DEMAND_REGISTER)
    {
        return new CGXDLMSDemandRegister();
    }
    if (type == OBJECT_TYPE_MAC_ADDRESS_SETUP)
    {
        return new CGXDLMSMacAddressSetup();
    }        
    if (type == OBJECT_TYPE_EVENT)
    {
        return new CGXDLMSCustomObject(type);
    }
    if (type == OBJECT_TYPE_EXTENDED_REGISTER)
    {
        return new CGXDLMSExtendedRegister();
    }
    if (type == OBJECT_TYPE_GPRS_SETUP)
    {
        return new CGXDLMSGPRSSetup();
    }
	if (type == OBJECT_TYPE_SECURITY_SETUP)
    {
        return new CGXDLMSSecuritySetup();
    }	
    if (type == OBJECT_TYPE_IEC_HDLC_SETUP)
    {
        return new CGXDLMSHdlcSetup();
    }
    if (type == OBJECT_TYPE_IEC_LOCAL_PORT_SETUP)
    {
        return new CGXDLMSIECOpticalPortSetup();
    }
    if (type == OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP)
    {
        return new CGXDLMSCustomObject(type);
    }
    if (type == OBJECT_TYPE_IP4_SETUP)
    {
        return new CGXDLMSIp4Setup();
    }
    if (type == OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP)
    {
        return new CGXDLMSMBusSlavePortSetup();
    }
    if (type == OBJECT_TYPE_IMAGE_TRANSFER)
    {
        return new CGXDLMSImageTransfer();
    }
    if (type == OBJECT_TYPE_DISCONNECT_CONTROL)
    {
        return new CGXDLMSDisconnectControl();
    }
	if (type == OBJECT_TYPE_LIMITER)
    {
        return new CGXDLMSLimiter();
    }        
    if (type == OBJECT_TYPE_MBUS_CLIENT)
    {
        return new CGXDLMSMBusClient();
    }		
    if (type == OBJECT_TYPE_MODEM_CONFIGURATION)
    {
        return new CGXDLMSModemConfiguration();
    }
    if (type == OBJECT_TYPE_PPP_SETUP)
    {
        return new CGXDLMSPppSetup();
    }
    if (type == OBJECT_TYPE_PROFILE_GENERIC)
    {
        return new CGXDLMSProfileGeneric();
    }        
    if (type == OBJECT_TYPE_REGISTER)
    {
        return new CGXDLMSRegister();
    }
    if (type == OBJECT_TYPE_REGISTER_ACTIVATION)
    {
        return new CGXDLMSRegisterActivation();
    }
    if (type == OBJECT_TYPE_REGISTER_MONITOR)
    {
        return new CGXDLMSRegisterMonitor();
    }
    if (type == OBJECT_TYPE_REGISTER_TABLE)
    {
        return new CGXDLMSCustomObject(type);
    }
    if (type == OBJECT_TYPE_REMOTE_ANALOGUE_CONTROL)
    {
        return new CGXDLMSCustomObject(type);
    }
    if (type == OBJECT_TYPE_REMOTE_DIGITAL_CONTROL)
    {
        return new CGXDLMSCustomObject(type);
    }
    if (type == OBJECT_TYPE_SAP_ASSIGNMENT)
    {
        return new CGXDLMSSapAssignment();
    }
    if (type == OBJECT_TYPE_SCHEDULE)
    {
        return new CGXDLMSSchedule();
    }
    if (type == OBJECT_TYPE_SCRIPT_TABLE)
    {
        return new CGXDLMSScriptTable();
    }
    if (type == OBJECT_TYPE_SMTP_SETUP)
    {
        return new CGXDLMSCustomObject(type);
    }
    if (type == OBJECT_TYPE_SPECIAL_DAYS_TABLE)
    {
        return new CGXDLMSSpecialDaysTable();
    }
    if (type == OBJECT_TYPE_STATUS_MAPPING)
    {
        return new CGXDLMSCustomObject(type);
    }
    if (type == OBJECT_TYPE_TCP_UDP_SETUP)
    {
        return new CGXDLMSTcpUdpSetup();
    }
    if (type == OBJECT_TYPE_TUNNEL)
    {
        return new CGXDLMSCustomObject(type);
    }
    if (type == OBJECT_TYPE_UTILITY_TABLES)
    {
        return new CGXDLMSCustomObject(type);
    }                
    return new CGXDLMSCustomObject(type);
}

