#include "head.h"

Server server[maxv], tryserver[maxv];
Switch switches[maxv], tryswitches[maxv];
VirtualCluster vc[maxv], vcTemplate[MAX_VC_TEMPLATE];

bool visited[2][maxv];   //��¼�������ͽ������Ƿ񱻷��ʹ��ı�����飬��0ά��Ƿ���������1ά��ǽ�����

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

VirtualCluster vcPreState;  //�ݴ���е����������������ԭ״̬

//==================placement==================
int FFServerPosPtr = 0; //����ͼ����-1��1ʱ�޸�

//==================scheduler==================
//��������������ʽ��¼�������޸�λ�ã��������������������𡱣�
int trytreeServerUpdateLocCnt;
int trytreeServerUpdateLoc[maxv];
int trytreeSwitchUpdateLocCnt;
int trytreeSwitchUpdateLoc[maxv];

//ֻ�ڱ������ⲿ�������õ�ȫ��dp���飬��ȫ����Ϊ�˼ӿ��ٶ�
int knapsackDp[MAX_VC_VM_NUM + 1][MAX_SERVER_CPU + 2][MAX_SERVER_MEMORY + 2];
int knapsackPath[MAX_VC_VM_NUM + 1][MAX_SERVER_CPU + 2][MAX_SERVER_MEMORY + 2];//��¼·����ֻ������ת�Ʒ�ʽ��0��ʾ�Լ�ת�Ƶ��Լ���1��ʾѡȡ����һ����Ʒ��ת��


//==================evaluation==================
int newReqAcCnt, newReqTotCnt;      //�½�����ļ���
int scaleReqAcCnt, scaleReqTotCnt;  //��������ļ���
int migMemoryCnt;   //Ǩ�����ڴ�
int migCnt;         //Ǩ���ܴ���
//todo������δ��
int migTotTime;     //����Ǩ�Ƶ�ʱ��



