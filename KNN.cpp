#include "KNN.h"

CKNN::CKNN(int k, int method)
{
	//K值必须不小于0，默认为0
	m_k = (k<0) ? 0 : k;
	//规范化method
	switch(method)
	{
	case 1:
		m_method = STD_KNN;
		break;
	case 2:
		m_method = DWV_KNN;
		break;
	default:
		m_method = STD_KNN;
	}
}
CKNN::~CKNN()
{
}

void CKNN::Run(const VECDATA &train, const VECDATA &test)
{
	if(train.empty())
	{
		cout<<"训练集为空！"<<endl;
		return;
	}
	if(test.empty())
	{
		cout<<"测试集为空！"<<endl;
		return;
	}
	//初始化一些算法变量
	InitAlgorithm(train);
	int trainSize = train.size();
	int testSize = test.size();
	cout<<"**************************************"<<endl;
	cout<<"开始KNN分类：训练="<<trainSize<<", 测试="<<testSize<<", K="<<m_k<<", Method="<<GetMethod()<<endl;
	cout<<"计算中..."<<endl;
	//每个测试数据与训练数据计算欧氏距离，按距离插入排序
	m_testDists.reserve(testSize);
	for(int i=0; i!=testSize; ++i)
	{
		vector<CNeighbor> everyTest;
		everyTest.reserve(testSize);
		for(int j=0; j!=trainSize; ++j)
		{
			CNeighbor nb;
			nb.id = j;
			nb.distence = EuDistance(test[i], train[j]);
			//插入
			vector<CNeighbor>::iterator iter = everyTest.begin();
			vector<CNeighbor>::iterator itE = everyTest.end();
			for(; iter != itE; ++iter)
			{
				if(nb.distence < (*iter).distence)
					break;
			}
			everyTest.insert(iter, nb);
		}
		m_testDists.push_back(everyTest);
	}
	//每个测试数据取距离最近的K个训练数据，统计占多数的类
	VECDATA testSet = test;
	for(int i=0; i!=testSize; ++i)
	{
		vector<double> classStat;
		for(int j=0; j!=trainSize; ++j)
		{
			if(j>=m_k)		//前k个
				break;
			int trainID = m_testDists[i][j].id;
			double dist = m_testDists[i][j].distence;
			int c = train[trainID].Class();
			int hastoAppend = c - classStat.size();
			while(hastoAppend >= 0)
			{
				int tmp = 0;
				classStat.push_back(tmp);
				hastoAppend--;
			}
			//距离加权表决
			if(m_method == STD_KNN)
				classStat[c] += 1.0;
			else if(m_method == DWV_KNN)
				classStat[c] = classStat[c] + 1.0/dist/dist;
		}
		//多数表决（要乘以类权值）
		int maxClass = 0;
		double maxClassStat = 0.0;
		for(int j=1; j!=classStat.size(); ++j)
		{
			classStat[j] = classStat[j]*m_classWeights[j];
			if(classStat[j] > maxClassStat)
			{
				maxClassStat = classStat[j];
				maxClass = j;
			}
		}
		//设置分类
		testSet[i].Classify(maxClass);
	}
	//结果分析
	cout<<"分类结果："<<endl;
	PrintClassification(testSet);
	PrintEffect(test, testSet);
}
void CKNN::InitAlgorithm(const VECDATA &train)
{
	int size = train.size();
	//K值必须小于训练集
	if(m_k > size)
		m_k = size;
	m_classes.clear();
	m_classWeights.clear();
	for(int i=0; i!=size; ++i)
	{
		//统计每类的权重
		int c = train[i].Class();
		int hastoAppend = c - m_classWeights.size();
		while(hastoAppend >= 0)
		{
			vector<unsigned> tmp;
			m_classes.push_back(tmp);
			double d = 0.0;
			m_classWeights.push_back(d);
			hastoAppend--;
		}
		m_classWeights[c] += 1.0;
	}

	//求权值
	int minc = size;
	for(int i=0; i!=m_classWeights.size(); ++i)
	{
		if(m_classWeights[i] < 1.0)
			continue;
		if(m_classWeights[i] < minc)
			minc = (int)m_classWeights[i];
		m_classWeights[i] = size/m_classWeights[i];
	}
	//若K为0，初始化一个合适K值，选占比例最小的类个数作为K值
	if(m_k == 0)
		m_k = minc;
}
double CKNN::EuDistance(const CData &d1, const CData &d2)const
{
	double rd = 0.0;
	int size = d1.Size();
	for(int i=0; i!=size; ++i)
	{
		rd += (d1.Column(i)-d2.Column(i)) * (d1.Column(i)-d2.Column(i));
	}
	return sqrt(rd);
}
void CKNN::PrintClassification(const VECDATA &test)
{
	//整理结果
	int testSize = test.size();
	for(int i=0; i!=testSize; ++i)
	{
		int c = test[i].Class();
		m_classes[c].push_back(test[i].ID());
	}
	//打印
	for(int i=1; i!=m_classes.size(); ++i)
	{
		int count = m_classes[i].size();
		if(count == 0)
			continue;
		cout<<"Classification "<<i<<":"<<endl;
		for(int j=0; j!=count; ++j)
		{
			cout<<m_classes[i][j]<<" ";
			if(j%5 == 4 || j == count-1)
				cout<<endl;
		}
		cout<<endl;
	}
}
void CKNN::PrintEffect(const VECDATA &ods, const VECDATA &rds)
{
	//比较分类标识
	int size = ods.size();
	int hits = 0;
	for(int i=0; i!=size; ++i)
	{
		if(ods[i].Class() == rds[i].Class())
		{
			hits++;
		}
	}
	cout<<"结果分析：Accuracy = "<<hits*100.0/size<<"%"<<endl<<endl;
}
char *CKNN::GetMethod()const
{
	switch(m_method)
	{
	case STD_KNN:
		return "Standard";
	case DWV_KNN:
		return "Distance-Weighted Voting";
	default:
		return "?";
	}
}