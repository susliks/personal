#include "head.h"

int main()
{
    srand((unsigned)time(NULL));

    init();
    build();
    firstCopyTryTree();
    loadVcTemplate();

    generatePoissonSeq();

    int totalTime = 10 * 3600;   //����������ʱ�� ��λ����
    int curTime = 0;
    while(curTime < totalTime)
    {
        //DEBUG
        if(curTime == 1000)
        {
            int a;
            a ++;
        }

        cout << curTime << endl;
        terminateJudge(curTime);

        int vcId;
        while((vcId = generateNewReq(curTime)) != -1)    //����ͳһʱ�̿����ж�����󵽴�
        {
            newReqTotCnt++;
            if(placement(vcId, curTime) == true)
                newReqAcCnt++;
        }

        for(int i = 0; i < SCALE_REQ_SEQ_NUM; i++)  //6�߳�����
        {
            int reqMode;
            while((vcId = generateScaleReq(i, curTime, reqMode)) != -1)  //����ͳһʱ�̿����ж�����󵽴�
            {
                scaleReqTotCnt++;
                if(scheduler(vcId, reqMode) == true)
                    scaleReqAcCnt++;
            }
        }

        curTime++;
    }

    cout << newReqAcCnt << endl;
    cout << newReqTotCnt << endl;

    //DEBUG
    cout << activeVcCnt << endl;

    printEvaluationResult();

	return 0;
}
