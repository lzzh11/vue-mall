#include<iostream>
#include<stdio.h>
#include<string>
#include<string.h>
#include<cmath>
#include<ctime>
#include<stdlib.h>
using namespace std;
int seed = (int)time(0);//产生随机种子
int cityposition[30][2] = { {87,7},{91,38},{83,46},{71,44},{64,60},{68,58},{83,69},{87,76},{74,78},{71,71},{58,69},{54,62},{51,67},{37,84},{41,94},{2,99},{7,64},{22,60},{25,62},{18,54},{4,50},{13,40},{18,40},{24,42},{25,38},{41,26},{45,21},{44,35},{58,35},{62,32} };
#define CITY_NUM 30//城市数量
#define ANT_NUM 100//蚁群数量
#define TMAC 1000//迭代最大次数
#define ROU 0.5//误差大小
#define ALPHA 1//信息素重要程度的参数
#define BETA 4//启发式因子重要程度的参数
#define Q 100//信息素残留参数
#define maxn 100
#define inf 0x3f3f3f3f
double dis[maxn][maxn];//距离矩阵
double info[maxn][maxn];//信息素矩阵
bool vis[CITY_NUM][CITY_NUM];//标记矩阵

//返回指定范围内的随机整数
int rnd(int low, int upper)
{
    return low + (upper - low) * rand() / (RAND_MAX + 1);//rand()/(RAND_MAX+1)为0-1间随机数
}

//返回指定范围内的随机浮点数
double rnd(double low, double upper)
{
    double temp = rand() / ((double)RAND_MAX + 1.0);//rand()/(RAND_MAX+1)为0-1间随机数
    return low + temp * (upper - low);
}

struct Ant//蚂蚁结构体
{
    int path[CITY_NUM];//保存蚂蚁走的路径
    double length1;//路径总长度
    bool vis[CITY_NUM];//标记走过的城市
    int current;//当前城市
    int moved_cnt;//已走城市数量

    //初始化
    void init()
    { 
        memset(vis, false, sizeof(vis));//初始化函数，vis为要被填充的内存块，false为设置的值，sizeof(vis)为设置该值的字符数
        length1 = 0;//路径总长度为0
        current = rnd(0, CITY_NUM);//随机整数
        path[0] = current;//走过的路径只包含current
        vis[current] = true;//标记走过的城市current为真
        moved_cnt = 1;//已走城市数量为1
    }

    //选择下一个城市
    int choose()
    {
        int select_city = -1;//所选城市编号
        double sum = 0;//总信息素
        double posb[CITY_NUM];//保存每个城市被选中的概率
        for (int i = 0; i < CITY_NUM; i++)
        {
            if (!vis[i])
            {
                posb[i] = pow(info[current][i], ALPHA) * pow(1.0 / dis[current][i], BETA);//计算每个城市被选中的概率
                sum = sum + posb[i];
            }
            else
            {
                posb[i] = 0;
            }
        }
        //进行轮盘选择
        double temp = 0;
        if (sum > 0)//总的信息素大于0
        {
            temp = rnd(0.0, sum);//取随机整数
            for (int i = 0; i < CITY_NUM; i++)
            {
                if (!vis[i])
                {
                    temp = temp - posb[i];//查找具体的i值，选择城市
                    if (temp < 0.0)
                    {
                        select_city = i;
                        break;
                    }
                }
            }
        }
        else //信息素太少就随便找个没走过的城市
        {
            for (int i = 0; i < CITY_NUM; i++)
            {
                if (!vis[i])
                {
                    select_city = i;
                    break;
                }
            }
        }
        return select_city;
    }

    //蚂蚁的移动
    void move()
    {
        int nex_cityno = choose();//下一个城市存在nex_cityno
        path[moved_cnt] = nex_cityno;
        vis[nex_cityno] = true;
        length1 = length1 + dis[current][nex_cityno];
        current = nex_cityno;
        moved_cnt++;
    }

    //蚂蚁进行一次迭代
    void search()
    {
        init();
        while (moved_cnt < CITY_NUM)
        {
            move();
        }
        //回到原来的城市
        length1 = length1 + dis[path[CITY_NUM - 1]][path[0]];
    }
};

struct TSP//蚁群算法
{
    Ant ants[ANT_NUM];//定义蚁群
    Ant ant_best;//最优路径蚂蚁

    void Init()
    {
        //初始化为最大值
        ant_best.length1 = inf;
        //计算两两城市间距离
        for (int i = 0; i < CITY_NUM; i++)
        {
            for (int j = 0; j < CITY_NUM; j++)
            {
                info[i][j] = 1.0;
                double temp1 = double(cityposition[j][0]) - double(cityposition[i][0]);
                double temp2 = double(cityposition[j][1]) - double(cityposition[i][1]);
                dis[i][j] = sqrt(temp1 * temp1 + temp2 * temp2);//距离公式
            }
        }
    }

    //更新信息素
    void updateinfo()
    {
        double temp_info[CITY_NUM][CITY_NUM];
        memset(temp_info, 0, sizeof(temp_info));
        int u, v;
        for (int i = 0; i < ANT_NUM; i++) //遍历每一只蚂蚁
        {
            for (int j = 1; j < CITY_NUM; j++)
            {
                u = ants[i].path[j - 1];
                v = ants[i].path[j];
                temp_info[u][v] = temp_info[u][v] + Q / ants[i].length1;
                temp_info[v][u] = temp_info[u][v];
            }
            //最后城市和开始城市之间的信息素
            u = ants[i].path[0];
            temp_info[u][v] = temp_info[u][v] + Q / ants[i].length1;
            temp_info[v][u] = temp_info[u][v];
        }
        for (int i = 0; i < CITY_NUM; i++)
        {
            for (int j = 0; j < CITY_NUM; j++)
            {
                info[i][j] = info[i][j] * ROU + temp_info[i][j];
            }
        }
    }
    void Search()
    {
        //迭代TMAC次
        for (int i = 0; i < TMAC; i++)
        {
            //所有蚂蚁都进行一次遍历
            for (int j = 0; j < ANT_NUM; j++)
            {
                ants[j].search();
            }
            //保存所有蚂蚁遍历的最佳结果
            for (int j = 0; j < ANT_NUM; j++)
            {
                if (ant_best.length1 > ants[j].length1)
                {
                    ant_best = ants[j];
                }
            }
            updateinfo();
            //printf("第%d次迭代最优路径长度是%lf\n", i, ant_best.length1);
            cout << "第" << i << "次迭代最优路径长度是" << ant_best.length1 << endl;
            cout << endl;
        }
    }
};
int main()
{
    srand(seed);
    TSP tsp;
    tsp.Init();
    tsp.Search();
    //printf("最短路径如下\n");
    cout << "最短路径如下：" << endl;
    for (int i = 0; i < CITY_NUM; i++)
    {
        //printf("%d", tsp.ant_best.path[i]);
        cout << tsp.ant_best.path[i];
        if (i < CITY_NUM - 1)
            cout << "->";
        else
            cout << endl;
    }
    return 0;
}
