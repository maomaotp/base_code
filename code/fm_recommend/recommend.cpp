#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

//从TXT中读入数据到矩阵（二维数组）
template <typename T>
vector<vector<T> > txtRead(string FilePath,int row,int col)
{
    ifstream input(FilePath);
    if (!input.is_open())
    {
        cerr << "File is not existing, check the path: \n" <<  FilePath << endl;
        exit(1);
    }
    vector<vector<T> > data(row, vector<T>(col,0)); 
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < col; ++j)
        {
            //因为这里针对的情况是用户只给出对items的喜欢与不喜欢的情况，而movielens
            //是一个1-5的评分数据，所以把分数达到3的看作是喜欢，标记为1，小于3的视为
            // 不喜欢，置为0
            input >> data[i][j];
            if (data[i][j] >= 3)
                data[i][j] = 1;
            else
                data[i][j] = 0;
        }
    }
    return data;
}

//把矩阵中的数据写入TXT文件
template<typename T>
void txtWrite(vector<vector<T> > Matrix, string dest)
{
    ofstream output(dest);
    vector<vector<T> >::size_type row = Matrix.size();
    vector<T>::size_type col = Matrix[0].size();
    for (vector<vector<T> >::size_type i = 0; i < row; ++i)
    {
        for (vector<T>::size_type j = 0; j < col; ++j)
        {
            
            output << setprecision(3)<< Matrix[i][j] << "\t";
        }
        output << endl;
    }
}

// 求两个向量的内积
double InnerProduct(std::vector<double> A, std::vector<double> B)
{
    double res = 0;
    for(std::vector<double>::size_type i = 0; i < A.size(); ++i)
    {
        res += A[i] * B[i];
    }
    return res;
}

//矩阵转置操作
template<typename T>//
vector<vector<T> > Transpose(vector<vector<T> > Matrix)
{
    unsigned row = Matrix.size();
    unsigned col = Matrix[0].size();
    vector<vector<T> > Trans(col,vector<T>(row,0));
    for (unsigned i = 0; i < col; ++i)
    {
        for (unsigned j = 0; j < row; ++j)
        {
            Trans[i][j] = Matrix[j][i];
        }
    }
    return Trans;
}

//求一个向量中所有元素的和
template<typename T>
T SumVector(vector<T> vec)
{
    T res = 0;

    for (vector<T>::size_type i = 0; i < vec.size(); ++i)
        res += vec[i];
    return res;
}

//对一个向量中的元素进行降序排列，返回重排后的元素在原来
//向量中的索引
bool IsBigger(double a, double b)
{
    return a >= b;
}
vector<unsigned> DescendVector(vector<double> vec)
{
    vector<double> tmpVec = vec;
    sort(tmpVec.begin(), tmpVec.end(), IsBigger);
    vector<unsigned> idx;
    for (vector<double>::size_type i = 0; i < tmpVec.size(); ++i)
    {
        for (vector<double>::size_type j = 0; j < vec.size(); ++j)
        {
            if (tmpVec[i] == vec[j])
                idx.push_back(j);
        }
    }
    return idx;
}


//基于概率传播(ProbS)的二部图的推荐函数，data是训练数据
vector<vector<double> > ProbS(vector<vector<double> > data)
{
    auto row = data.size();
    auto col = data[0].size();
    vector<vector<double> > transData = Transpose(data);

    //第一步利用概率传播机制计算权重矩阵
    vector<vector<double> > weights(col, vector<double>(col, 0));
    for (vector<double>::size_type i = 0; i < col; ++i)
    {
        for (vector<double>::size_type j = 0; j < col; ++j)
        {
            double degree = SumVector<double>(transData[j]);
            double sum = 0;
            for (vector<double>::size_type k = 0; k < row; ++k)
            {
                sum += transData[i][k] * transData[j][k] / SumVector<double>(data[k]);
            }
            if (degree)
                weights[i][j] = sum / degree;
        }
    }

    //第二步利用权重矩阵和训练数据集针对每个用户对每一个item评分
    vector<vector<double> > scores(row, vector<double>(col, 0));
    for (vector<double>::size_type i = 0; i < row; ++i)
    {
        for (vector<double>::size_type j = 0; j < col; ++j)
        {
            //等于0的地方代表user i 还木有评价过item j,需要预测
            if (0 == data[i][j])
                scores[i][j] = InnerProduct(weights[j],data[i]);
        }
    }
    return scores;
}

//计算推荐结果的命中率:推荐的items中用户确实喜欢的items数量/推荐的items数量
//用户确实喜欢的items是由测试集给出,length表示推荐列表最长为多少，这里将测出
//推荐列表长度由1已知增加到length过程中，推荐命中率的变化
vector<vector<double> > ComputeHitRate(vector<vector<double> > scores, vector<vector<double> > test,
    unsigned length)
{
    auto usersNum = test.size();
    auto itemsNum = test[0].size();
    
    vector<vector<unsigned> > sortedIndex;
    //因为只是对测试集中的用户和items进行测试，于是选取与测试集大小一样的预测数据
    vector<vector<double> > selectedScores(usersNum, vector<double>(itemsNum,0));
    vector<double> line;
    for (unsigned i = 0; i < usersNum; ++i)
    {
        for (unsigned j = 0; j < itemsNum; ++j)
        {
            line.push_back(scores[i][j]);
        }
        sortedIndex.push_back(DescendVector(line));
        line.clear();
    }
    //hitRate的第一列存储推荐列表的长度，第二列存储对应的命中率
    vector<vector<double> > hitRate(length);
    for (unsigned k = 1; k <= length; ++k)
    {
        hitRate[k-1].push_back(k);
        double Counter = 0;
        for (unsigned i = 0; i < usersNum; ++i)
        {
            for (unsigned j = 0; j < k; ++j)
            {
                unsigned itemIndex = sortedIndex[i][j];
                if (test[i][itemIndex])    
                    ++Counter;
            }
        }
        hitRate[k-1].push_back(Counter / (k * usersNum));
    }
    return hitRate;
}
int main()
{
    string FilePath1("data.txt");
    //string FilePath2("E:\\Matlab code\\recommendation system\\data\\movielens\\test.txt");
    
    int row = 10;
    int col = 10;
    cout << "数据读取中..." << endl;
    vector<vector<double> > train = txtRead<double>(FilePath1, row, col);
    //vector<vector<double> > test = txtRead<double>(FilePath2, 462, 1591);

    cout << "利用二部图网络进行评分预测..." << endl;
    vector<vector<double> > predictScores = ProbS(train);
    txtWrite(predictScores, "predictScores.txt");
    /*
    cout << "计算命中率..." << endl;
    vector<vector<double> > hitRate = ComputeHitRate(predictScores, test, 1591);

    txtWrite(hitRate, "hitRate.txt");
    cout << "命中率结果保存完毕！" << endl;
    */
    return 0;
}
