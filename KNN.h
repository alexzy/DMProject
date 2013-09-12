#ifndef __KNN_H__
#define __KNN_H__

#include <math.h>
#include <vector>
#include <iostream>
using namespace std;
#include "Data.h"

enum KNN_METHOD
{
	STD_KNN = 1,				//基本KNN
	DWV_KNN						//距离加权表决
};

class CNeighbor
{
public:
	friend class CKNN;
private:
	unsigned id;				//编号
	double distence;			//距离
};

class CKNN
{
public:
	CKNN(int k=0, int method=STD_KNN);
	~CKNN();

	void Run(const VECDATA &train, const VECDATA &test);

private:
	void InitAlgorithm(const VECDATA &train);
	double EuDistance(const CData &d1, const CData &d2)const;
	void PrintClassification(const VECDATA &test);
	void PrintEffect(const VECDATA &ods, const VECDATA &rds);

	int m_k;													//K值
	KNN_METHOD m_method;										//方法选项
	vector<double> m_classWeights;								//训练集每一类占的权重
	vector<vector<CNeighbor>> m_testDists;						//测试集对训练集距离排序
	vector<vector<unsigned>> m_classes;							//分类结果
public:
	char *GetMethod()const;										//取得方法名
};

#endif