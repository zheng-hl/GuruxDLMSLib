#include "GXStandardObisCode.h"

CGXStandardObisCode::CGXStandardObisCode(vector< basic_string<char> > obis, 
					basic_string<char>& desc, 
					basic_string<char>& interfaces, basic_string<char>& dataType)
{
    m_OBIS.clear();
	m_OBIS.insert(m_OBIS.end(), obis.begin(), obis.end());
    SetDescription(desc);
    SetInterfaces(interfaces);
    SetDataType(dataType);
}

vector< basic_string<char> >& CGXStandardObisCode::GetOBIS()
{
    return m_OBIS;
}

void CGXStandardObisCode::SetOBIS(vector< basic_string<char> >& value)
{
	m_OBIS.clear();
	m_OBIS.insert(m_OBIS.end(), value.begin(), value.end());
}

/** 
 m_OBIS code description.
*/
basic_string<char>& CGXStandardObisCode::GetDescription()
{
    return m_Description;
}
void CGXStandardObisCode::SetDescription(basic_string<char> value)
{
    m_Description = value;
}

/** 
 m_Interfaces that are using this m_OBIS code.
*/
basic_string<char>& CGXStandardObisCode::GetInterfaces()
{
    return m_Interfaces;
}
void CGXStandardObisCode::SetInterfaces(basic_string<char> value)
{
    m_Interfaces = value;
}

/** 
 Standard data types.
*/        
basic_string<char>& CGXStandardObisCode::GetDataType()
{
    return m_DataType;
}

void CGXStandardObisCode::SetDataType(basic_string<char> value)
{
    m_DataType = value;
}