#include <vector>
#include <functional>
#include "SVGigE.h"
#include "Camera.h"

SVSCAMERA_API class SVSCamera
{
private:
    SVSCamera(const SVSCamera&);
    const SVSCamera& operator=(const SVSCamera&);
 
    CameraContainerClient_handle m_CameraContainer;
    std::vector<Camera_handle> m_CameraList;
    std::vector<StreamingChannel_handle> m_StreamChannelList;
    //图像参数
    std::vector<int> m_ImgWidthList, m_ImgHeightList;
    std::vector<unsigned char*> m_RGBImgBuffer;
    //相机参数
    std::vector<float> m_ExposureTimeList;
    //图像回调
    //typedef std::function <SVGigE_RETURN (Image_handle, void*)> m_Callback;
    //union {
    //    void(__stdcall *ThreadProc)(void *, Image_handle, void*);
    //    void(__stdcall SVSCamera::*m_pImgCallback)(Image_handle, void*);
    //} Proc;
    //SVGigE_RETURN __stdcall m_ImgCallback(Image_handle, void*);

public:
    SVSCamera();
    ~SVSCamera();


    //1 获取相机列表
    /*返回一个const的camera list，使用者需要按需组织下标数组*/
    const std::vector<std::string> getCameraList();
    //2 打开指定相机
    /*参数为指定相机在camera list中的下标*/
    bool openCamera(std::vector<int>);
    ////3 设置指定相机回调
    ///*参数为指定相机在camera list中的下标和一个函数指针组成的map*/
    //bool setCameraCB(std::map<int, CB_FUNC>);
    //4 开始指定相机抓图
    /*参数为指定相机在camera list中的下标*/
    bool startGrab(std::vector<int>);
    //5 停止指定相机抓图
    /*参数为指定相机在camera list中的下标*/
    bool stopGrab(std::vector<int>);
    //6 关闭指定相机
    /*参数为指定相机在camera list中的下标*/
    bool closeCamera(std::vector<int>);
    //7 抓取单张图片
    bool singleGrab(std::vector<int>);
    //8 获取Bayer变换后的RGB图像
    bool getRGBBuffer(std::vector<int>,unsigned char**);
    //9 获取图像宽和高
    bool getImgWidth(std::vector<int>,int&);
    bool getImgHeight(std::vector<int>,int&);
    //// 关闭全部相机，用于在析构时关闭打开的相机。
    //bool closeCamera();

    //设置触发模式
    bool setExposureTime(std::vector<int>,float);
};
