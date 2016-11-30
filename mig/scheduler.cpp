#include "head.h"

//第一次从老树把所有内容都完整复制到新树
//只执行一次，之后都会变成增量复制
void firstCopyTryTree()
{
    for(int i = 0; i < servercnt; i++)
    {
        tryserver[i].cpu = server[i].cpu;
        tryserver[i].id = server[i].id;
        tryserver[i].memory = server[i].memory;
        tryserver[i].parent = server[i].parent;
        tryserver[i].upbandwidth = server[i].upbandwidth;
    }
    for(int i = 0; i < switchcnt; i++)
    {
        tryswitches[i].childcnt = switches[i].childcnt;
        for(int j = 0; j < switches[i].childcnt; j++)
            tryswitches[i].child[j] = switches[i].child[j];
        tryswitches[i].id = switches[i].id;
        tryswitches[i].parent = switches[i].parent;
        tryswitches[i].upbandwidth = switches[i].upbandwidth;
    }
}

//VC消亡时，删除网络上该vc所占资源
void deleteVCResource(int vcId)
{
    //更新CPU、MEMORY资源
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        int serverId = vc[vcId].vm[i].location;
        server[serverId].cpu += vc[vcId].vm[i].cpuCost;
        server[serverId].memory += vc[vcId].vm[i].memoryCost;
    }

    //更新BandWidth资源 依次对边做LCA
    for(int i = 0; i < vc[vcId].edgeCnt; i++)
    {
        int u = vc[vcId].link[i].u;
        int v = vc[vcId].link[i].v;

        int uLoc = vc[vcId].vm[u].location;
        int vLoc = vc[vcId].vm[v].location;
        //server层
        if(uLoc != vLoc)
        {
            server[uLoc].upbandwidth += vc[vcId].link[i].bandwidthCost;
            server[vLoc].upbandwidth += vc[vcId].link[i].bandwidthCost;

            uLoc = server[uLoc].parent;
            vLoc = server[vLoc].parent;
        }
        //switch层
        while(uLoc != vLoc)
        {
            switches[uLoc].upbandwidth += vc[vcId].link[i].bandwidthCost;
            switches[vLoc].upbandwidth += vc[vcId].link[i].bandwidthCost;

            uLoc = switches[uLoc].parent;
            vLoc = switches[vLoc].parent;
        }
    }
}

//在老树上删除当前vc的原状态所占用的资源
void deletePreVCResource()
{
    //更新CPU、MEMORY资源
    for(int i = 0; i < vcPreState.vmCnt; i++)
    {
        int serverId = vcPreState.vm[i].location;
        server[serverId].cpu += vcPreState.vm[i].cpuCost;
        server[serverId].memory += vcPreState.vm[i].memoryCost;
    }

    //更新BandWidth资源 依次对边做LCA
    for(int i = 0; i < vcPreState.edgeCnt; i++)
    {
        int u = vcPreState.link[i].u;
        int v = vcPreState.link[i].v;

        int uLoc = vcPreState.vm[u].location;
        int vLoc = vcPreState.vm[v].location;
        //server层
        if(uLoc != vLoc)
        {
            server[uLoc].upbandwidth += vcPreState.link[i].bandwidthCost;
            server[vLoc].upbandwidth += vcPreState.link[i].bandwidthCost;

            uLoc = server[uLoc].parent;
            vLoc = server[vLoc].parent;
        }
        //switch层
        while(uLoc != vLoc)
        {
            switches[uLoc].upbandwidth += vcPreState.link[i].bandwidthCost;
            switches[vLoc].upbandwidth += vcPreState.link[i].bandwidthCost;

            uLoc = switches[uLoc].parent;
            vLoc = switches[vLoc].parent;
        }
    }
}

//用vc来更新老树
void updateFromVC(int vcId) //不再做资源是否满足的判断
{
    //更新CPU、MEMORY资源
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        int serverId = vc[vcId].vm[i].location;
        server[serverId].cpu -= vc[vcId].vm[i].cpuCost;
        server[serverId].memory -= vc[vcId].vm[i].memoryCost;
    }

    //更新BandWidth资源 依次对边做LCA
    for(int i = 0; i < vc[vcId].edgeCnt; i++)
    {
        int u = vc[vcId].link[i].u;
        int v = vc[vcId].link[i].v;

        int uLoc = vc[vcId].vm[u].location;
        int vLoc = vc[vcId].vm[v].location;
        //server层
        if(uLoc != vLoc)
        {
            server[uLoc].upbandwidth -= vc[vcId].link[i].bandwidthCost;
            server[vLoc].upbandwidth -= vc[vcId].link[i].bandwidthCost;

            uLoc = server[uLoc].parent;
            vLoc = server[vLoc].parent;
        }
        //switch层
        while(uLoc != vLoc)
        {
            switches[uLoc].upbandwidth -= vc[vcId].link[i].bandwidthCost;
            switches[vLoc].upbandwidth -= vc[vcId].link[i].bandwidthCost;

            uLoc = switches[uLoc].parent;
            vLoc = switches[vLoc].parent;
        }
    }
}



//用新树更新老树
//更新成功时调用
void updateFromTryTree()
{
    for(int i = 0; i < trytreeServerUpdateLocCnt; i++)
    {
        int loc = trytreeServerUpdateLoc[i];
        server[loc].cpu = tryserver[loc].cpu;
        server[loc].memory = tryserver[loc].memory;
        server[loc].upbandwidth = tryserver[loc].upbandwidth;
    }
    for(int i = 0; i < trytreeSwitchUpdateLocCnt; i++)
    {
        int loc = trytreeSwitchUpdateLoc[i];
        switches[loc].upbandwidth = tryswitches[loc].upbandwidth;
    }
    trytreeServerUpdateLocCnt = 0;
    trytreeSwitchUpdateLocCnt = 0;
}

//用老树还原新树
//更新失败时调用
void rollbackTryTree()
{
    for(int i = 0; i < trytreeServerUpdateLocCnt; i++)
    {
        int loc = trytreeServerUpdateLoc[i];
        tryserver[loc].cpu = server[loc].cpu;
        tryserver[loc].memory = server[loc].memory;
        tryserver[loc].upbandwidth = server[loc].upbandwidth;
    }
    for(int i = 0; i < trytreeSwitchUpdateLocCnt; i++)
    {
        int loc = trytreeSwitchUpdateLoc[i];
        tryswitches[loc].upbandwidth = switches[loc].upbandwidth;
    }
    trytreeServerUpdateLocCnt = 0;
    trytreeSwitchUpdateLocCnt = 0;
}

void vcAddEdge(VirtualCluster& vc, int u, int v, int cost)
{
    //邻接表加双向边
    vc.edge[vc.si].to = v;
    vc.edge[vc.si].bandwidthCost = cost;
    vc.edge[vc.si].next = vc.head[u];
    vc.head[u] = vc.si++;
    vc.edge[vc.si].to = u;
    vc.edge[vc.si].bandwidthCost = cost;
    vc.edge[vc.si].next = vc.head[v];
    vc.head[v] = vc.si++;
}

void loadVcTemplate()   //记录除了vmLoc以外的所有信息
{
    FILE *fin = fopen("VCtemplate.txt", "r");
    int T;
    fscanf(fin, "%d", &T);
    while(T--)
    {
        int n, m;
        fscanf(fin, "%d%d", &n, &m);
        vcTemplate[vctpltcnt].vmCnt = n;
        vcTemplate[vctpltcnt].edgeCnt = m;

        for(int i = 0; i < n; i++)
        {
            int cpu, memory;
            fscanf(fin, "%d%d", &cpu, &memory);
            vcTemplate[vctpltcnt].vm[i].id = i;
            vcTemplate[vctpltcnt].vm[i].location = -1;
            vcTemplate[vctpltcnt].vm[i].cpuCost = cpu;
            vcTemplate[vctpltcnt].vm[i].memoryCost = memory;
        }

        for(int i = 0; i < m; i++)
        {
            int u, v, cost;
            fscanf(fin, "%d%d%d", &u, &v, &cost);
            vcTemplate[vctpltcnt].link[i].u = u;
            vcTemplate[vctpltcnt].link[i].v = v;
            vcTemplate[vctpltcnt].link[i].bandwidthCost = cost;
            vcAddEdge(vcTemplate[vctpltcnt], u, v, cost);
        }

        vctpltcnt++;
    }

    fclose(fin);
}


void rollbackVC(int vcId)
{
    //把VC的原状态(只需要关注三种资源）从缓冲区还原出来
    //vc[vcId].id = vcPreState.id;
    //vc[vcId].si = vcPreState.si;
    //vc[vcId].vmCnt = vcPreState.vmCnt;
    //vc[vcId].edgeCnt = vcPreState.edgeCnt;
    for(int i = 0; i < vcPreState.vmCnt; i++)
    {
        //vcPreState.vm[i].id = vc[vcId].vm[i].id;
        vc[vcId].vm[i].location = vcPreState.vm[i].location;
        vc[vcId].vm[i].cpuCost = vcPreState.vm[i].cpuCost;
        vc[vcId].vm[i].memoryCost = vcPreState.vm[i].memoryCost;
    }
    /*for(int i = 0; i < vcPreState.vmCnt; i++)
    {
        vcPreState.head[i] = vc[vcId].head[i];
    }*/
    for(int i = 0; i < vcPreState.edgeCnt; i++)
    {
        //vcPreState.link[i].u = vc[vcId].link[i].u;
        //vcPreState.link[i].v = vc[vcId].link[i].v;
        vc[vcId].link[i].bandwidthCost = vcPreState.link[i].bandwidthCost;
    }
    for(int i = 0; i < vcPreState.si; i++)
    {
        //vcPreState.edge[i].to = vc[vcId].edge[i].to;
        //vcPreState.edge[i].next = vc[vcId].edge[i].next;
        vc[vcId].edge[i].bandwidthCost = vcPreState.edge[i].bandwidthCost;
    }
}


void settleInServer(int vcId, int serverId, int waitSet[], int &waitSetSize, int settleSet[], int &settleSetSize)//需要更新waitSetSize的值
{
    //cout << "enter settleInServer" << endl;

    //DEBUG
    if(serverId == -1)
    {
        int a = 0;
        a = a + 1;
    }

    //DEBUG
    int debugCnt = 0;
    for(int i = 0; i < vc[vcId].vmCnt; i++)
        if(waitSet[i] == 1)
            debugCnt++;
    if(debugCnt != waitSetSize)
    {
        int a = 0;
        a++;
    }


    int preWaitSetSize = waitSetSize;

    visited[0][serverId] = 1;

    //int settleSetSize = 0;
    //int settleSet[MAX_VC_VM_NUM] = {0};

    //knapsackProblem Part
    int tmploc = 0;
    int w1[MAX_VC_VM_NUM], w2[MAX_VC_VM_NUM], v[MAX_VC_VM_NUM];
    int tmpMap[MAX_VC_VM_NUM];              //建立从w、v数组到waitSet的下标映射

    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        if(waitSet[i] == 1)
        {
            w1[tmploc] = vc[vcId].vm[i].cpuCost;
            w2[tmploc] = v[tmploc] = vc[vcId].vm[i].memoryCost;
            tmpMap[tmploc] = i;
            tmploc++;
        }
    }

    //memset(knapsackDp, 0, sizeof(knapsackDp));    //只需要初始化一维
    int cpuLimit = tryserver[serverId].cpu;
    int memoryLimit = tryserver[serverId].memory;
    for(int i = 0; i <= cpuLimit; i++)
        for(int j = 0; j <= memoryLimit; j++)
        {
            knapsackDp[waitSetSize][i][j] = 0;
        }

//DEBUG
int tmp = waitSetSize;

    for(int k = waitSetSize - 1; k >= 0; k--)
        for(int i = 0; i <= cpuLimit; i++)
            for(int j = 0; j <= memoryLimit; j++)
            {
                if(i < w1[k] || j < w2[k])
                {
                    knapsackDp[k][i][j] = knapsackDp[k+1][i][j];
                    knapsackPath[k][i][j] = 0;
                }
                else
                {
                    if(knapsackDp[k+1][i - w1[k]][j - w2[k]] + v[k] > knapsackDp[k+1][i][j])
                    {
                        knapsackDp[k][i][j] = knapsackDp[k+1][i - w1[k]][j - w2[k]] + v[k];
                        knapsackPath[k][i][j] = 1;
                    }
                    else
                    {
                        knapsackDp[k][i][j] = knapsackDp[k+1][i][j];
                        knapsackPath[k][i][j] = 0;
                    }
                }
            }


    int tmpi = cpuLimit;
    int tmpj = memoryLimit;
    for(int k = 0; k < waitSetSize; k++)
    {
        if(knapsackPath[k][tmpi][tmpj] == 1)//选中了当前物品
        {
            settleSet[tmpMap[k]] = 1;   //注意这里要使用映射数组回到waitSet/settleSet那种状态
            //选中了当前物品，说明回溯数组的上一个状态时knapsackPath[k+1][tmpi-w1[k]][tmpj-w2[k]]
            settleSetSize++;
            tmpi -= w1[k];
            tmpj -= w2[k];
        }
    }

    //heuristicSearch part
    int b1[MAX_VC_VM_NUM];
    int b2[MAX_VC_VM_NUM];
    int curOutBandwidth = 0;

    //计算对外流量的时候顺便计算B1、B2值
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        if(settleSet[i] == 1)
        {
            b1[i] = b2[i] = 0;
            for(int j = vc[vcId].head[i]; j != -1; j = vc[vcId].edge[j].next)
            {
                int v = vc[vcId].edge[j].to;
                if(settleSet[v] == 1)   //去点在内部
                {
                    b1[i] += vc[vcId].edge[j].bandwidthCost;
                }
                else                    //去点在外部
                {
                    b2[i] += vc[vcId].edge[j].bandwidthCost;
                    curOutBandwidth += vc[vcId].edge[j].bandwidthCost;
                }
            }
        }
    }

    while(settleSetSize > 0 && curOutBandwidth > tryserver[serverId].upbandwidth)
    {
        //根据启发规则进一步删点
        int maxAim = -INF;
        int delLoc = -1;
        for(int i = 0; i < vc[vcId].vmCnt; i++)
        {
            if(settleSet[i] == 1)
            {
                int tmpAim = b2[i] - b1[i];
                if(tmpAim > maxAim)
                {
                    maxAim = tmpAim;
                    delLoc = i;
                }
            }
        }
        settleSet[delLoc] = 0;
        settleSetSize--;

        //重新计算对外流量的时候顺便计算B1、B2值
        curOutBandwidth = 0;
        for(int i = 0; i < vc[vcId].vmCnt; i++)
        {
            if(settleSet[i] == 1)
            {
                b1[i] = b2[i] = 0;
                for(int j = vc[vcId].head[i]; j != -1; j = vc[vcId].edge[j].next)
                {
                    int v = vc[vcId].edge[j].to;
                    if(settleSet[v] == 1)   //去点在内部
                    {
                        b1[i] += vc[vcId].edge[j].bandwidthCost;
                    }
                    else                    //去点在外部
                    {
                        b2[i] += vc[vcId].edge[j].bandwidthCost;
                        curOutBandwidth += vc[vcId].edge[j].bandwidthCost;
                    }
                }
            }
        }
    }

    //用settleSet更新waitSet
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        if(settleSet[i] == 1)
        {
            //DEBUG
            if(waitSet[i] != 1)
            {
                int tmp = settleSetSize;
                int a = 0;
                a++;
            }

            waitSet[i] = 0;
            waitSetSize--;
        }
    }

    //更新该VC的所有vm的location
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        if(settleSet[i] == 1)
        {
            vc[vcId].vm[i].location = serverId;
        }
    }

    //用settleSet更新资源使用情况
    for(int i = 0; i < vc[vcId].vmCnt; i++) //(内存、cpu）
    {
        if(settleSet[i] == 1)
        {
            tryserver[serverId].cpu -= vc[vcId].vm[i].cpuCost;
            tryserver[serverId].memory -= vc[vcId].vm[i].memoryCost;
        }
    }
    tryserver[serverId].upbandwidth -= curOutBandwidth;//(带宽）


    if(settleSetSize == preWaitSetSize) //说明当前节点没有东西需要继续往外走，说明可能还可以继续放
        visited[0][serverId] = 0;
}


void settleInSwitch(int vcId, int switchId, int waitSet[], int &waitSetSize, int settleSet[], int &settleSetSize)
{
    //cout << "enter settleInSwitch" << endl;

    int preWaitSetSize = waitSetSize;

    visited[1][switchId] = 1;

    //遍历未访问过的子节点
    for(int i = 0; i < switches[switchId].childcnt && waitSetSize > 0; i++) //未放置完且仍有未访问的子节点
    {
        if(switches[switchId].level == 3)   //说明子节点是服务器
        {
            int childId = switches[switchId].child[i];
            if(visited[0][childId]) //如果当前节点已经被访问过
                continue;

            //todo
            int childSettleSet[MAX_VC_VM_NUM] = {0};
            int childSettleSetSize = 0;
            settleInServer(vcId, childId, waitSet, waitSetSize, childSettleSet, childSettleSetSize);
            for(int i = 0; i < vc[vcId].vmCnt; i++) //合并settleSet
            {
                if(childSettleSet[i] == 1)
                {
                    settleSet[i] = 1;
                    settleSetSize++;
                }
            }
        }
        else    //说明子节点是交换机
        {
            int childId = switches[switchId].child[i];
            if(visited[1][childId])  //如果当前节点已经被访问过
                continue;

            //todo
            int childSettleSet[MAX_VC_VM_NUM] = {0};
            int childSettleSetSize = 0;
            settleInSwitch(vcId, childId, waitSet, waitSetSize, childSettleSet, childSettleSetSize);
            for(int i = 0; i < vc[vcId].vmCnt; i++) //合并settleSet
            {
                if(childSettleSet[i] == 1)
                {
                    settleSet[i] = 1;
                    settleSetSize++;
                }
            }
        }
    }

    //todo:“往下走”地上传

    //计算子节点的上行带宽
    int curOutBandwidth = 0;
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        if(settleSet[i] == 0)//说明这个点在外部
        {
            for(int j = vc[vcId].head[i]; j != -1; j = vc[vcId].edge[j].next)
            {
                int v = vc[vcId].edge[j].to;
                if(settleSet[v] == 1)   //去点在内部
                {
                    curOutBandwidth += vc[vcId].edge[j].bandwidthCost;
                }
            }
        }
    }

    if(curOutBandwidth <= tryswitches[switchId].upbandwidth)    //带宽足够
    {
        //nothing to do
    }
    else    //带宽不足
    {
        //把settleSet中的vm的location信息清空
        for(int i = 0; i < vc[vcId].vmCnt; i++)
        {
            if(settleSet[i] == 1)
            {
                vc[vcId].vm[i].location = -1;
            }
        }

        //把settleSet中的点重新拿到waitSet中
        for(int i = 0; i < vc[vcId].vmCnt; i++)
        {
            if(settleSet[i] == 1)
            {
                //DEBUG
                if(waitSet[i] != 0)
                {
                    int a = 0;
                    a++;
                }


                waitSet[i] = 1;
                waitSetSize++;
                settleSet[i] = 0;
                settleSetSize--;
            }
        }
    }

    if(settleSetSize == preWaitSetSize) //说明当前节点可能还可以继续放
        visited[1][switchId] = 0;
}

//返回true表示这个请求可以被满足，返回false表示拒绝这个请求
bool scheduler(int vcId, int reqMode)
{
    //特判是否有单个vm的某种资源超过理论最大值
    bool limitExceeded = false;
    for(int i = 0; i < vc[vcId].vmCnt && !limitExceeded; i++)
    {
        if(vc[vcId].vm[i].cpuCost > MAX_SERVER_CPU)
            limitExceeded = true;
        if(vc[vcId].vm[i].memoryCost > MAX_SERVER_MEMORY)
            limitExceeded = true;
    }
    for(int i = 0; i < vc[vcId].edgeCnt && !limitExceeded; i++)
    {
        if(vc[vcId].link[i].bandwidthCost > MAX_SERVER_UPBANDWIDTH)
            limitExceeded = true;
    }
    if(limitExceeded)
    {
        rollbackVC(vcId);
        return false;
    }



    firstCopyTryTree();

    //判是否需要迁移,顺便更新新树
    bool needToMigrate = false;

    if(reqMode == 0 || reqMode == 1)
    {
        for(int i = 0; i < vc[vcId].edgeCnt && needToMigrate == false; i++)
        {
            int u = vc[vcId].link[i].u;
            int v = vc[vcId].link[i].v;

            //int tmpStack[10]; //暂存两个节点之间的边，三层交换机的结构下边数不会超过6
            //int tmpStackCnt = 0;

            int uLoc = vc[vcId].vm[u].location;
            int vLoc = vc[vcId].vm[v].location;
            //server层
            if(uLoc != vLoc)
            {
                tryserver[uLoc].upbandwidth -= (vc[vcId].link[i].bandwidthCost - vcPreState.link[i].bandwidthCost);
                tryserver[vLoc].upbandwidth -= (vc[vcId].link[i].bandwidthCost - vcPreState.link[i].bandwidthCost);
                if(tryserver[uLoc].upbandwidth < 0 || tryserver[vLoc].upbandwidth < 0)
                    needToMigrate = true;

                //增量地记录新树的修改位置
                trytreeServerUpdateLoc[trytreeServerUpdateLocCnt++] = uLoc;
                trytreeServerUpdateLoc[trytreeServerUpdateLocCnt++] = vLoc;

                uLoc = tryserver[uLoc].parent;
                vLoc = tryserver[vLoc].parent;
            }
            //switch层
            while(uLoc != vLoc)
            {
                tryswitches[uLoc].upbandwidth -= (vc[vcId].link[i].bandwidthCost - vcPreState.link[i].bandwidthCost);
                tryswitches[vLoc].upbandwidth -= (vc[vcId].link[i].bandwidthCost - vcPreState.link[i].bandwidthCost);
                if(tryswitches[uLoc].upbandwidth < 0 || tryswitches[vLoc].upbandwidth < 0)
                    needToMigrate = true;

                //增量地记录新树的修改位置
                trytreeSwitchUpdateLoc[trytreeSwitchUpdateLocCnt++] = uLoc;
                trytreeSwitchUpdateLoc[trytreeSwitchUpdateLocCnt++] = vLoc;

                uLoc = tryswitches[uLoc].parent;
                vLoc = tryswitches[vLoc].parent;
            }
        }
    }
    else if(reqMode == 2 || reqMode == 3)
    {
        for(int i = 0; i < vc[vcId].vmCnt && needToMigrate == false; i++)
        {
            int serverId = vc[vcId].vm[i].location;
            tryserver[serverId].cpu -= (vc[vcId].vm[i].cpuCost - vcPreState.vm[i].cpuCost);
            if(tryserver[serverId].cpu < 0)
                needToMigrate = true;

            //增量地记录新树的修改位置
            trytreeServerUpdateLoc[trytreeServerUpdateLocCnt++] = serverId;
        }
    }
    else if(reqMode == 4 || reqMode == 5)
    {
        for(int i = 0; i < vc[vcId].vmCnt && needToMigrate == false; i++)
        {
            int serverId = vc[vcId].vm[i].location;
            tryserver[serverId].memory -= (vc[vcId].vm[i].memoryCost - vcPreState.vm[i].memoryCost);
            if(tryserver[serverId].memory < 0)
                needToMigrate = true;

            //增量地记录新树的修改位置
            trytreeServerUpdateLoc[trytreeServerUpdateLocCnt++] = serverId;
        }
    }



    //若不需要迁移（表明请求可以满足），则直接用新树更新老树,退出
    if(needToMigrate == false)
    {
        updateFromTryTree();
        return true;
    }


    //能从这里往下走则说明需要迁移
    rollbackTryTree();

    //把每个VM的location设置为-1，表示当前并没有放置的位置
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        vc[vcId].vm[i].location = -1;
    }

    memset(visited, 0, sizeof(visited));    //清空访问标记数组

    //在新树上删除该VC所占资源
    for(int i = 0; i < vcPreState.vmCnt; i++)
    {
        int loc = vcPreState.vm[i].location;
        tryserver[loc].cpu += vcPreState.vm[i].cpuCost;
        tryserver[loc].memory += vcPreState.vm[i].memoryCost;
        trytreeServerUpdateLoc[trytreeServerUpdateLocCnt++] = loc;
    }
    for(int i = 0; i < vcPreState.edgeCnt; i++)
    {
        int u = vcPreState.link[i].u;
        int v = vcPreState.link[i].v;
        int uLoc = vcPreState.vm[u].location;
        int vLoc = vcPreState.vm[v].location;

        //server level
        if(uLoc != vLoc)
        {
            tryserver[uLoc].upbandwidth += vcPreState.link[i].bandwidthCost;
            tryserver[vLoc].upbandwidth += vcPreState.link[i].bandwidthCost;
            trytreeServerUpdateLoc[trytreeServerUpdateLocCnt++] = uLoc;
            trytreeServerUpdateLoc[trytreeServerUpdateLocCnt++] = vLoc;
            uLoc = tryserver[uLoc].parent;
            vLoc = tryserver[vLoc].parent;
        }
        //switch level
        while(uLoc != vLoc)
        {
            tryswitches[uLoc].upbandwidth += vcPreState.link[i].bandwidthCost;
            tryswitches[vLoc].upbandwidth += vcPreState.link[i].bandwidthCost;
            trytreeSwitchUpdateLoc[trytreeSwitchUpdateLocCnt++] = uLoc;
            trytreeSwitchUpdateLoc[trytreeSwitchUpdateLocCnt++] = vLoc;
            uLoc = tryswitches[uLoc].parent;
            vLoc = tryswitches[vLoc].parent;
        }
    }

    //试放
    bool canMigrate = false;
    queue<ScheduleQueNode> serverque;
    queue<ScheduleQueNode> switchque;
    //遍历一次VC的所有VM，往队列中加服务器
    map<int, int> serverMap;    //serverId离散化，建立从原serverId到离散化后的server的id的映射
    int tmpServerCnt = 0;
    vector<int> tmpvec[MAX_VC_VM_NUM];
    //先按服务器的位置整理出所有vm
    for(int i = 0; i < vcPreState.vmCnt; i++)
    {
        int tmpServerId = vcPreState.vm[i].location;
        if(serverMap.count(tmpServerId) == 1)
        {
            tmpvec[serverMap[tmpServerId]].push_back(i);
        }
        else
        {
            serverMap[tmpServerId] = tmpServerCnt++;
            tmpvec[serverMap[tmpServerId]].push_back(i);
        }
    }
    //把当前vc所用到的服务器及其待放点集加入队列
    for(map<int, int>::iterator it = serverMap.begin(); it != serverMap.end(); it++)    //因为map默认以key值排序，所以这样遍历可以保证顺序访问server
    {
        int tmpSize = tmpvec[it->second].size();
        int tmpWaitSet[MAX_VC_VM_NUM] = {0};
        int tmpSettleSet[MAX_VC_VM_NUM] = {0};
        for(int i = 0; i < tmpSize; i++)
        {
            tmpWaitSet[tmpvec[it->second][i]] = 1;
        }
        ScheduleQueNode tmpNode;
        tmpNode.init(it->first, vc[vcId].vmCnt, tmpWaitSet, tmpSettleSet);
        serverque.push(tmpNode);
    }

    //迭代服务器队列
    ScheduleQueNode parentNode;
    int preParentId = -1;
    while(!serverque.empty())
    {
        ScheduleQueNode cur = serverque.front();
        serverque.pop();
        int serverId = cur.id;

        settleInServer(vcId, serverId, cur.waitSet, cur.waitSetSize, cur.settleSet, cur.settleSetSize);

        //如果当前服务器还不能盛放所有的VM，则要把其父节点压入交换机队列

        if(cur.settleSetSize < vc[vcId].vmCnt)
        {
            if(preParentId == -1)   //第一次进入这个循环
            {
                preParentId = server[serverId].parent;
                parentNode.init(server[serverId].parent, vc[vcId].vmCnt, cur.waitSet, cur.settleSet);
            }
            else if(server[serverId].parent == parentNode.id)//与上一个服务器的父节点相同
            {
                parentNode.update(vc[vcId].vmCnt, cur.waitSet, cur.settleSet);
            }
            else if(server[serverId].parent != parentNode.id)    //与上一个服务器的父节点不同
            {
                switchque.push(parentNode); //压入上一个交换机节点
                preParentId = server[serverId].parent;
                parentNode.init(server[serverId].parent, vc[vcId].vmCnt, cur.waitSet, cur.settleSet);
            }
        }
    }
    if(preParentId != -1)
        switchque.push(parentNode); //压入最后一个交换机节点


    //迭代交换机队列
    preParentId = -1;   //复用上面“迭代服务器”过程的数据结构

    //建立新的waitSet和settleSet
    int newWaitSet[MAX_VC_VM_NUM];
    int newSettleSet[MAX_VC_VM_NUM];

    while(!switchque.empty())
    {
        ScheduleQueNode cur = switchque.front();
        switchque.pop();
        int switchId = cur.id;
        int newWaitSetSize = cur.waitSetSize;
        int newSettleSetSize = cur.settleSetSize;
        for(int i = 0; i < vc[vcId].vmCnt; i++)
        {
            newWaitSet[i] = cur.waitSet[i];
            newSettleSet[i] = cur.settleSet[i];
        }
        settleInSwitch(vcId, switchId, newWaitSet, newWaitSetSize, newSettleSet, newSettleSetSize);
        //settleInSwitch(vcId, switchId, cur.waitSet, cur.waitSetSize, cur.settleSet, cur.settleSetSize);

        //todo
        if(newSettleSetSize == vc[vcId].vmCnt)   //以当前节点为根的子树已经可以盛放所有vm
        {
            canMigrate = true;
            break;
        }
        else if(switches[switchId].level == 1)   //当前已经是交换机的根节点了
        {
            canMigrate = false;
            break;
        }
        else    //以当前节点为根的子树还不能盛放所有vm
        {
            //计算子节点的上行带宽
            int curOutBandwidth = 0;
            for(int i = 0; i < vc[vcId].vmCnt; i++)
            {
                if(newSettleSet[i] == 0)//说明这个点在外部
                {
                    for(int j = vc[vcId].head[i]; j != -1; j = vc[vcId].edge[j].next)
                    {
                        int v = vc[vcId].edge[j].to;
                        if(newSettleSet[v] == 1)   //去点在内部
                        {
                            curOutBandwidth += vc[vcId].edge[j].bandwidthCost;
                        }
                    }
                }
            }

            if(curOutBandwidth <= tryswitches[switchId].upbandwidth)    //带宽足够
            {
                tryswitches[switchId].upbandwidth -= curOutBandwidth;

                //上传
                if(preParentId == -1)   //第一次进入这个循环
                {
                    preParentId = switches[switchId].parent;
                    parentNode.init(switches[switchId].parent, vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }
                else if(switches[switchId].parent == parentNode.id)//与上一个交换机的父节点相同
                {
                    parentNode.update(vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }
                else if(switches[switchId].parent != parentNode.id)    //与上一个交换机的父节点不同
                {
                    switchque.push(parentNode); //压入上一个交换机节点
                    preParentId = switches[switchId].parent;
                    parentNode.init(switches[switchId].parent, vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }

            }
            else    //带宽不足
            {
                //把settleSet中的vm的location信息清空
                for(int i = 0; i < vc[vcId].vmCnt; i++)
                {
                    if(newSettleSet[i] == 1)
                    {
                        vc[vcId].vm[i].location = -1;
                    }
                }

                //把settleSet中的点重新拿到waitSet中
                for(int i = 0; i < vc[vcId].vmCnt; i++)
                {
                    if(newSettleSet[i] == 1)
                    {
                        //DEBUG
                        if(newWaitSet[i] != 0)
                        {
                            int a = 0;
                            a++;
                        }

                        newWaitSet[i] = 1;
                        newWaitSetSize++;
                        newSettleSet[i] = 0;
                        newSettleSetSize--;
                    }
                }

                //上传
                if(preParentId == -1)   //第一次进入这个循环
                {
                    preParentId = switches[switchId].parent;
                    parentNode.init(switches[switchId].parent, vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }
                else if(switches[switchId].parent == parentNode.id)//与上一个交换机的父节点相同
                {
                    parentNode.update(vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }
                else if(switches[switchId].parent != parentNode.id)    //与上一个交换机的父节点不同
                {
                    switchque.push(parentNode); //压入上一个交换机节点
                    preParentId = switches[switchId].parent;
                    parentNode.init(switches[switchId].parent, vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }
            }
        }

        if(switchque.empty())   //todo：不知道对不对，一个比较抽搐的写法：能走到这里说明迭代还没有结束（上面有个break），而队列为空说明还有一个父节点没有被压入队列
        {
            switchque.push(parentNode); //压入“最后一个”交换机节点
        }
    }

    if(canMigrate == true)
    {
        migCnt++;   //在需要迁移且成功迁移时，统计一下迁移次数
        updateMigMemoryCnt(vcId);

        //updateFromTryTree();
        deletePreVCResource();
        updateFromVC(vcId);
        return true;
    }
    else if(canMigrate == false)
    {
        rollbackVC(vcId);
        rollbackTryTree();
        return false;
    }
}
