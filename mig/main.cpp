#include "head.h"

int main()
{
    srand((unsigned)time(NULL));

    init();
    build();
    loadVcTemplate();

    generatePoissonSeq();

    int totalTime = 10 * 3600;   //程序总运行时间 单位：秒
    int curTime = 0;
    while(curTime < totalTime)
    {
        cout << curTime << endl;
        terminateJudge(curTime);

        int vcId;
        while((vcId = generateNewReq(curTime)) != -1)    //考虑统一时刻可能有多个请求到达
        {
            newReqTotCnt++;
            if(placement(vcId) == true)
                newReqAcCnt++;
        }

        for(int i = 0; i < SCALE_REQ_SEQ_NUM; i++)  //6线程生成
        {
            int vcId, reqMode;
            while((vcId = generateScaleReq(i, curTime, reqMode)) != -1)  //考虑统一时刻可能有多个请求到达
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
