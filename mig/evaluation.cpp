#include "head.h"

void updateMigMemoryCnt(int vcId)   //�����ܵ�Ǩ���ڴ���
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
    printf("�½����������:%f\n", (double)newReqAcCnt / newReqTotCnt);
    printf("�������������:%f\n", (double)scaleReqAcCnt / scaleReqTotCnt);
    printf("Ǩ���ڴ���:%d\n", migMemoryCnt);
}
