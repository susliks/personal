#include "head.h"

Server server[maxv], tryserver[maxv];
Switch switches[maxv], tryswitches[maxv];
VirtualCluster vc[maxv], vcTemplate[MAX_VC_TEMPLATE];

bool visited[2][maxv];   //记录服务器和交换机是否被访问过的标记数组，第0维标记服务器，第1维标记交换机

//==================init==================
int servercnt;
int switchcnt;
int vccnt;
int vctpltcnt;  //vc template count


//==================req==================
double tmpPoissonSeq[MAX_REQ_NUM];
int newReqPoissonSeq[MAX_REQ_NUM];
int scaleReqPoissonSeq[SCALE_REQ_SEQ_NUM][MAX_REQ_NUM];
int newReqSeqPtr;
int scaleReqSeqPtr;
set<int> activeVc;
int activeVcCnt;

multimap<int, int> vcEndCheck;

VirtualCluster vcPreState;  //暂存进行弹性伸缩的虚拟机的原状态

//==================placement==================
int FFServerPosPtr = 0; //在试图返回-1或1时修改

//==================scheduler==================
//用来以增量的形式记录新树的修改位置（即“新树和老树的区别”）
int trytreeServerUpdateLocCnt;
int trytreeServerUpdateLoc[maxv];
int trytreeSwitchUpdateLocCnt;
int trytreeSwitchUpdateLoc[maxv];

//只在背包问题部分起作用的全局dp数组，开全局是为了加快速度
int knapsackDp[MAX_VC_VM_NUM + 1][MAX_SERVER_CPU + 2][MAX_SERVER_MEMORY + 2];
int knapsackPath[MAX_VC_VM_NUM + 1][MAX_SERVER_CPU + 2][MAX_SERVER_MEMORY + 2];//记录路径，只有两种转移方式，0表示自己转移到自己，1表示选取了上一件物品的转移


//==================evaluation==================
int newReqAcCnt, newReqTotCnt;      //新建请求的计数
int scaleReqAcCnt, scaleReqTotCnt;  //调整请求的计数
int migMemoryCnt;   //迁移总内存
int migCnt;         //迁移总次数
//todo：定义未明
int migTotTime;     //花在迁移的时间



