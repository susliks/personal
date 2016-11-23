#include "head.h"

void init()
{
    servercnt = 0;
    switchcnt = 0;
    vccnt = 0;
    vctpltcnt = 0;
    //newReqSeqPtr = 0;
    //scaleReqSeqPtr = 0;

    //TODO�����ǣ�����ȵ�������ʱ���ٳ�ʼ����
    //��ʼ��VC�ڽӱ�
    for(int i = 0; i < maxv; i++)
    {
        vc[i].si = 0;
        memset(vc[i].head, -1, sizeof(vc[i].head));
        memset(vc[i].edge, -1, sizeof(vc[i].edge));
    }
    for(int i = 0; i < MAX_VC_TEMPLATE; i++)
    {
        vcTemplate[i].si = 0;
        memset(vcTemplate[i].head, -1, sizeof(vcTemplate[i].head));
        memset(vcTemplate[i].edge, -1, sizeof(vcTemplate[i].edge));
    }
}

void addServer(int parent, int cpu, int memory, int upbandwidth)
{
    server[servercnt].id = servercnt;
    server[servercnt].cpu = cpu;
    server[servercnt].memory = memory;
    server[servercnt].upbandwidth = upbandwidth;
    server[servercnt].parent = parent;
    switches[parent].child[switches[parent].childcnt++] = servercnt;    //���¸��ڵ���ӽڵ���Ϣ
    servercnt++;
}

void addSwitch(int parent, int upbandwidth) //����޸��ڵ㣬��parentֵΪ-1
{
    if(parent != -1)
        switches[switchcnt].level = switches[parent].level + 1;
    else
        switches[switchcnt].level = 1;

    switches[switchcnt].id = switchcnt;
    switches[switchcnt].parent = parent;
    switches[switchcnt].upbandwidth = upbandwidth;
    if(parent != -1)    //���¸��ڵ�ĺ�����Ϣ
        switches[parent].child[switches[parent].childcnt++] = switchcnt;
    switches[switchcnt].childcnt = 0;
    switchcnt++;
}

void build()
{
    /*===================������������=============================*/
    addSwitch(-1, 0);   //core switch
    int parentCoreSwitchId = switchcnt - 1;
    for(int i = 0; i < MAX_CHILD_POD_NUM; i++)    //recursively buliding
    {
        addSwitch(parentCoreSwitchId, MAX_SERVER_UPBANDWIDTH *
                  MAX_CHILD_SERVER_NUM * MAX_CHILD_RACK_NUM /
                  UPBANDWIDTH_LAMBDA2);
        int parentAggregationSwitchId = switchcnt - 1;
        for(int j = 0; j < MAX_CHILD_RACK_NUM; j++)
        {
            addSwitch(parentAggregationSwitchId, MAX_SERVER_UPBANDWIDTH *
                      MAX_CHILD_SERVER_NUM / UPBANDWIDTH_LAMBDA1);
            int parentTORSwitchId = switchcnt - 1;
            for(int k = 0; k < MAX_CHILD_SERVER_NUM; k++)
            {
                addServer(parentTORSwitchId, MAX_SERVER_CPU,
                          MAX_SERVER_MEMORY, MAX_SERVER_UPBANDWIDTH);
            }
        }
    }
}
