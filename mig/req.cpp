#include "head.h"



double randomExponential(double lambda) //生成负指数分布
{
    double pV = 0.0;
    while(true)
    {
        pV = (double)rand() / (double)RAND_MAX;
        if (pV != 1)
        {
            break;
        }
    }
    pV = (-1.0 / lambda) * log(1 - pV);
    return pV;
}

void generatePoissonSeq()
{
    for(int i = 1; i < MAX_REQ_NUM; i++)
        tmpPoissonSeq[i] = tmpPoissonSeq[i-1] + randomExponential(0.36);
    for(int i = 0; i < MAX_REQ_NUM; i++)    //取天花板数，保证在整点时刻处理请求
        newReqPoissonSeq[i] = ceil(tmpPoissonSeq[i]);

    for(int i = 0; i < SCALE_REQ_SEQ_NUM; i++)
    {
        for(int j = 1; j < MAX_REQ_NUM; j++)
            tmpPoissonSeq[i] = tmpPoissonSeq[i-1] + randomExponential(0.36);
        for(int j = 0; j < MAX_REQ_NUM; j++)
            scaleReqPoissonSeq[i][j] = ceil(tmpPoissonSeq[i]);
    }
}

//直接在vc数组创建一个元素
//等放置的时候判断能不能放：如果不能，把lefttime置0
int generateNewReq(int curTime) //-1：无请求 否则返回新创建的vc编号
{
    if(newReqPoissonSeq[newReqSeqPtr] == curTime)
    {
        newReqSeqPtr++;

        int tpltId = rand() % vctpltcnt;    //随机取一个模板
        vc[vccnt].id = vccnt;
        vc[vccnt].leftTime = randomExponential((double)1 / 3600);   //依据kraken论文的说法，是依据均值3600的指数分布去生成随机数

        vcEndCheck.insert(pair<int, int>(curTime + vc[vccnt].leftTime, vccnt)); //计算出消亡的时间点，存入vcEndCheck

        vc[vccnt].vmCnt = vcTemplate[tpltId].vmCnt;
        vc[vccnt].edgeCnt = vcTemplate[tpltId].edgeCnt;
        vc[vccnt].si = vcTemplate[tpltId].si;
        for(int i = 0; i < vc[vccnt].vmCnt; i++)
        {
            vc[vccnt].vm[i].id = vcTemplate[tpltId].vm[i].id;
            vc[vccnt].vm[i].location = vcTemplate[tpltId].vm[i].location;
            vc[vccnt].vm[i].cpuCost = vcTemplate[tpltId].vm[i].cpuCost;
            vc[vccnt].vm[i].memoryCost = vcTemplate[tpltId].vm[i].memoryCost;
        }
        for(int i = 0; i < vc[vccnt].vmCnt; i++)
            vc[vccnt].head[i] = vcTemplate[tpltId].head[i];
        for(int i = 0; i < vc[vccnt].edgeCnt; i++)
        {
            vc[vccnt].link[i].u = vcTemplate[tpltId].link[i].u;
            vc[vccnt].link[i].v = vcTemplate[tpltId].link[i].v;
            vc[vccnt].link[i].bandwidthCost = vcTemplate[tpltId].link[i].bandwidthCost;
        }
        for(int i = 0; i < vc[vccnt].si; i++)
        {
            vc[vccnt].edge[i].to = vcTemplate[tpltId].edge[i].to;
            vc[vccnt].edge[i].next = vcTemplate[tpltId].edge[i].next;
            vc[vccnt].edge[i].bandwidthCost = vcTemplate[tpltId].edge[i].bandwidthCost;
        }

        return vccnt - 1;
    }
    else
        return -1;
}

//先把该vc的信息存到一个临时缓冲区，更新VC和对应服务器的各参数
//等放置的时候再判能不能放：如果不能，还原vc信息，还原服务器信息
int generateScaleReq(int processId, int curTime, int &reqMode)   //-1：无请求 否则返回修改的vc编号
{
    if(scaleReqPoissonSeq[processId][scaleReqSeqPtr] == curTime && activeVcCnt > 0)
    {
        //随机选取一个活跃的vc
        int activeVcId = rand() % activeVcCnt;
        set<int>::iterator iter = activeVc.begin();
        //TODO:可能存在进一步优化的方法
        for(int i = 0; i < activeVcId; i++)
            iter++;
        int vcId = *iter;

        //把VC的原状态(只需要关注三种资源）存到一个缓冲区
        vcPreState.id = vc[vcId].id;
        vcPreState.si = vc[vcId].si;
        vcPreState.vmCnt = vc[vcId].vmCnt;
        vcPreState.edgeCnt = vc[vcId].edgeCnt;
        for(int i = 0; i < vcPreState.vmCnt; i++)
        {
            vcPreState.vm[i].id = vc[vcId].vm[i].id;
            vcPreState.vm[i].location = vc[vcId].vm[i].location;
            vcPreState.vm[i].cpuCost = vc[vcId].vm[i].cpuCost;
            vcPreState.vm[i].memoryCost = vc[vcId].vm[i].memoryCost;
        }
        for(int i = 0; i < vcPreState.vmCnt; i++)
        {
            vcPreState.head[i] = vc[vcId].head[i];
        }
        for(int i = 0; i < vcPreState.edgeCnt; i++)
        {
            vcPreState.link[i].u = vc[vcId].link[i].u;
            vcPreState.link[i].v = vc[vcId].link[i].v;
            vcPreState.link[i].bandwidthCost = vc[vcId].link[i].bandwidthCost;
        }
        for(int i = 0; i < vcPreState.si; i++)
        {
            vcPreState.edge[i].to = vc[vcId].edge[i].to;
            vcPreState.edge[i].next = vc[vcId].edge[i].next;
            vcPreState.edge[i].bandwidthCost = vc[vcId].edge[i].bandwidthCost;
        }

        //把每个VM的location设置为-1，表示当前并没有放置的位置
        for(int i = 0; i < vc[vcId].vmCnt; i++)
        {
            vc[vcId].vm[i].location = -1;
        }


        //更新VC和对应服务器的参数
        //随机选取一种变化的模式
        reqMode = rand() % SCALE_REQ_MODE_NUM;
        switch(reqMode)
        {
            case 0:         //bandwidth upgrade
                for(int i = 0; i < vc[vcId].edgeCnt; i++)
                {
                    vc[vcId].link[i].bandwidthCost = floor(vc[vcId].link[i].bandwidthCost * BANDWIDTH_CHANGE_FACTOR);
                }
                for(int i = 0; i < vc[vcId].si; i++)
                {
                    vc[vcId].edge[i].bandwidthCost = floor(vc[vcId].edge[i].bandwidthCost * BANDWIDTH_CHANGE_FACTOR);
                }
                break;
            case 1:         //bandwidth downgrade
                for(int i = 0; i < vc[vcId].edgeCnt; i++)
                {
                    vc[vcId].link[i].bandwidthCost = floor(vc[vcId].link[i].bandwidthCost / BANDWIDTH_CHANGE_FACTOR);
                }
                for(int i = 0; i < vc[vcId].si; i++)
                {
                    vc[vcId].edge[i].bandwidthCost = floor(vc[vcId].edge[i].bandwidthCost / BANDWIDTH_CHANGE_FACTOR);
                }
                break;
            case 2:         //cpu upgrade
                for(int i = 0; i < vc[vcId].vmCnt; i++)
                {
                    vc[vcId].vm[i].cpuCost = floor(vc[vcId].vm[i].cpuCost * CPU_CHANGE_FACTOR);
                }
                break;
            case 3:         //cpu downgrade
                for(int i = 0; i < vc[vcId].vmCnt; i++)
                {
                    vc[vcId].vm[i].cpuCost = floor(vc[vcId].vm[i].cpuCost / CPU_CHANGE_FACTOR);
                }
                break;
            case 4:         //memory upgrade
                for(int i = 0; i < vc[vcId].vmCnt; i++)
                {
                    vc[vcId].vm[i].memoryCost = floor(vc[vcId].vm[i].memoryCost * MEMORY_CHANGE_FACTOR);
                }
                break;
            case 5:         //memory downgrade
                for(int i = 0; i < vc[vcId].vmCnt; i++)
                {
                    vc[vcId].vm[i].memoryCost = floor(vc[vcId].vm[i].memoryCost / MEMORY_CHANGE_FACTOR);
                }
                break;
        }

        return vcId;
    }
    else
        return -1;
}

void terminateJudge(int curTime)
{
    pair<multimap<int, int>::iterator, multimap<int, int>::iterator> range;
    range = vcEndCheck.equal_range(curTime);
    for(multimap<int, int>::iterator i = range.first; i != range.second; i++)
    {
        int vcId = i->second;

        //更新activeVC
        activeVc.erase(vcId);
        activeVcCnt--;

        //更新资源使用情况
        deleteVCResource(vcId);
    }
    vcEndCheck.erase(range.first, range.second);
}
