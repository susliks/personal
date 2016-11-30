#include "head.h"


int FFsettleInSwitch(int vcId, int switchId, int &curVmPos, int intervalLB, int intervalRB, int serverStartPos)  //返回值：0，暂时放置结束 -1放置失败，结束 1放置成功
{
    visited[1][switchId] = !visited[1][switchId];   //为了标记出当前是第几次访问这个交换机

    int preVmPos = curVmPos;    //以备回滚
    int endFlag = 0;    //0:未结束 -1：失败结束 1：成功结束

    //计算进来的带宽是否足够
    int curOutBandwidth = 0;
    if(preVmPos > 0)
    {
        for(int i = vc[vcId].head[preVmPos]; i != -1; i = vc[vcId].edge[i].next)
        {
            if(vc[vcId].edge[i].to == preVmPos - 1)
            {
                curOutBandwidth += vc[vcId].edge[i].bandwidthCost;
                break;
            }
        }
    }
    if(curOutBandwidth > switches[switchId].upbandwidth)
    {
        endFlag = 0;
        return endFlag;
    }

    if(switches[switchId].level == 1)    //子节点是Aggregation Switch
    {
        int curSwitchPos = serverStartPos / MAX_CHILD_RACK_NUM / MAX_CHILD_SERVER_NUM;
        //for(int i = 0; i <= switches[switchId].childcnt && endFlag == 0; i++)
        while(curSwitchPos < switches[switchId].childcnt && endFlag == 0)
        {
            int childId = switches[switchId].child[curSwitchPos];
            int newIntervalLB = intervalLB + MAX_CHILD_RACK_NUM * MAX_CHILD_SERVER_NUM * curSwitchPos;
            int newIntervalRB = newIntervalLB + MAX_CHILD_RACK_NUM * MAX_CHILD_SERVER_NUM - 1;
            endFlag = FFsettleInSwitch(vcId, childId, curVmPos, newIntervalLB, newIntervalRB, serverStartPos);
            //curSwitchPos = (curSwitchPos + 1) % switches[switchId].childcnt;
            curSwitchPos++;
        }
    }
    else if(switches[switchId].level == 2)  //子节点是TOR Switch
    {
        //int curSwitchPos = (serverStartPos % (MAX_CHILD_RACK_NUM * MAX_CHILD_SERVER_NUM)) / MAX_CHILD_SERVER_NUM;

        int curSwitchPos;
        if(intervalLB <= serverStartPos && serverStartPos <= intervalRB && visited[1][switchId] == 1)   //搜索起点在这个区间内并且该节点第一次被访问
            curSwitchPos = (serverStartPos - intervalLB) / MAX_CHILD_SERVER_NUM;
        else
            curSwitchPos = 0;
        //for(int i = 0; i <= switches[switchId].childcnt && endFlag == 0; i++)
        while(curSwitchPos < switches[switchId].childcnt && endFlag == 0)
        {
            int childId = switches[switchId].child[curSwitchPos];
            int newIntervalLB = intervalLB + MAX_CHILD_SERVER_NUM * curSwitchPos;
            int newIntervalRB = newIntervalLB + MAX_CHILD_SERVER_NUM - 1;
            endFlag = FFsettleInSwitch(vcId, childId, curVmPos, newIntervalLB, newIntervalRB, serverStartPos);
            //curSwitchPos = (curSwitchPos + 1) % switches[switchId].childcnt;
            curSwitchPos++;
        }
    }
    else if(switches[switchId].level == 3)  //子节点是服务器
    {
        int curServerPos;
        if(intervalLB <= serverStartPos && serverStartPos <= intervalRB && visited[1][switchId] == 1)   //搜索起点在这个区间内并且该节点第一次被访问
            curServerPos = serverStartPos - intervalLB;
        else
            curServerPos = 0;
        //for(int i = 0; i <= switches[switchId].childcnt && endFlag == 0; i++)
        while(curServerPos < switches[switchId].childcnt && endFlag == 0)
        {
            int childId = switches[switchId].child[curServerPos];
            int newIntervalLB = intervalLB + curServerPos;
            int newIntervalRB = newIntervalLB;
            endFlag = FFsettleInServer(vcId, childId, curVmPos, newIntervalLB, newIntervalRB, serverStartPos);
            //curServerPos = (curServerPos + 1) % switches[switchId].childcnt;
            curServerPos++;
        }
    }

    if(curVmPos == vc[vcId].vmCnt)  //成功放置，退出循环
    {
        endFlag = 1;
    }
    else if(curVmPos < vc[vcId].vmCnt)
    {
        for(int i = vc[vcId].head[curVmPos]; i != -1; i = vc[vcId].edge[i].next)
        {
            if(vc[vcId].edge[i].to == curVmPos + 1)
            {
                curOutBandwidth += vc[vcId].edge[i].bandwidthCost;
                break;
            }
        }

        if(curOutBandwidth > switches[switchId].upbandwidth) //带宽不足，回滚
            curVmPos = preVmPos;
    }

    //DEBUG
    if(endFlag == -1 || (endFlag == 0 && switches[switchId].level == 1))
    {
        int a = 0;
        a++;

    }

    return endFlag;
}

int FFsettleInServer(int vcId, int serverId, int &curVmPos, int intervalLB, int intervalRB, int serverStartPos)
{
    visited[0][serverId] = 1;

    int preVmPos = curVmPos;    //以备回滚
    int endFlag = 0;    //0:未结束 -1：失败结束 1：成功结束

    //计算进来的带宽是否足够
    int curOutBandwidth = 0;
    if(preVmPos > 0)
    {
        for(int i = vc[vcId].head[preVmPos]; i != -1; i = vc[vcId].edge[i].next)
        {
            if(vc[vcId].edge[i].to == preVmPos - 1)
            {
                curOutBandwidth += vc[vcId].edge[i].bandwidthCost;
                break;
            }
        }
    }
    if(curOutBandwidth > server[serverId].upbandwidth)
    {
        endFlag = 0;
        return endFlag;
    }

    //判CPU、MEMORY资源
    int curCpu = 0;
    int curMemory = 0;
    while(curVmPos < vc[vcId].vmCnt && curCpu + vc[vcId].vm[curVmPos].cpuCost <= server[serverId].cpu
          && curMemory + vc[vcId].vm[curVmPos].memoryCost <= server[serverId].memory)
    {
        curCpu += vc[vcId].vm[curVmPos].cpuCost;
        curMemory += vc[vcId].vm[curVmPos].memoryCost;
        vc[vcId].vm[curVmPos].location = serverId;
        curVmPos++;
    }

    if(curVmPos == vc[vcId].vmCnt)  //成功放置，退出循环
    {
        //更新下次搜索的起始位置
        FFServerPosPtr = (serverId + 1) % (MAX_CHILD_POD_NUM * MAX_CHILD_RACK_NUM * MAX_CHILD_SERVER_NUM);

        return 1;
    }
    else// if(curVmPos < vc[vcId].vmCnt)
    {
        for(int i = vc[vcId].head[curVmPos-1]; i != -1; i = vc[vcId].edge[i].next)
        {
            if(vc[vcId].edge[i].to == curVmPos)
            {
                curOutBandwidth += vc[vcId].edge[i].bandwidthCost;
                break;
            }
        }

        if(curOutBandwidth > server[serverId].upbandwidth) //带宽不足，回滚
            curVmPos = preVmPos;

        if((server[serverId].id + 1) % (MAX_CHILD_POD_NUM * MAX_CHILD_RACK_NUM * MAX_CHILD_SERVER_NUM) == serverStartPos)
            return -1;  //搜索到起点
        else
            return 0;
    }
}

bool placement(int vcId, int curTime)
{
    memset(visited, 0, sizeof(visited));    //清空访问标记数组

    int curVmPos = 0;
    if(FFsettleInSwitch(vcId, 0, curVmPos, 0, MAX_CHILD_POD_NUM * MAX_CHILD_RACK_NUM * MAX_CHILD_SERVER_NUM - 1, FFServerPosPtr) == 1)
    {
        vcEndCheck.insert(pair<int, int>(curTime + vc[vccnt].leftTime, vcId)); //计算出消亡的时间点，存入vcEndCheck
        updateFromVC(vcId);
        return true;
    }
    else    //放置失败
    {
        activeVc.erase(vcId);
        activeVcCnt--;

        return false;
    }

}
