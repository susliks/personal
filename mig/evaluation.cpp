#include "head.h"

void updateMigMemoryCnt(int vcId)   //更新总的迁移内存数
{
    for(int i = 0; i < vcPreState.vmCnt; i++)
    {
        if(vcPreState.vm[i].location != vc[vcId].vm[i].location)
        {
            migMemoryCnt += vcPreState.vm[i].memoryCost;
        }
    }
}
