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

#include "GXDLMSSNSettings.h"
#include "GXHelpers.h"

CGXDLMSSNSettings::CGXDLMSSNSettings(void)
{
	//Set default values.
	m_ConformanceBlock[0] = 0x1C;
	m_ConformanceBlock[1] = 0x03;
	m_ConformanceBlock[2] = 0x20;
}

CGXDLMSSNSettings::~CGXDLMSSNSettings(void)
{
}

//Bit 3
bool CGXDLMSSNSettings::GetRead()
{
	return GXHelpers::GetBits(m_ConformanceBlock[0], 0x8);
}

void CGXDLMSSNSettings::SetRead(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[0], 0x8, newVal);	
}

//Bit 4
bool CGXDLMSSNSettings::GetWrite()
{
	return GXHelpers::GetBits(m_ConformanceBlock[0], 0x10);
}

void CGXDLMSSNSettings::SetWrite(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[0], 0x10, newVal);
}

//Bit 5
bool CGXDLMSSNSettings::GetUnconfirmedWrite()
{
	return GXHelpers::GetBits(m_ConformanceBlock[0], 0x20);
}

void CGXDLMSSNSettings::SetUnconfirmedWrite(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[0], 0x20, newVal);
}

//Bit 14
bool CGXDLMSSNSettings::GetMultibleReferences()
{
	return GXHelpers::GetBits(m_ConformanceBlock[1], 0x40);
}

void CGXDLMSSNSettings::SetMultibleReferences(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[1], 0x40, newVal);
}

//Bit 15
bool CGXDLMSSNSettings::GetInformationReport()
{
	return GXHelpers::GetBits(m_ConformanceBlock[1], 0x80);
}

void CGXDLMSSNSettings::SetInformationReport(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[1], 0x20, newVal);
}

//Bit 18
bool CGXDLMSSNSettings::GetParametrizedAccess()
{
	return GXHelpers::GetBits(m_ConformanceBlock[2], 0x4);
}

void CGXDLMSSNSettings::SetParametrizedAccess(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[2], 0x4, newVal);
}
