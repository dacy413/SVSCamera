#include "../SVSCamera/SVSCamera.h"
#include <vector>
//#pragma comment(lib,"../Debug/SVSCamera.lib")
#pragma comment(lib,"../Release/SVSCamera.lib")
#include <windows.h>
int main()
{
    //SVSCamera sc;
    //std::vector<std::string> ts = sc.getCameraList();

    //if (ts.size() == 0)
    //{
    //    return 0;
    //}
    //std::vector<int> tmp = { 0 };
    //bool res;
    //res = sc.openCamera(tmp);
    //res = sc.setExposureTime(tmp, 20000);
    //res = sc.startGrab(tmp);
    //for (int i = 0; i < 10; ++i)
    //{
    //    printf("IMAGE %d\n", i);
    //    res = sc.singleGrab(tmp);
    //}
    //res = sc.stopGrab(tmp);
    //res = sc.closeCamera(tmp);
    /*========================================*/
    if (!openCamera())
    {
        return 0;
    }
    setExposureTime(70000);
    startGrab();
    unsigned char* t1 = NULL;
    int t2 = 0;
    int t3 = 0;
    for (int i = 0; i < 10; ++i)
    {
        printf("IMAGE %d\n", i);
        t1 = singleGrab(&t2, &t3);
        printf("====>> %x %d %d\n", t1, t2, t3);
    }
    stopGrab();
    closeCamera();
    return 1;
}