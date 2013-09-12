#include "Data.h"

CData::CData()
{
	m_id = 0;
	m_class = 0;
}
CData::~CData()
{
}
void CData::Clear()
{
	for(int i=0; i != m_column.size(); ++i)
	{
		m_column[i] = 0.0;
	}
}
CData& CData::operator+=(const CData &d)
{
	for(int i=0; i!=m_column.size(); ++i)
	{
		m_column[i] += d.m_column[i];
	}
	return *this;
}
CData& CData::operator/=(const int &n)
{
	for(int i=0; i!=m_column.size(); ++i)
	{
		m_column[i] /= n;
	}
	return *this;
}