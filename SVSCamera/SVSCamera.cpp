#include "SVSCamera.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <Windows.h>

std::mutex g_Mutex;
std::condition_variable g_CV;
bool g_ImgStatus = false;
unsigned char *g_ImgBuffer = NULL;

SVGigE_RETURN __stdcall ImgCallback(Image_handle img_handle, void* context)
{
    std::unique_lock<std::mutex> t_lock(g_Mutex);
    g_ImgStatus = true;
    g_ImgBuffer = Image_getDataPointer(img_handle);
    int t_ImgSizeX = Image_getSizeX(img_handle);
    int t_ImgSizeY = Image_getSizeY(img_handle);
    int t_ImgID = Image_getImageID(img_handle);
    GVSP_PIXEL_TYPE t_ImgType = Image_getPixelType(img_handle);
    printf("IMAGE TYPE:  %d\n", t_ImgType);
    printf("IMAGE ADDR:  %08x\n", g_ImgBuffer);
    printf("IMAGE SIZE X:  %d\n", t_ImgSizeX);
    printf("IMAGE SIZE Y:  %d\n", t_ImgSizeY);
    printf("IMAGE ID:  %d\n", t_ImgID);
    //char filename[128];
    //sprintf_s(filename, 128, "RGB%d.bmp", t_ImgID);
    int t_ImgSize = Image_getImageSize(img_handle);
    SVGigE_RETURN res;
    res = Image_getImageRGB(img_handle, (unsigned char*)context, t_ImgSize*3, BAYER_METHOD_HQLINEAR);
    //res = SVGigE_writeImageToBitmapFile(filename, (unsigned char*)context, t_ImgSizeX, t_ImgSizeY, GVSP_PIX_RGB24);
    /*保存灰度图像格式为MONO8*/
    //SVGigE_RETURN res = SVGigE_writeImageToBitmapFile(filename, g_ImgBuffer, t_ImgSizeX, t_ImgSizeY, GVSP_PIX_MONO8);
    g_CV.notify_one();
    Image_release(img_handle);
    return SVGigE_SUCCESS;
}

SVSCamera::SVSCamera()
{
}
SVSCamera::~SVSCamera()
{
}

const std::vector<std::string> SVSCamera::getCameraList()
{
    std::vector<std::string> t_SerialNameList;
    m_CameraContainer = CameraContainer_create(SVGigETL_TypeFilter);
    CameraContainer_discovery(m_CameraContainer);
    int t_CameraNumber = CameraContainer_getNumberOfCameras(m_CameraContainer);
    if (t_CameraNumber == 0)
    {
        printf("CAMERA NUMBER IS 0!\n");
        return t_SerialNameList;
    }
    else
    {
        printf("CAMERA NUMBER IS %d!\n", t_CameraNumber);
        for (int i = 0; i < t_CameraNumber; i++)
        {
            Camera_handle t_Camera;
            t_Camera = CameraContainer_getCamera(m_CameraContainer, i);
            m_CameraList.push_back(t_Camera);
            t_SerialNameList.push_back(Camera_getSerialNumber(t_Camera));
        }
    }
    //初始化参数数组
    m_ExposureTimeList.resize(t_CameraNumber);
    m_ImgHeightList.resize(t_CameraNumber);
    m_ImgWidthList.resize(t_CameraNumber);
    m_StreamChannelList.resize(t_CameraNumber);
    m_RGBImgBuffer.resize(t_CameraNumber);
    return t_SerialNameList;
}

bool SVSCamera::openCamera(std::vector<int> index_list)
{
    for (auto i : index_list)
    {
        if (i > m_CameraList.size())
        {
            printf("OPEN CAMERA INDEX OUT OF RANGE!\n");
            return false;
        }
        if (Camera_openConnection(m_CameraList[i], 4) != SVGigE_SUCCESS)
        {
            printf("OPEN CAMERA %d ERROR!\n",i);
            return false;
        }

        //相机属性处理
        Camera_getImagerHeight(m_CameraList[i],&m_ImgHeightList[i]);
        Camera_getImagerWidth(m_CameraList[i], &m_ImgWidthList[i]);
        
        //RGB TMP buffer
        unsigned char* t_RGBImgBuffer = (unsigned char *)GlobalAlloc(GMEM_FIXED, 3 * m_ImgHeightList[i] * m_ImgWidthList[i]);
        //unsigned char* t_RGBImgBuffer = (unsigned char*)malloc(3 * m_ImgHeightList[i] * m_ImgWidthList[i]);
        m_RGBImgBuffer[i] = t_RGBImgBuffer;

        int t_PacketSize;
        Camera_evaluateMaximalPacketSize(m_CameraList[i], &t_PacketSize);
        
        Camera_setAcquisitionMode(m_CameraList[i], ACQUISITION_MODE_SOFTWARE_TRIGGER);
        StreamingChannel_handle t_StreamChannelHandle = NULL;
        //StreamCallback t_Callback = std::bind(&SVSCamera::m_ImgCallback, this, std::placeholders::_1,std::placeholders::_2);
        //StreamingChannel_create(&t_StreamChannelHandle, m_CameraContainer, m_CameraList[i], 1, (StreamCallback)&t_Callback, 0);
        StreamingChannel_create(&t_StreamChannelHandle, m_CameraContainer, m_CameraList[i], 1, ImgCallback, (void*)t_RGBImgBuffer);
        m_StreamChannelList[i] = t_StreamChannelHandle;
    }
    return true;
}

bool SVSCamera::setExposureTime(std::vector<int> index_list, float et_value)
{
    for (auto i : index_list)
    {
        if (i > m_CameraList.size())
        {
            printf("SET CAMERA EXPOSURETIME OUT OF RANGE!\n");
            return false;
        }
        if (Camera_setExposureTime(m_CameraList[i], et_value) != SVGigE_SUCCESS)
        {
            printf("SET CAMERA EXPOSURE TIME %d ERROR!\n", i);
            return false;
        }
        m_ExposureTimeList[i] = et_value;
    }
    return true;
}

//SVGigE_RETURN SVSCamera::m_ImgCallback(Image_handle img_handle, void* context)
//{
//    unsigned char *buffer = Image_getDataPointer(img_handle);
//    printf("IMAGE ADDR:  %08x\n", buffer);
//    return SVGigE_SUCCESS;
//}

bool SVSCamera::startGrab(std::vector<int> index_list)
{
    for (auto i : index_list)
    {
        if (i > m_CameraList.size())
        {
            printf("START GRAB OUT OF RANGE!\n");
            return false;
        }
        if (Camera_setAcquisitionControl(m_CameraList[i], ACQUISITION_CONTROL_START) != SVGigE_SUCCESS)
        {
            printf("START GRAB %d ERROR!\n", i);
            return false;
        }
    }
    return true;
}

bool SVSCamera::stopGrab(std::vector<int> index_list)
{
    for (auto i : index_list)
    {
        if (i > m_CameraList.size())
        {
            printf("STOP GRAB OUT OF RANGE!\n");
            return false;
        }
        if (Camera_setAcquisitionControl(m_CameraList[i], ACQUISITION_CONTROL_STOP) != SVGigE_SUCCESS)
        {
            printf("STOP GRAB %d ERROR!\n", i);
            return false;
        }
    }
    return true;
}

bool SVSCamera::singleGrab(std::vector<int> index_list)
{
    for (auto i : index_list)
    {
        if (i > m_CameraList.size())
        {
            printf("SINGLE GRAB OUT OF RANGE!\n");
            return false;
        }
        if (Camera_softwareTrigger(m_CameraList[i]) != SVGigE_SUCCESS)
        {
            printf("SINGLE GRAB %d ERROR!\n", i);
            return false;
        }
    }
    g_ImgStatus = false;
    g_ImgBuffer = NULL;
    while(!g_ImgStatus)
    {
        std::unique_lock<std::mutex> t_lock(g_Mutex);
        g_CV.wait(t_lock);
    }
    return true;
}

bool SVSCamera::closeCamera(std::vector<int> index_list)
{
    for (auto i : index_list)
    {
        if (i > m_CameraList.size())
        {
            printf("CLOSE CAMERA OUT OF RANGE!\n");
            return false;
        }
        GlobalFree(m_RGBImgBuffer[i]);
        if (StreamingChannel_delete(m_StreamChannelList[i]) != SVGigE_SUCCESS)
        {
            printf("CLOSE CAMERA %d ERROR!\n", i);
            return false;
        }
        if (Camera_closeConnection(m_CameraList[i]) != SVGigE_SUCCESS)
        {
            printf("CLOSE CAMERA %d ERROR!\n", i);
            return false;
        }
    }
    return true;
}

bool SVSCamera::getRGBBuffer(std::vector<int> index_list,unsigned char** img_buffer)
{
    for (auto i : index_list)
    {
        if (i > m_CameraList.size())
        {
            printf("SINGLE GRAB OUT OF RANGE!\n");
            return false;
        }
        *img_buffer = m_RGBImgBuffer[i];
    }
    return true;
}

bool SVSCamera::getImgWidth(std::vector<int> index_list,int& img_width)
{
    for (auto i : index_list)
    {
        if (i > m_CameraList.size())
        {
            printf("SINGLE GRAB OUT OF RANGE!\n");
            return false;
        }
        img_width = m_ImgWidthList[i];
    }
    return true;
}

bool SVSCamera::getImgHeight(std::vector<int> index_list, int& img_height)
{
    for (auto i : index_list)
    {
        if (i > m_CameraList.size())
        {
            printf("SINGLE GRAB OUT OF RANGE!\n");
            return false;
        }
        img_height = m_ImgHeightList[i];
    }
    return true;
}
/*=======================================================================================*/
SVSCamera g_SC;
std::vector<int> g_CI = { 0 };
SVSCAMERA_API bool openCamera()
{
    std::vector<std::string> ts = g_SC.getCameraList();
    if (ts.size() == 0)
    {
        return false;
    }
    return g_SC.openCamera(g_CI);
}

SVSCAMERA_API bool closeCamera()
{
    return g_SC.closeCamera(g_CI);
}

SVSCAMERA_API bool startGrab()
{
    return g_SC.startGrab(g_CI);
}

SVSCAMERA_API bool stopGrab()
{
    return g_SC.stopGrab(g_CI);
}

SVSCAMERA_API bool  singleGrab(unsigned char** img_buffer, int* img_width, int* img_height)
{
    if (!g_SC.singleGrab(g_CI))
    {
        return false;
    }
    if (!g_SC.getRGBBuffer(g_CI, img_buffer))
    {
        return false;
    }
    if (!g_SC.getImgWidth(g_CI, *img_width))
    {
        return false;
    }
    if (!g_SC.getImgHeight(g_CI, *img_height))
    {
        return false;
    }
    return true;
}

SVSCAMERA_API unsigned char* singleGrab(int* img_width, int* img_height)
{
    g_SC.singleGrab(g_CI);
    unsigned char * t_ImgBuffer = NULL;
    g_SC.getRGBBuffer(g_CI, &t_ImgBuffer);
    g_SC.getImgWidth(g_CI, *img_width);
    g_SC.getImgHeight(g_CI, *img_height);
    return t_ImgBuffer;
}

SVSCAMERA_API bool setExposureTime(float value)
{
    return g_SC.setExposureTime(g_CI, value);
}

SVSCAMERA_API bool getRGBImgSize(int* img_size)
{
    int t_Width, t_Height;
    g_SC.getImgWidth(g_CI, t_Width);
    g_SC.getImgHeight(g_CI, t_Height);
    *img_size = t_Width*t_Height * 3;
    return true;
}