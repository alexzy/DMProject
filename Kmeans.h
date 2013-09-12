#ifndef __KMEANS_H__
#define __KMEANS_H__


#include <time.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;
#include "Data.h"

enum KM_METHOD
{
	STD_KMEANS = 1,							//基本K均值
	LED_KMEANS								//Leader初始质心生成
};

class CLeader
{
public:
	friend class CKmeans;
	bool operator<(const CLeader &l)		//for sort
	{
		if(dataNum > l.dataNum)
			return true;
		else
			return false;	
	}
private:
	int index;								//索引
	int dataNum;							//规划数据数
};

class CKmeans
{
public:
	CKmeans(int k, int method=STD_KMEANS, double thres=1.5);
	~CKmeans();

	void Run(const VECDATA &ds);

private:
	void InitCentroids(const VECDATA &ds);						//随机初始化质心
	void ComputeMeans(const VECDATA &ds);						//计算均值作为新质心
	double EuDistance(const CData &d1, const CData &d2);		//计算欧式距离
	void PrintCluster(const VECDATA &ds);						//打印分簇结果
	void PrintEffect(const VECDATA &ods, VECDATA &rds);			//打印结果分析（命中率）

	KM_METHOD m_method;											//方法选项
	double m_leaderThres;										//Leader距离阈值
	int m_clusterNum;											//簇数，即K
	VECDATA m_centroids;										//质心
	vector<vector<int>> m_clusters;								//分簇结果
public:
	char *GetMethod()const;										//取得方法名
};

#endif