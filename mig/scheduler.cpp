#include "head.h"

//��һ�δ��������������ݶ��������Ƶ�����
//ִֻ��һ�Σ�֮�󶼻�����������
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

//VC����ʱ��ɾ�������ϸ�vc��ռ��Դ
void deleteVCResource(int vcId)
{
    //����CPU��MEMORY��Դ
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        int serverId = vc[vcId].vm[i].location;
        server[serverId].cpu += vc[vcId].vm[i].cpuCost;
        server[serverId].memory += vc[vcId].vm[i].memoryCost;
    }

    //����BandWidth��Դ ���ζԱ���LCA
    for(int i = 0; i < vc[vcId].edgeCnt; i++)
    {
        int u = vc[vcId].link[i].u;
        int v = vc[vcId].link[i].v;

        int uLoc = vc[vcId].vm[u].location;
        int vLoc = vc[vcId].vm[v].location;
        //server��
        if(uLoc != vLoc)
        {
            server[uLoc].upbandwidth += vc[vcId].link[i].bandwidthCost;
            server[vLoc].upbandwidth += vc[vcId].link[i].bandwidthCost;

            uLoc = server[uLoc].parent;
            vLoc = server[vLoc].parent;
        }
        //switch��
        while(uLoc != vLoc)
        {
            switches[uLoc].upbandwidth += vc[vcId].link[i].bandwidthCost;
            switches[vLoc].upbandwidth += vc[vcId].link[i].bandwidthCost;

            uLoc = switches[uLoc].parent;
            vLoc = switches[vLoc].parent;
        }
    }
}

//��������ɾ����ǰvc��ԭ״̬��ռ�õ���Դ
void deletePreVCResource()
{
    //����CPU��MEMORY��Դ
    for(int i = 0; i < vcPreState.vmCnt; i++)
    {
        int serverId = vcPreState.vm[i].location;
        server[serverId].cpu += vcPreState.vm[i].cpuCost;
        server[serverId].memory += vcPreState.vm[i].memoryCost;
    }

    //����BandWidth��Դ ���ζԱ���LCA
    for(int i = 0; i < vcPreState.edgeCnt; i++)
    {
        int u = vcPreState.link[i].u;
        int v = vcPreState.link[i].v;

        int uLoc = vcPreState.vm[u].location;
        int vLoc = vcPreState.vm[v].location;
        //server��
        if(uLoc != vLoc)
        {
            server[uLoc].upbandwidth += vcPreState.link[i].bandwidthCost;
            server[vLoc].upbandwidth += vcPreState.link[i].bandwidthCost;

            uLoc = server[uLoc].parent;
            vLoc = server[vLoc].parent;
        }
        //switch��
        while(uLoc != vLoc)
        {
            switches[uLoc].upbandwidth += vcPreState.link[i].bandwidthCost;
            switches[vLoc].upbandwidth += vcPreState.link[i].bandwidthCost;

            uLoc = switches[uLoc].parent;
            vLoc = switches[vLoc].parent;
        }
    }
}

//��vc����������
void updateFromVC(int vcId) //��������Դ�Ƿ�������ж�
{
    //����CPU��MEMORY��Դ
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        int serverId = vc[vcId].vm[i].location;
        server[serverId].cpu -= vc[vcId].vm[i].cpuCost;
        server[serverId].memory -= vc[vcId].vm[i].memoryCost;
    }

    //����BandWidth��Դ ���ζԱ���LCA
    for(int i = 0; i < vc[vcId].edgeCnt; i++)
    {
        int u = vc[vcId].link[i].u;
        int v = vc[vcId].link[i].v;

        int uLoc = vc[vcId].vm[u].location;
        int vLoc = vc[vcId].vm[v].location;
        //server��
        if(uLoc != vLoc)
        {
            server[uLoc].upbandwidth -= vc[vcId].link[i].bandwidthCost;
            server[vLoc].upbandwidth -= vc[vcId].link[i].bandwidthCost;

            uLoc = server[uLoc].parent;
            vLoc = server[vLoc].parent;
        }
        //switch��
        while(uLoc != vLoc)
        {
            switches[uLoc].upbandwidth -= vc[vcId].link[i].bandwidthCost;
            switches[vLoc].upbandwidth -= vc[vcId].link[i].bandwidthCost;

            uLoc = switches[uLoc].parent;
            vLoc = switches[vLoc].parent;
        }
    }
}



//��������������
//���³ɹ�ʱ����
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

//��������ԭ����
//����ʧ��ʱ����
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
    //�ڽӱ��˫���
    vc.edge[vc.si].to = v;
    vc.edge[vc.si].bandwidthCost = cost;
    vc.edge[vc.si].next = vc.head[u];
    vc.head[u] = vc.si++;
    vc.edge[vc.si].to = u;
    vc.edge[vc.si].bandwidthCost = cost;
    vc.edge[vc.si].next = vc.head[v];
    vc.head[v] = vc.si++;
}

void loadVcTemplate()   //��¼����vmLoc�����������Ϣ
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
    //��VC��ԭ״̬(ֻ��Ҫ��ע������Դ���ӻ�������ԭ����
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


void settleInServer(int vcId, int serverId, int waitSet[], int &waitSetSize, int settleSet[], int &settleSetSize)//��Ҫ����waitSetSize��ֵ
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
    int tmpMap[MAX_VC_VM_NUM];              //������w��v���鵽waitSet���±�ӳ��

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

    //memset(knapsackDp, 0, sizeof(knapsackDp));    //ֻ��Ҫ��ʼ��һά
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
        if(knapsackPath[k][tmpi][tmpj] == 1)//ѡ���˵�ǰ��Ʒ
        {
            settleSet[tmpMap[k]] = 1;   //ע������Ҫʹ��ӳ������ص�waitSet/settleSet����״̬
            //ѡ���˵�ǰ��Ʒ��˵�������������һ��״̬ʱknapsackPath[k+1][tmpi-w1[k]][tmpj-w2[k]]
            settleSetSize++;
            tmpi -= w1[k];
            tmpj -= w2[k];
        }
    }

    //heuristicSearch part
    int b1[MAX_VC_VM_NUM];
    int b2[MAX_VC_VM_NUM];
    int curOutBandwidth = 0;

    //�������������ʱ��˳�����B1��B2ֵ
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        if(settleSet[i] == 1)
        {
            b1[i] = b2[i] = 0;
            for(int j = vc[vcId].head[i]; j != -1; j = vc[vcId].edge[j].next)
            {
                int v = vc[vcId].edge[j].to;
                if(settleSet[v] == 1)   //ȥ�����ڲ�
                {
                    b1[i] += vc[vcId].edge[j].bandwidthCost;
                }
                else                    //ȥ�����ⲿ
                {
                    b2[i] += vc[vcId].edge[j].bandwidthCost;
                    curOutBandwidth += vc[vcId].edge[j].bandwidthCost;
                }
            }
        }
    }

    while(settleSetSize > 0 && curOutBandwidth > tryserver[serverId].upbandwidth)
    {
        //�������������һ��ɾ��
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

        //���¼������������ʱ��˳�����B1��B2ֵ
        curOutBandwidth = 0;
        for(int i = 0; i < vc[vcId].vmCnt; i++)
        {
            if(settleSet[i] == 1)
            {
                b1[i] = b2[i] = 0;
                for(int j = vc[vcId].head[i]; j != -1; j = vc[vcId].edge[j].next)
                {
                    int v = vc[vcId].edge[j].to;
                    if(settleSet[v] == 1)   //ȥ�����ڲ�
                    {
                        b1[i] += vc[vcId].edge[j].bandwidthCost;
                    }
                    else                    //ȥ�����ⲿ
                    {
                        b2[i] += vc[vcId].edge[j].bandwidthCost;
                        curOutBandwidth += vc[vcId].edge[j].bandwidthCost;
                    }
                }
            }
        }
    }

    //��settleSet����waitSet
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

    //���¸�VC������vm��location
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        if(settleSet[i] == 1)
        {
            vc[vcId].vm[i].location = serverId;
        }
    }

    //��settleSet������Դʹ�����
    for(int i = 0; i < vc[vcId].vmCnt; i++) //(�ڴ桢cpu��
    {
        if(settleSet[i] == 1)
        {
            tryserver[serverId].cpu -= vc[vcId].vm[i].cpuCost;
            tryserver[serverId].memory -= vc[vcId].vm[i].memoryCost;
        }
    }
    tryserver[serverId].upbandwidth -= curOutBandwidth;//(����


    if(settleSetSize == preWaitSetSize) //˵����ǰ�ڵ�û�ж�����Ҫ���������ߣ�˵�����ܻ����Լ�����
        visited[0][serverId] = 0;
}


void settleInSwitch(int vcId, int switchId, int waitSet[], int &waitSetSize, int settleSet[], int &settleSetSize)
{
    //cout << "enter settleInSwitch" << endl;

    int preWaitSetSize = waitSetSize;

    visited[1][switchId] = 1;

    //����δ���ʹ����ӽڵ�
    for(int i = 0; i < switches[switchId].childcnt && waitSetSize > 0; i++) //δ������������δ���ʵ��ӽڵ�
    {
        if(switches[switchId].level == 3)   //˵���ӽڵ��Ƿ�����
        {
            int childId = switches[switchId].child[i];
            if(visited[0][childId]) //�����ǰ�ڵ��Ѿ������ʹ�
                continue;

            //todo
            int childSettleSet[MAX_VC_VM_NUM] = {0};
            int childSettleSetSize = 0;
            settleInServer(vcId, childId, waitSet, waitSetSize, childSettleSet, childSettleSetSize);
            for(int i = 0; i < vc[vcId].vmCnt; i++) //�ϲ�settleSet
            {
                if(childSettleSet[i] == 1)
                {
                    settleSet[i] = 1;
                    settleSetSize++;
                }
            }
        }
        else    //˵���ӽڵ��ǽ�����
        {
            int childId = switches[switchId].child[i];
            if(visited[1][childId])  //�����ǰ�ڵ��Ѿ������ʹ�
                continue;

            //todo
            int childSettleSet[MAX_VC_VM_NUM] = {0};
            int childSettleSetSize = 0;
            settleInSwitch(vcId, childId, waitSet, waitSetSize, childSettleSet, childSettleSetSize);
            for(int i = 0; i < vc[vcId].vmCnt; i++) //�ϲ�settleSet
            {
                if(childSettleSet[i] == 1)
                {
                    settleSet[i] = 1;
                    settleSetSize++;
                }
            }
        }
    }

    //todo:�������ߡ����ϴ�

    //�����ӽڵ�����д���
    int curOutBandwidth = 0;
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        if(settleSet[i] == 0)//˵����������ⲿ
        {
            for(int j = vc[vcId].head[i]; j != -1; j = vc[vcId].edge[j].next)
            {
                int v = vc[vcId].edge[j].to;
                if(settleSet[v] == 1)   //ȥ�����ڲ�
                {
                    curOutBandwidth += vc[vcId].edge[j].bandwidthCost;
                }
            }
        }
    }

    if(curOutBandwidth <= tryswitches[switchId].upbandwidth)    //�����㹻
    {
        //nothing to do
    }
    else    //������
    {
        //��settleSet�е�vm��location��Ϣ���
        for(int i = 0; i < vc[vcId].vmCnt; i++)
        {
            if(settleSet[i] == 1)
            {
                vc[vcId].vm[i].location = -1;
            }
        }

        //��settleSet�еĵ������õ�waitSet��
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

    if(settleSetSize == preWaitSetSize) //˵����ǰ�ڵ���ܻ����Լ�����
        visited[1][switchId] = 0;
}

//����true��ʾ���������Ա����㣬����false��ʾ�ܾ��������
bool scheduler(int vcId, int reqMode)
{
    //�����Ƿ��е���vm��ĳ����Դ�����������ֵ
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

    //���Ƿ���ҪǨ��,˳���������
    bool needToMigrate = false;

    if(reqMode == 0 || reqMode == 1)
    {
        for(int i = 0; i < vc[vcId].edgeCnt && needToMigrate == false; i++)
        {
            int u = vc[vcId].link[i].u;
            int v = vc[vcId].link[i].v;

            //int tmpStack[10]; //�ݴ������ڵ�֮��ıߣ����㽻�����Ľṹ�±������ᳬ��6
            //int tmpStackCnt = 0;

            int uLoc = vc[vcId].vm[u].location;
            int vLoc = vc[vcId].vm[v].location;
            //server��
            if(uLoc != vLoc)
            {
                tryserver[uLoc].upbandwidth -= (vc[vcId].link[i].bandwidthCost - vcPreState.link[i].bandwidthCost);
                tryserver[vLoc].upbandwidth -= (vc[vcId].link[i].bandwidthCost - vcPreState.link[i].bandwidthCost);
                if(tryserver[uLoc].upbandwidth < 0 || tryserver[vLoc].upbandwidth < 0)
                    needToMigrate = true;

                //�����ؼ�¼�������޸�λ��
                trytreeServerUpdateLoc[trytreeServerUpdateLocCnt++] = uLoc;
                trytreeServerUpdateLoc[trytreeServerUpdateLocCnt++] = vLoc;

                uLoc = tryserver[uLoc].parent;
                vLoc = tryserver[vLoc].parent;
            }
            //switch��
            while(uLoc != vLoc)
            {
                tryswitches[uLoc].upbandwidth -= (vc[vcId].link[i].bandwidthCost - vcPreState.link[i].bandwidthCost);
                tryswitches[vLoc].upbandwidth -= (vc[vcId].link[i].bandwidthCost - vcPreState.link[i].bandwidthCost);
                if(tryswitches[uLoc].upbandwidth < 0 || tryswitches[vLoc].upbandwidth < 0)
                    needToMigrate = true;

                //�����ؼ�¼�������޸�λ��
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

            //�����ؼ�¼�������޸�λ��
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

            //�����ؼ�¼�������޸�λ��
            trytreeServerUpdateLoc[trytreeServerUpdateLocCnt++] = serverId;
        }
    }



    //������ҪǨ�ƣ���������������㣩����ֱ����������������,�˳�
    if(needToMigrate == false)
    {
        updateFromTryTree();
        return true;
    }


    //�ܴ�������������˵����ҪǨ��
    rollbackTryTree();

    //��ÿ��VM��location����Ϊ-1����ʾ��ǰ��û�з��õ�λ��
    for(int i = 0; i < vc[vcId].vmCnt; i++)
    {
        vc[vcId].vm[i].location = -1;
    }

    memset(visited, 0, sizeof(visited));    //��շ��ʱ������

    //��������ɾ����VC��ռ��Դ
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

    //�Է�
    bool canMigrate = false;
    queue<ScheduleQueNode> serverque;
    queue<ScheduleQueNode> switchque;
    //����һ��VC������VM���������мӷ�����
    map<int, int> serverMap;    //serverId��ɢ����������ԭserverId����ɢ�����server��id��ӳ��
    int tmpServerCnt = 0;
    vector<int> tmpvec[MAX_VC_VM_NUM];
    //�Ȱ���������λ�����������vm
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
    //�ѵ�ǰvc���õ��ķ�����������ŵ㼯�������
    for(map<int, int>::iterator it = serverMap.begin(); it != serverMap.end(); it++)    //��ΪmapĬ����keyֵ�������������������Ա�֤˳�����server
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

    //��������������
    ScheduleQueNode parentNode;
    int preParentId = -1;
    while(!serverque.empty())
    {
        ScheduleQueNode cur = serverque.front();
        serverque.pop();
        int serverId = cur.id;

        settleInServer(vcId, serverId, cur.waitSet, cur.waitSetSize, cur.settleSet, cur.settleSetSize);

        //�����ǰ������������ʢ�����е�VM����Ҫ���丸�ڵ�ѹ�뽻��������

        if(cur.settleSetSize < vc[vcId].vmCnt)
        {
            if(preParentId == -1)   //��һ�ν������ѭ��
            {
                preParentId = server[serverId].parent;
                parentNode.init(server[serverId].parent, vc[vcId].vmCnt, cur.waitSet, cur.settleSet);
            }
            else if(server[serverId].parent == parentNode.id)//����һ���������ĸ��ڵ���ͬ
            {
                parentNode.update(vc[vcId].vmCnt, cur.waitSet, cur.settleSet);
            }
            else if(server[serverId].parent != parentNode.id)    //����һ���������ĸ��ڵ㲻ͬ
            {
                switchque.push(parentNode); //ѹ����һ���������ڵ�
                preParentId = server[serverId].parent;
                parentNode.init(server[serverId].parent, vc[vcId].vmCnt, cur.waitSet, cur.settleSet);
            }
        }
    }
    if(preParentId != -1)
        switchque.push(parentNode); //ѹ�����һ���������ڵ�


    //��������������
    preParentId = -1;   //�������桰���������������̵����ݽṹ

    //�����µ�waitSet��settleSet
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
        if(newSettleSetSize == vc[vcId].vmCnt)   //�Ե�ǰ�ڵ�Ϊ���������Ѿ�����ʢ������vm
        {
            canMigrate = true;
            break;
        }
        else if(switches[switchId].level == 1)   //��ǰ�Ѿ��ǽ������ĸ��ڵ���
        {
            canMigrate = false;
            break;
        }
        else    //�Ե�ǰ�ڵ�Ϊ��������������ʢ������vm
        {
            //�����ӽڵ�����д���
            int curOutBandwidth = 0;
            for(int i = 0; i < vc[vcId].vmCnt; i++)
            {
                if(newSettleSet[i] == 0)//˵����������ⲿ
                {
                    for(int j = vc[vcId].head[i]; j != -1; j = vc[vcId].edge[j].next)
                    {
                        int v = vc[vcId].edge[j].to;
                        if(newSettleSet[v] == 1)   //ȥ�����ڲ�
                        {
                            curOutBandwidth += vc[vcId].edge[j].bandwidthCost;
                        }
                    }
                }
            }

            if(curOutBandwidth <= tryswitches[switchId].upbandwidth)    //�����㹻
            {
                tryswitches[switchId].upbandwidth -= curOutBandwidth;

                //�ϴ�
                if(preParentId == -1)   //��һ�ν������ѭ��
                {
                    preParentId = switches[switchId].parent;
                    parentNode.init(switches[switchId].parent, vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }
                else if(switches[switchId].parent == parentNode.id)//����һ���������ĸ��ڵ���ͬ
                {
                    parentNode.update(vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }
                else if(switches[switchId].parent != parentNode.id)    //����һ���������ĸ��ڵ㲻ͬ
                {
                    switchque.push(parentNode); //ѹ����һ���������ڵ�
                    preParentId = switches[switchId].parent;
                    parentNode.init(switches[switchId].parent, vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }

            }
            else    //������
            {
                //��settleSet�е�vm��location��Ϣ���
                for(int i = 0; i < vc[vcId].vmCnt; i++)
                {
                    if(newSettleSet[i] == 1)
                    {
                        vc[vcId].vm[i].location = -1;
                    }
                }

                //��settleSet�еĵ������õ�waitSet��
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

                //�ϴ�
                if(preParentId == -1)   //��һ�ν������ѭ��
                {
                    preParentId = switches[switchId].parent;
                    parentNode.init(switches[switchId].parent, vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }
                else if(switches[switchId].parent == parentNode.id)//����һ���������ĸ��ڵ���ͬ
                {
                    parentNode.update(vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }
                else if(switches[switchId].parent != parentNode.id)    //����һ���������ĸ��ڵ㲻ͬ
                {
                    switchque.push(parentNode); //ѹ����һ���������ڵ�
                    preParentId = switches[switchId].parent;
                    parentNode.init(switches[switchId].parent, vc[vcId].vmCnt, newWaitSet, newSettleSet);
                }
            }
        }

        if(switchque.empty())   //todo����֪���Բ��ԣ�һ���Ƚϳ鴤��д�������ߵ�����˵��������û�н����������и�break����������Ϊ��˵������һ�����ڵ�û�б�ѹ�����
        {
            switchque.push(parentNode); //ѹ�롰���һ�����������ڵ�
        }
    }

    if(canMigrate == true)
    {
        migCnt++;   //����ҪǨ���ҳɹ�Ǩ��ʱ��ͳ��һ��Ǩ�ƴ���
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
