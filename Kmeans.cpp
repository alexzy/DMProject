#include "Kmeans.h"

CKmeans::CKmeans(int k, int method, double thres)
{
	//K值必须不小于2，默认为2
	m_clusterNum = (k<2) ? 2 : k;
	m_leaderThres = thres;
	//规范化method
	switch(method)
	{
	case 1:
		m_method = STD_KMEANS;
		break;
	case 2:
		m_method = LED_KMEANS;
		break;
	default:
		m_method = STD_KMEANS;
	}
}
CKmeans::~CKmeans()
{
}

void CKmeans::Run(const VECDATA &ds)
{
	if(ds.empty())
	{
		cout<<"数据集为空！"<<endl;
		return;
	}
	VECDATA cluster = ds;
	int clusterSize = cluster.size();
	//簇数不能大于数据数
	if(m_clusterNum > clusterSize)
		m_clusterNum = clusterSize;
	cout<<"**************************************"<<endl;
	cout<<"开始K均值聚类：数据="<<clusterSize<<", K="<<m_clusterNum<<", Method="<<GetMethod()<<endl;
	//随机初始化K个质心至m_centroids数组
	InitCentroids(cluster);
	//重复聚类直至分簇不再变化
	cout<<"计算中..."<<endl;
	int times = 1;
	bool bChange = false;
	while(times<100)
	{
		//每个数据与质心计算欧式距离，加入距离最近的簇
		for(int i=0; i!= clusterSize; ++i)
		{
			double dis = EuDistance( cluster[i], m_centroids[0] );
			int c = 0;
			for(int j=1; j!=m_centroids.size(); ++j)
			{
				double dis1 = EuDistance( cluster[i], m_centroids[j] );
				if(dis1 < dis)
				{
					dis = dis1;
					c = j;
				}
			}
			//分簇有变化
			if( c != cluster[i].Cluster() )
				bChange = true;
			cluster[i].SetCluster(c);
		}
		//若分簇不变，则停止重复聚类
		if(!bChange)
			break;
		//重新选择质心（求均值）
		ComputeMeans(cluster);
		bChange = false;
		times++;
	}
	//结果分析
	cout<<"聚类结果："<<"times="<<times<<endl;
	PrintCluster(cluster);
	PrintEffect(ds, cluster);
}

void CKmeans::InitCentroids(const VECDATA &ds)
{
	m_centroids.clear();
	m_clusters.clear();
	//随机种子
	srand((unsigned)time(NULL));
	int dsSize = ds.size();
	vector<CLeader> leaders;
	if(m_method == LED_KMEANS)
	{
		//随机产生一个Leader
		CLeader leader;
		leader.index = rand()%ds.size();
		leader.dataNum = 0;
		leaders.push_back(leader);
		//对每个数据项规划Leader
		for(int i=0; i!=dsSize; ++i)
		{
			double dis = EuDistance(ds[(leaders[0].index)], ds[i]);;
			int leaderSize = leaders.size();
			int ownerIndex = 0;
			for(int j=1; j!=leaderSize; ++j)
			{
				int lIndex = leaders[j].index;
				double dis1 = EuDistance(ds[lIndex], ds[i]);
				if(dis1 < dis)
				{
					dis = dis1;
					ownerIndex = j;
				}
			}
			//与Leader的距离若小于阈值，则加入最近的Leader，否则创建一个新Leader
			if(dis > m_leaderThres)
			{
				CLeader ld;
				ld.index = i;
				ld.dataNum = 1;
				leaders.push_back(ld);
			}
			else
			{
				leaders[ownerIndex].dataNum ++;
			}
		}
		//找数据数最多的前K个Leader（先排序）
		sort(leaders.begin(), leaders.end());
		cout<<"Leaders=";
	}
	int leaderSize = leaders.size();
	for(int i=0; i<m_clusterNum; ++i)
	{
		if(i<leaderSize)	//使用Leader初始化质心
		{
			int index = leaders[i].index;
			cout<<"["<<ds[index].ID()<<"] ";
			m_centroids.push_back(ds[index]);
			vector<int> tmp;
			m_clusters.push_back(tmp);
		}
		else				//使用随机编号初始化质心
		{
			int ranIndex = rand()%dsSize;
			//检查产生的编号是否已经被选作为质心
			int j = 0;
			for(; j<i; ++j)
			{
				if(ds[ranIndex].ID() == m_centroids[j].ID())
					break;
			}
			//若不重复则加入，否则重新生成
			if(j >= i)
			{
				m_centroids.push_back(ds[ranIndex]);
				vector<int> tmp;
				m_clusters.push_back(tmp);
			}
			else
				--i;
		}
	}
	if(m_method == LED_KMEANS) cout<<endl;
}
void CKmeans::ComputeMeans(const VECDATA &ds)
{
	//每个簇计数
	vector<int> countInClusters;
	int centroidSize = m_centroids.size();
	int dSize = ds.size();
	for(int i=0; i!=centroidSize; ++i)
	{
		countInClusters.push_back(0);
		m_centroids[i].Clear();
	}
	//统计各个簇之和
	for(int i=0; i!=dSize; ++i)
	{
		int c = ds[i].Cluster();
		m_centroids[c] += ds[i];
		countInClusters[c] ++;
	}
	//计算均值
	for(int i=0; i!=centroidSize; ++i)
	{
		m_centroids[i] /= countInClusters[i];
	}
}
double CKmeans::EuDistance(const CData &d1, const CData &d2)
{
	double rd = 0.0;
	int size = d1.Size();
	for(int i=0; i!=size; ++i)
	{
		rd += (d1.Column(i)-d2.Column(i)) * (d1.Column(i)-d2.Column(i));
	}
	return sqrt(rd);
}
void CKmeans::PrintCluster(const VECDATA &cluster)
{
	//按行打印所有数据的簇标识
	int clusterSize = cluster.size();
	for(int i=0; i!=clusterSize; ++i)
	{
		int id = cluster[i].ID();
		int c = cluster[i].Cluster();
		m_clusters[c].push_back(id);		//用于以后打印分簇结果
		if( id%25 == 1 )
		{
			cout << "[" << id-1 <<"] ";
		}
		char a = 'A'+c;
		cout << a << " ";
		if( id%25 == 0 )
		{
			cout << endl;
		}
	}
	cout << endl;
	//打印分簇结果
	for(int i=0; i!=m_clusterNum; ++i)
	{
		cout<<"Cluster "<<i+1<<":"<<endl;
		int preNum = -1;
		int curNum;
		bool bSkip = false;
		for(int j=0; j != m_clusters[i].size(); ++j)
		{
			curNum = m_clusters[i][j];
			if(curNum != preNum+1)
			{
				if(preNum != -1)
				{
					if(!bSkip)
						cout<<"-"<<preNum<<", "<<curNum;
					else
						cout<<", "<<curNum;
				}
				else
					cout<<curNum;
				bSkip = true;
			}
			else if(!bSkip)
			{
				cout<<"-";
				if(j == m_clusters[i].size()-1)
					cout<<curNum;
			}
			else
			{
				bSkip = false;
			}
			preNum = curNum;
		}
		cout<<endl;
	}
	cout << endl;
}
void CKmeans::PrintEffect(const VECDATA &ods, VECDATA &rds)
{
	//每簇的成分统计
	vector<vector<int>> compos;
	for(int i=0; i<m_clusterNum; ++i)
	{
		vector<int> tmpv;
		compos.push_back(tmpv);
	}

	int size = rds.size();
	for(int i=0; i!=size; ++i)
	{
		int rc = rds[i].Cluster();
		int oc = ods[i].Class()-1;
		//对于每一簇，若要统计的成分编号大于compos[rc]大小，则要为compos[rc]补充元素。
		int hastoAppend = oc - compos[rc].size();
		while(hastoAppend >= 0)
		{
			int t = 0;
			compos[rc].push_back(t);
			hastoAppend--;
		}
		compos[rc][oc] ++;
	}
	//找每簇主成分，并计算比例
	int hits = 0;
	for(int i=0; i<m_clusterNum; ++i)
	{
		int mainCNum = 0;
		for(int j=0; j != compos[i].size(); ++j)
		{
			if( compos[i][j] > mainCNum )
				mainCNum = compos[i][j];
		}
		hits += mainCNum;
	}
	cout << "结果分析：Average Hits = " << hits*100.0/size <<"%"<< endl << endl;;
}
char *CKmeans::GetMethod()const
{
	switch(m_method)
	{
	case STD_KMEANS:
		return "Standard";
	case LED_KMEANS:
		return "LK-means";
	default:
		return "?";
	}
}