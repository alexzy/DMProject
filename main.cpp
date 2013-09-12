#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
#include "Data.h"
#include "Kmeans.h"
#include "KNN.h"

VECDATA g_dataset;			//数据集
VECDATA g_trainingSet;		//训练集
VECDATA g_testSet;			//测试集

class CInitDataset
{
public:
	static bool LoadFile(const char *fileName);
	static void InitTrainingSet(double ratio, int method);
};

int main()
{
	cout<<"**** DM Project by 郑越 S11102035 ****"<<endl;
	//读取文件
	if(!CInitDataset::LoadFile("iris.txt"))
	{
		return -1;
	}
	while(1)
	{
		cout<<"**************************************"<<endl;
		cout<<"请输入数字选择："<<endl;
		cout<<"1. 分类(KNN)(默认)"<<endl;
		cout<<"2. 聚类(K-means)"<<endl;
		cout<<"0. 退出"<<endl;
		int menu = 1;
		cin.unsetf(ios::skipws);
		cin>>menu;
		cin.clear();	cin.sync();
		if(menu == 1)
		{
			cout<<"分类请输入训练集比例(默认0.5)：";
			double trainRatio = 0.5;
			cin>>trainRatio;
			cin.clear();	cin.sync();
			if(trainRatio>1 || trainRatio<0)
			{
				cout<<"输入有误，将使用默认比例0.5！"<<endl;
				trainRatio = 0.5;
			}
			cout<<endl;
			int tr = (int)(100 * trainRatio);
			cout<<"请选择训练集生成方式(默认前"<<tr<<"%)："<<endl;
			cout<<"1. 总数据前"<<tr<<"%"<<endl;
			cout<<"2. 总数据后"<<tr<<"%"<<endl;
			cout<<"3. 随机产生约"<<tr<<"%"<<endl;
			int initMethod = 1;
			cin>>initMethod;
			cin.clear();	cin.sync();
			if(initMethod!=1 && initMethod!=2 && initMethod!=3)
			{
				cout<<"输入有误，将默认使用总数据的前"<<tr<<"%！"<<endl<<endl;;
				initMethod = 1;
			}
			CInitDataset::InitTrainingSet(trainRatio, initMethod);
			cin.clear();	cin.sync();
			cout<<"请输入K(默认为少数类的数据数)：";
			int k = 0;
			cin>>k;
			cin.clear();	cin.sync();
			cout<<endl;
			cout<<"请选择表决方法(默认为标准)："<<endl;
			cout<<"1. 标准KNN"<<endl;
			cout<<"2. 使用距离加权表决"<<endl;
			int method = STD_KNN;
			cin>>method;
			cin.clear();	cin.sync();
			cout<<endl;
			//分类
			CKNN knn(k, method);
			knn.Run(g_trainingSet, g_testSet);
			system("pause");
		}
		else if(menu == 2)
		{
			cout<<"请输入K(默认为3)：";
			int k = 3;
			cin>>k;
			cin.clear();	cin.sync();
			cout<<endl;
			cout<<"请选择质心初始化方法(默认为标准)："<<endl;
			cout<<"1. 标准K-means"<<endl;
			cout<<"2. Leader K-means"<<endl;
			int method = STD_KMEANS;
			cin>>method;
			cin.clear();	cin.sync();
			cout<<endl;
			double thres = 1.5;
			if(method == LED_KMEANS)
			{
				cout<<"请输入Leader距离阈值(默认为1.5)：";
				cin>>thres;
				cin.clear();	cin.sync();
				cout<<endl;
			}
			//聚类
			CKmeans kmeans(k, method, thres);
			kmeans.Run(g_dataset);
			system("pause");
		}
		else if(menu == 0)
		{
			break;
		}
	
	}
	system("pause");
	return 0;
}

bool CInitDataset::LoadFile(const char *fileName)
{
	ifstream file(fileName, ios::in);
	if(file.fail())
	{
		cout<<"打开文件失败！"<<endl;
		system("pause");
		return false;
	}
	string tmpStr;
	int columnNum = 0, colTest = 0;
	bool bFirst = true;
	while(getline(file, tmpStr))
	{
		istringstream line(tmpStr);
		CData data;
		//第一个是编号
		line >> data.m_id;
		//后面是字段，判断所有数据的字段数是否一致
		double x;
		while(line >> x)
		{
			data.m_column.push_back(x);
			//记录第一条数据的字段数
			if(bFirst)
				columnNum++;
			else
				colTest++;
		}
		if(!bFirst && colTest != columnNum)
		{
			cout<<"数据字段数不一致！"<<endl;
			system("pause");
			return false;
		}
		g_dataset.push_back(data);
		bFirst = false;
		colTest = 0;
	}
	if(g_dataset.size() == 0 || columnNum == 0)
	{
		cout<<"未读取到任何数据！"<<endl;
		system("pause");
		return false;
	}
	int datasetSize = g_dataset.size();
	cout<<"从"<<fileName<<"读取"<<datasetSize<<"条数据，字段数="<<columnNum<<endl;
	file.close();

	//设置默认分类
	for(int i=0; i!=datasetSize; ++i)
	{
		int id = g_dataset[i].m_id;
		if( (id>0 && id<26) || (id>75 && id<101) )
			g_dataset[i].m_class = 1;
		if( (id>25 && id<51) || (id>100 && id<126) )
			g_dataset[i].m_class = 2;
		if( (id>50 && id<76) || (id>125 && id<151) )
			g_dataset[i].m_class = 3;
	}
	return true;
} 

void CInitDataset::InitTrainingSet(double ratio, int method)
{
	//初始化训练集、测试集
	int datasetSize = g_dataset.size();
	int trainNum = (int)(datasetSize*ratio);
	if(method == 3)
		srand((unsigned)time(NULL));
		
	g_trainingSet.clear();
	g_testSet.clear();
	for(int i=0; i!=datasetSize; ++i)
	{
		int id = g_dataset[i].m_id;
		if(method == 1)
		{
			if(id < trainNum+1)
			{
				g_trainingSet.push_back(g_dataset[i]);
			}
			else
			{
				g_testSet.push_back(g_dataset[i]);
			}
		}
		if(method == 2)
		{
			if(id > (datasetSize-trainNum) )
			{
				g_trainingSet.push_back(g_dataset[i]);
			}
			else
			{
				g_testSet.push_back(g_dataset[i]);
			}
		}
		if(method == 3)
		{
			if(rand()%datasetSize < trainNum)
			{
				g_trainingSet.push_back(g_dataset[i]);
			}
			else
			{
				g_testSet.push_back(g_dataset[i]);
			}
		}
	}
}


