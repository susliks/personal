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

const int MAX_CHILD_NUM = 50;   //һ���ڵ�ĺ�����Ŀ����

const int MAX_CHILD_SERVER_NUM = 40;//һ��ToR switch�µ�server��Ŀ����
const int MAX_CHILD_RACK_NUM = 40;  //һ��aggregation switch�µ�rack��Ŀ����
const int MAX_CHILD_POD_NUM = 10;   //һ��core switch�µ�pod��Ŀ����
const int MAX_VC_VM_NUM = 10;       //һ��VC�µ�VM��Ŀ����
const int MAX_VC_EDGE_NUM = 1000;   //һ��VC�µ�VM������ͨ·��Ŀ������
const int MAX_VC_TEMPLATE = 100;    //VCģ�������

/*��������ṹ��������Դ������*/
const int MAX_SERVER_CPU = 64;
//const int MAX_SERVER_MEMORY = 32 * 1024;    //��λ��MB
const int MAX_SERVER_MEMORY = 32;    //��λ����λ�ڴ�
const int MAX_SERVER_UPBANDWIDTH = 1 * 1024;//��λ��MB
const int UPBANDWIDTH_LAMBDA1 = 1;          //��������lambda�Ķ���μ�kraken����
const int UPBANDWIDTH_LAMBDA2 = 1;

const int INF = 0x7fffffff;

struct Server
{
    int id;
    int cpu;            //��ʹ�õ�cpu��Դ
    int memory;         //��ʹ�õ��ڴ���Դ
    int upbandwidth;    //��ʹ�õ����д�����Դ
    int parent;         //���ڵ��id����
};
extern Server server[maxv], tryserver[maxv];

struct Switch
{
    int id;
    int level;                  //�������������������·ֱ���1��2��3��
    int upbandwidth;            //��ʹ�õ����д�����Դ
    int childcnt;               //�ӽڵ���Ŀ
    int child[MAX_CHILD_NUM];   //�ӽڵ��id����
    int parent;                 //���ڵ��id����
};
extern Switch switches[maxv], tryswitches[maxv];

struct VirtualMachine
{
    int id;
    int location;       //��Ӧ�������id����ʶ��ǰ���������̨������� -1��ʾδ����
    int cpuCost;        //��ʾ����������Ҫ����cpu��Դ
    int memoryCost;     //��ʾ����������Ҫ�����ڴ���Դ
};

//VM������ͨ·
struct FlowLink
{
    int u, v;           //��Ӧ�������id���ֱ��ʶһ���ߵ������˵�
    int bandwidthCost;  //��ʾ��������ҪԤ����������
};

//VM������ͨ·���ڽ�������ʽ��Ϊ�˼򻯼��㣩
struct Edge
{
    int to, bandwidthCost, next;
};

struct VirtualCluster
{
    int id;
    int vmCnt;                          //VC�µ�VM��Ŀ
    VirtualMachine vm[MAX_VC_VM_NUM];   //VC�µ�VM
    int edgeCnt;                        //VC�µ�VM������ͨ·����Ŀ
    FlowLink link[MAX_VC_EDGE_NUM];     //VC�µ�VM������ͨ·
    int leftTime;

    //�����ڽ�����Ĵ洢��ʽ
    int head[MAX_VC_VM_NUM];
    int si = 0;
    Edge edge[MAX_VC_EDGE_NUM];
};
extern VirtualCluster vc[maxv], vcTemplate[MAX_VC_TEMPLATE];

struct Request
{
    int vcId;           //��ӦVirtualCluster��id��������ǰ�����Ƕ��ĸ�VC����
    int mode;           //��ʾ��ǰ����������   0���ı��ض�VM��cpu��Դ
                        //                        1���ı��ض�VM��memory��Դ
                        //                        2���ı��ض�edge��bandwidth��Դ
    int subId;          //���mode���������Ĳ������ͣ���ʶ��Ӧ���ߵ�id
    double percentage;  //�仯�İٷ���������Ŀ���ǡ�����10%��ʱ��percentageֵΪ��-0.1��
};

struct ScheduleQueNode
{
    int id;
    int waitSetSize;
    int waitSet[MAX_VC_VM_NUM]; //Ͱ��˼�룬waitSet[i]==1 ��ʾ��ǰVC�ĵ�i��vm��δ�����ã��ȴ��У�
                                //waitSet[i]==0 ��ʾ��ǰVC�ĵ�i��vm�ѱ�����
    int settleSetSize;
    int settleSet[MAX_VC_VM_NUM];//��ͬwaitSet,settle[i]==1 ��ʾ��ǰVC�ĵ�i��vm�ѱ������ڵ�ǰ�ڵ�֮��

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

extern bool visited[2][maxv];   //��¼�������ͽ������Ƿ񱻷��ʹ��ı�����飬��0ά��Ƿ���������1ά��ǽ�����


//==================init==================
extern int servercnt;
extern int switchcnt;
extern int vccnt;
extern int vctpltcnt;  //vc template count

void init();
void addServer(int parent, int cpu, int memory, int upbandwidth);
void addSwitch(int parent, int upbandwidth); //����޸��ڵ㣬��parentֵΪ-1
void build();

//==================req==================
const int MAX_REQ_NUM = 100000;
const int SCALE_REQ_SEQ_NUM = 6;
const int SCALE_REQ_MODE_NUM = 6;   //scale-up/scale-down�����п��ܱ仯��ģʽ
const double CPU_CHANGE_FACTOR = 1.5;          //����kraken�����У�������Դ�ı仯��
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

extern VirtualCluster vcPreState;  //�ݴ���е����������������ԭ״̬


double randomExponential(double lambda);    //���ɸ�ָ���ֲ�
void generatePoissonSeq();
int generateNewReq(int curTime); //-1�������� ���򷵻��´�����vc���
int generateScaleReq(int processId, int curTime, int &reqMode);   //-1�������� ���򷵻��޸ĵ�vc���
void terminateJudge(int curTime);

//==================placement==================
extern int FFServerPosPtr; //����ͼ����-1��1ʱ�޸�

int FFsettleInSwitch(int vcId, int switchId, int &curVmPos, int intervalLB, int intervalRB, int serverStartPos);  //����ֵ��0����ʱ���ý��� -1����ʧ�ܣ����� 1���óɹ�
int FFsettleInServer(int vcId, int serverId, int &curVmPos, int intervalLB, int intervalRB, int serverStartPos);
bool placement(int vcId, int curTime);

//==================scheduler==================
//��������������ʽ��¼�������޸�λ�ã��������������������𡱣�
extern int trytreeServerUpdateLocCnt;
extern int trytreeServerUpdateLoc[maxv];
extern int trytreeSwitchUpdateLocCnt;
extern int trytreeSwitchUpdateLoc[maxv];

//ֻ�ڱ������ⲿ�������õ�ȫ��dp���飬��ȫ����Ϊ�˼ӿ��ٶ�
extern int knapsackDp[MAX_VC_VM_NUM + 1][MAX_SERVER_CPU + 2][MAX_SERVER_MEMORY + 2];
extern int knapsackPath[MAX_VC_VM_NUM + 1][MAX_SERVER_CPU + 2][MAX_SERVER_MEMORY + 2];//��¼·����ֻ������ת�Ʒ�ʽ��0��ʾ�Լ�ת�Ƶ��Լ���1��ʾѡȡ����һ����Ʒ��ת��


void firstCopyTryTree();    //ִֻ��һ�Σ�֮�󶼻�����������
void deleteVCResource(int vcId);    //VC����ʱ��ɾ�������ϸ�vc��ռ��Դ
void deletePreVCResource();         //��������ɾ����ǰvc��ԭ״̬��ռ�õ���Դ
void updateFromVC(int vcId);        //��vc����������
                                    //��������Դ�Ƿ�������ж�
void updateFromTryTree();
void rollbackTryTree();
void vcAddEdge(VirtualCluster& vc, int u, int v, int cost);
void loadVcTemplate();   //��¼����vmLoc�����������Ϣ
void rollbackVC(int vcId);
void settleInServer(int vcId, int serverId, int waitSet[], int &waitSetSize, int settleSet[], int &settleSetSize);//��Ҫ����waitSetSize��ֵ
void settleInSwitch(int vcId, int switchId, int waitSet[], int &waitSetSize, int settleSet[], int &settleSetSize);
bool scheduler(int vcId, int reqMode);


//==================evaluation==================
extern int newReqAcCnt, newReqTotCnt;      //�½�����ļ���
extern int scaleReqAcCnt, scaleReqTotCnt;  //��������ļ���
extern int migMemoryCnt;   //Ǩ�����ڴ�
extern int migCnt;         //Ǩ���ܴ���
//todo������δ��
extern int migTotTime;     //����Ǩ�Ƶ�ʱ��

void updateMigMemoryCnt(int vcId);   //�����ܵ�Ǩ���ڴ���
void printEvaluationResult();

//#endif // HEAD_H_INCLUDED
