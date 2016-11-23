#include "head.h"

int main()
{
    srand((unsigned)time(NULL));

    init();
    build();
    loadVcTemplate();

    generatePoissonSeq();

    int totalTime = 10 * 3600;   //����������ʱ�� ��λ����
    int curTime = 0;
    while(curTime < totalTime)
    {
        cout << curTime << endl;
        terminateJudge(curTime);

        int vcId;
        while((vcId = generateNewReq(curTime)) != -1)    //����ͳһʱ�̿����ж�����󵽴�
        {
            newReqTotCnt++;
            if(placement(vcId) == true)
                newReqAcCnt++;
        }

        for(int i = 0; i < SCALE_REQ_SEQ_NUM; i++)  //6�߳�����
        {
            int vcId, reqMode;
            while((vcId = generateScaleReq(i, curTime, reqMode)) != -1)  //����ͳһʱ�̿����ж�����󵽴�
            {
                scaleReqTotCnt++;
                if(scheduler(vcId, reqMode) == true)
                    scaleReqAcCnt++;
            }
        }

        curTime++;
    }


	return 0;
}
