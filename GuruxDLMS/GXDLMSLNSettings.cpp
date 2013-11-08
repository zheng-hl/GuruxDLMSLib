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

#include "GXDLMSLNSettings.h"
#include "GXHelpers.h"

CGXDLMSLNSettings::CGXDLMSLNSettings(void)
{
	//Set default values.
	m_ConformanceBlock[0] = 0x00;
	m_ConformanceBlock[1] = 0x7E;
	m_ConformanceBlock[2] = 0x1F;
}

CGXDLMSLNSettings::~CGXDLMSLNSettings(void)
{
}

//Bit 8
bool CGXDLMSLNSettings::GetAttribute0SetReferencing()
{
	return GXHelpers::GetBits(m_ConformanceBlock[1], 0x1);	
}

void CGXDLMSLNSettings::SetAttribute0SetReferencing(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[1], 0x1, newVal);
}

//Bit 9
bool CGXDLMSLNSettings::GetPriorityManagement()
{
	return GXHelpers::GetBits(m_ConformanceBlock[1], 0x2);
}

void CGXDLMSLNSettings::SetPriorityManagement(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[1], 0x2, newVal);
}

//Bit 10
bool CGXDLMSLNSettings::GetAttribute0GetReferencing()
{
	return GXHelpers::GetBits(m_ConformanceBlock[1], 0x4);
}

void CGXDLMSLNSettings::SetAttribute0GetReferencing(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[1], 0x4, newVal);
}

//Bit 11
bool CGXDLMSLNSettings::GetGetBlockTransfer()
{
	return GXHelpers::GetBits(m_ConformanceBlock[1], 0x8);
}

void CGXDLMSLNSettings::SetGetBlockTransfer(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[1], 0x8, newVal);
}

//Bit 12
bool CGXDLMSLNSettings::GetSetBlockTransfer()
{
	return GXHelpers::GetBits(m_ConformanceBlock[1], 0x10);
}

void CGXDLMSLNSettings::SetSetBlockTransfer(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[1], 0x10, newVal);
}

//Bit 13
bool CGXDLMSLNSettings::GetActionBlockTransfer()
{
	return GXHelpers::GetBits(m_ConformanceBlock[1], 0x20);
}

void CGXDLMSLNSettings::SetActionBlockTransfer(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[1], 0x20, newVal);
}

//Bit 14
bool CGXDLMSLNSettings::GetMultibleReferences()
{
	return GXHelpers::GetBits(m_ConformanceBlock[1], 0x40);
}

void CGXDLMSLNSettings::SetMultibleReferences(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[1], 0x40, newVal);
}

//Bit 19
bool CGXDLMSLNSettings::GetGet()
{
	return GXHelpers::GetBits(m_ConformanceBlock[2], 0x8);
}

void CGXDLMSLNSettings::SetGet(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[2], 0x8, newVal);
}

//Bit 20
bool CGXDLMSLNSettings::GetSet()
{
	return GXHelpers::GetBits(m_ConformanceBlock[2], 0x10);
}

void CGXDLMSLNSettings::SetSet(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[2], 0x10, newVal);
}

//Bit 21
bool CGXDLMSLNSettings::GetSelectiveAccess()
{
	return GXHelpers::GetBits(m_ConformanceBlock[2], 0x20);
}

void CGXDLMSLNSettings::SetSelectiveAccess(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[2], 0x20, newVal);
}

//Bit 22
bool CGXDLMSLNSettings::GetAction()
{
	return GXHelpers::GetBits(m_ConformanceBlock[2], 0x40);
}

void CGXDLMSLNSettings::SetAction(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[2], 0x40, newVal);
}

//Bit 23
bool CGXDLMSLNSettings::GetEventNotification()
{
	return GXHelpers::GetBits(m_ConformanceBlock[2], 0x80);
}

void CGXDLMSLNSettings::SetEventNotification(bool newVal)
{
	GXHelpers::SetBits(m_ConformanceBlock[2], 0x80, newVal);
}

