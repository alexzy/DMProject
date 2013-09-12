#ifndef __DATA_H__
#define __DATA_H__

#include <vector>
using namespace std;

class CData
{
public:
	CData();
	~CData();

	void Clear();
	CData& operator+=(const CData &d);
	CData& operator/=(const int &n);

	friend class CInitDataset;
private:
	unsigned m_id;							//编号
	vector<double> m_column;				//字段值数组
	unsigned m_class;						//类或簇
public:
	void Classify(const unsigned c)			{	m_class = c;				};
	void SetCluster(const unsigned c)		{	m_class = c;				};
	unsigned ID()const						{	return m_id;				};
	double Column(const unsigned i)const	{	return m_column[i];			};
	unsigned Size()const					{	return m_column.size();		};
	unsigned Class()const					{	return m_class;				};
	unsigned Cluster()const					{	return m_class;				};
};

#define VECDATA vector<CData>
#endif