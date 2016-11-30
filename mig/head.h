#pragma once
//#ifndef HEAD_H_INCLUDED
//#define HEAD_H_INCLUDED

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <set>
#include <queue>
#include <stack>
#include <map>
#include <vector>
#include <ctime>

using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef pair<int, int> P;

const int maxv = 20000;

const int MAX_CHILD_NUM = 50;   //一个节点的孩子数目上限

const int MAX_CHILD_SERVER_NUM = 40;//一个ToR switch下的server数目上限
const int MAX_CHILD_RACK_NUM = 40;  //一个aggregation switch下的rack数目上限
const int MAX_CHILD_POD_NUM = 10;   //一个core switch下的pod数目上限
const int MAX_VC_VM_NUM = 10;       //一个VC下的VM数目上限
const int MAX_VC_EDGE_NUM = 1000;   //一个VC下的VM间流量通路数目的上限
const int MAX_VC_TEMPLATE = 100;    //VC模板的上限

/*定义物理结构的三种资源的上限*/
const int MAX_SERVER_CPU = 64;
//const int MAX_SERVER_MEMORY = 32 * 1024;    //单位：MB
const int MAX_SERVER_MEMORY = 32;    //单位：单位内存
const int MAX_SERVER_UPBANDWIDTH = 1 * 1024;//单位：MB
const int UPBANDWIDTH_LAMBDA1 = 1;          //这里两个lambda的定义参见kraken论文
const int UPBANDWIDTH_LAMBDA2 = 1;

const int INF = 0x7fffffff;

struct Server
{
    int id;
    int cpu;            //已使用的cpu资源
    int memory;         //已使用的内存资源
    int upbandwidth;    //已使用的上行带宽资源
    int parent;         //父节点的id号码
};
extern Server server[maxv], tryserver[maxv];

struct Switch
{
    int id;
    int level;                  //交换机层数，自上往下分别是1、2、3层
    int upbandwidth;            //已使用的上行带宽资源
    int childcnt;               //子节点数目
    int child[MAX_CHILD_NUM];   //子节点的id号码
    int parent;                 //父节点的id号码
};
extern Switch switches[maxv], tryswitches[maxv];

struct VirtualMachine
{
    int id;
    int location;       //对应物理机的id，标识当前虚拟机在哪台物理机上 -1表示未放置
    int cpuCost;        //表示这个虚拟机需要多少cpu资源
    int memoryCost;     //表示这个虚拟机需要多少内存资源
};

//VM间流量通路
struct FlowLink
{
    int u, v;           //对应虚拟机的id，分别标识一条边的两个端点
    int bandwidthCost;  //表示这条边需要预留多少流量
};

//VM间流量通路的邻接链表形式（为了简化计算）
struct Edge
{
    int to, bandwidthCost, next;
};

struct VirtualCluster
{
    int id;
    int vmCnt;                          //VC下的VM数目
    VirtualMachine vm[MAX_VC_VM_NUM];   //VC下的VM
    int edgeCnt;                        //VC下的VM间流量通路的数目
    FlowLink link[MAX_VC_EDGE_NUM];     //VC下的VM间流量通路
    int leftTime;

    //增加邻接链表的存储方式
    int head[MAX_VC_VM_NUM];
    int si = 0;
    Edge edge[MAX_VC_EDGE_NUM];
};
extern VirtualCluster vc[maxv], vcTemplate[MAX_VC_TEMPLATE];

struct Request
{
    int vcId;           //对应VirtualCluster的id，表明当前操作是对哪个VC进行
    int mode;           //表示当前操作的类型   0：改变特定VM的cpu资源
                        //                        1：改变特定VM的memory资源
                        //                        2：改变特定edge的bandwidth资源
    int subId;          //针对mode变量给出的操作类型，标识对应点或边的id
    double percentage;  //变化的百分数，比如目标是“减少10%”时，percentage值为“-0.1”
};

struct ScheduleQueNode
{
    int id;
    int waitSetSize;
    int waitSet[MAX_VC_VM_NUM]; //桶排思想，waitSet[i]==1 表示当前VC的第i个vm还未被放置（等待中）
                                //waitSet[i]==0 表示当前VC的第i个vm已被放置
    int settleSetSize;
    int settleSet[MAX_VC_VM_NUM];//理同waitSet,settle[i]==1 表示当前VC的第i个vm已被放置在当前节点之下

    //set<int> waitSet;
    void init(int id, int vmCnt, const int waitSet[], const int settleSet[])
    {
        this->id = id;
        this->waitSetSize = 0;
        for(int i = 0; i < vmCnt; i++)
        {
            this->waitSet[i] = waitSet[i];
            if(this->waitSet[i] == 1)
                this->waitSetSize++;
        }
        this->settleSetSize = 0;
        for(int i = 0; i < vmCnt; i++)
        {
            this->settleSet[i] = settleSet[i];
            if(this->settleSet[i] == 1)
                this->settleSetSize++;
        }
    }

    void update(int vmCnt, const int waitSet[], const int settleSet[])
    {
        for(int i = 0; i < vmCnt; i++)
        {
            if(waitSet[i] == 1)
            {
                this->waitSet[i] = waitSet[i];
                this->waitSetSize++;
            }
            if(settleSet[i] == 1)
            {
                this->settleSet[i] = settleSet[i];
                this->settleSetSize++;
            }
        }
    }

};

extern bool visited[2][maxv];   //记录服务器和交换机是否被访问过的标记数组，第0维标记服务器，第1维标记交换机


//==================init==================
extern int servercnt;
extern int switchcnt;
extern int vccnt;
extern int vctpltcnt;  //vc template count

void init();
void addServer(int parent, int cpu, int memory, int upbandwidth);
void addSwitch(int parent, int upbandwidth); //如果无父节点，则parent值为-1
void build();

//==================req==================
const int MAX_REQ_NUM = 100000;
const int SCALE_REQ_SEQ_NUM = 6;
const int SCALE_REQ_MODE_NUM = 6;   //scale-up/scale-down请求中可能变化的模式
const double CPU_CHANGE_FACTOR = 1.5;          //依据kraken论文中，设置资源的变化率
const double MEMORY_CHANGE_FACTOR = 1.5;
const double BANDWIDTH_CHANGE_FACTOR = 1.5;

extern double tmpPoissonSeq[MAX_REQ_NUM];
extern int newReqPoissonSeq[MAX_REQ_NUM];
extern int scaleReqPoissonSeq[SCALE_REQ_SEQ_NUM][MAX_REQ_NUM];
extern int newReqSeqPtr;
extern int scaleReqSeqPtr;
extern set<int> activeVc;
extern int activeVcCnt;

extern multimap<int, int> vcEndCheck;

extern VirtualCluster vcPreState;  //暂存进行弹性伸缩的虚拟机的原状态


double randomExponential(double lambda);    //生成负指数分布
void generatePoissonSeq();
int generateNewReq(int curTime); //-1：无请求 否则返回新创建的vc编号
int generateScaleReq(int processId, int curTime, int &reqMode);   //-1：无请求 否则返回修改的vc编号
void terminateJudge(int curTime);

//==================placement==================
extern int FFServerPosPtr; //在试图返回-1或1时修改

int FFsettleInSwitch(int vcId, int switchId, int &curVmPos, int intervalLB, int intervalRB, int serverStartPos);  //返回值：0，暂时放置结束 -1放置失败，结束 1放置成功
int FFsettleInServer(int vcId, int serverId, int &curVmPos, int intervalLB, int intervalRB, int serverStartPos);
bool placement(int vcId, int curTime);

//==================scheduler==================
//用来以增量的形式记录新树的修改位置（即“新树和老树的区别”）
extern int trytreeServerUpdateLocCnt;
extern int trytreeServerUpdateLoc[maxv];
extern int trytreeSwitchUpdateLocCnt;
extern int trytreeSwitchUpdateLoc[maxv];

//只在背包问题部分起作用的全局dp数组，开全局是为了加快速度
extern int knapsackDp[MAX_VC_VM_NUM + 1][MAX_SERVER_CPU + 2][MAX_SERVER_MEMORY + 2];
extern int knapsackPath[MAX_VC_VM_NUM + 1][MAX_SERVER_CPU + 2][MAX_SERVER_MEMORY + 2];//记录路径，只有两种转移方式，0表示自己转移到自己，1表示选取了上一件物品的转移


void firstCopyTryTree();    //只执行一次，之后都会变成增量复制
void deleteVCResource(int vcId);    //VC消亡时，删除网络上该vc所占资源
void deletePreVCResource();         //在老树上删除当前vc的原状态所占用的资源
void updateFromVC(int vcId);        //用vc来更新老树
                                    //不再做资源是否满足的判断
void updateFromTryTree();
void rollbackTryTree();
void vcAddEdge(VirtualCluster& vc, int u, int v, int cost);
void loadVcTemplate();   //记录除了vmLoc以外的所有信息
void rollbackVC(int vcId);
void settleInServer(int vcId, int serverId, int waitSet[], int &waitSetSize, int settleSet[], int &settleSetSize);//需要更新waitSetSize的值
void settleInSwitch(int vcId, int switchId, int waitSet[], int &waitSetSize, int settleSet[], int &settleSetSize);
bool scheduler(int vcId, int reqMode);


//==================evaluation==================
extern int newReqAcCnt, newReqTotCnt;      //新建请求的计数
extern int scaleReqAcCnt, scaleReqTotCnt;  //调整请求的计数
extern int migMemoryCnt;   //迁移总内存
extern int migCnt;         //迁移总次数
//todo：定义未明
extern int migTotTime;     //花在迁移的时间

void updateMigMemoryCnt(int vcId);   //更新总的迁移内存数
void printEvaluationResult();

//#endif // HEAD_H_INCLUDED
