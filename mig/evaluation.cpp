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

void printEvaluationResult()
{
    printf("新建请求接受率:%f\n", (double)newReqAcCnt / newReqTotCnt);
    printf("调整请求接受率:%f\n", (double)scaleReqAcCnt / scaleReqTotCnt);
    printf("迁移内存数:%d\n", migMemoryCnt);
}
